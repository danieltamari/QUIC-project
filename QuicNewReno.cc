//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "QuicNewReno.h"

namespace inet {

QuicNewReno::QuicNewReno() {

    snd_cwnd=0;
    snd_wnd=0;
    snd_mss=0;

    ssthresh=0;
    recover=0;

    snd_max=0;
    lossRecovery = false;
    snd_una=0;

    srtt = 0;
    rttvar = 3.0 / 4.0;
    rtt = 0;

    rexmit_timeout = 3.0;
}


QuicNewReno::~QuicNewReno() {
    // TODO Auto-generated destructor stub
}


void QuicNewReno::recalculateSlowStartThreshold() {
    // RFC 2581, page 4:
    // "When a TCP sender detects segment loss using the retransmission
    // timer, the value of ssthresh MUST be set to no more than the value
    // given in equation 3:
    //
    //   ssthresh = max (FlightSize / 2, 2*SMSS)            (3)
    //
    // As discussed above, FlightSize is the amount of outstanding data in
    // the network."

    // set ssthresh to flight size / 2, but at least 2 SMSS
    // (the formula below practically amounts to ssthresh = cwnd / 2 most of the time)
    uint32 flight_size = std::min(snd_cwnd, snd_wnd);    // FIXME TODO - Does this formula computes the amount of outstanding data?
    ssthresh = std::max(flight_size / 2, 2 * snd_mss);
}


void QuicNewReno::processRexmitTimer() {
    // RFC 3782, page 6:
    // "6)  Retransmit timeouts:
    // After a retransmit timeout, record the highest sequence number
    // transmitted in the variable "recover" and exit the Fast Recovery
    // procedure if applicable."
    recover = (snd_max - 1);
    lossRecovery = false;

    // After REXMIT timeout TCP NewReno should start slow start with snd_cwnd = snd_mss.
    //
    // If calling "retransmitData();" there is no rexmit limitation (bytesToSend > snd_cwnd)
    // therefore "sendData();" has been modified and is called to rexmit outstanding data.
    //
    // RFC 2581, page 5:
    // "Furthermore, upon a timeout cwnd MUST be set to no more than the loss
    // window, LW, which equals 1 full-sized segment (regardless of the
    // value of IW).  Therefore, after retransmitting the dropped segment
    // the TCP sender uses the slow start algorithm to increase the window
    // from 1 full-sized segment to the new value of ssthresh, at which
    // point congestion avoidance again takes over."

    // begin Slow Start (RFC 2581)
    recalculateSlowStartThreshold();
    snd_cwnd = snd_mss;
}


void QuicNewReno::receivedDataAck() {
    // RFC 3782, page 5:
    // "5) When an ACK arrives that acknowledges new data, this ACK could be
    // the acknowledgment elicited by the retransmission from step 2, or
    // elicited by a later retransmission.
    //
    if (lossRecovery) {
        lossRecovery = false;
        // Exit Fast Recovery: deflating cwnd
        // set cwnd to min (ssthresh, FlightSize + SMSS)
        uint32 flight_size = snd_max - snd_una;
        snd_cwnd = std::min(ssthresh, flight_size + snd_mss);
        EV_INFO << "flight size is " << flight_size << "\n";
        EV_INFO << "ssthresh is " << ssthresh << "\n";
        EV << "Fast Recovery - Full ACK received: Exit Fast Recovery, setting cwnd to " << snd_cwnd << "\n";
    }

    else {
        //
        // Perform slow start and congestion avoidance.
        //
        if (snd_cwnd < ssthresh) {
            // perform Slow Start. RFC 2581: "During slow start, a TCP increments cwnd
            // by at most SMSS bytes for each ACK received that acknowledges new data."
            snd_cwnd += snd_mss;
        }

        else {
            // perform Congestion Avoidance (RFC 2581)
            uint32 incr = snd_mss * snd_mss / snd_cwnd;

            if (incr == 0)
                incr = 1;

            snd_cwnd += incr;
        }
        // RFC 3782, page 13:
        // "When not in Fast Recovery, the value of the state variable "recover"
        // should be pulled along with the value of the state variable for
        // acknowledgments (typically, "snd_una") so that, when large amounts of
        // data have been sent and acked, the sequence space does not wrap and
        // falsely indicate that Fast Recovery should not be entered (Section 3,
        // step 1, last paragraph)."
        recover = (snd_una - 2);
    }
}


bool QuicNewReno::receivedDuplicateAck(int dup_ACKS)
{
    bool start_epoch = false;
    if (!lossRecovery) {
        if (snd_una - 1 > recover) {
            recalculateSlowStartThreshold();
            recover = (snd_max - 1);
            lossRecovery = true;
            start_epoch = true;

            // RFC 3782, page 4:
            // "2) Entering Fast Retransmit:
            // Retransmit the lost segment and set cwnd to ssthresh plus 3 * SMSS.
            // This artificially "inflates" the congestion window by the number
            // of segments (three) that have left the network and the receiver
            // has buffered."
            if (dup_ACKS >= 3)
                snd_cwnd = ssthresh + 3 * snd_mss;

            else
                snd_cwnd = ssthresh;
         }
        EV_INFO << "NewReno on dupAcks == DUPTHRESH(=3): TCP is already in Fast Recovery procedure\n";
    }

    return start_epoch;
}


void QuicNewReno::inflateCwnd() {
    snd_cwnd += snd_mss;
    EV_DETAIL << "NewReno on dupAcks > DUPTHRESH(=3): Fast Recovery: inflating cwnd by SMSS, new cwnd=" << snd_cwnd << "\n";

}


void QuicNewReno::rttMeasurementComplete(simtime_t tSent, simtime_t tAcked) {
    //
    // Jacobson's algorithm for estimating RTT and adaptively setting RTO.
    //
    // Note: this implementation calculates in doubles. An impl. which uses
    // 500ms ticks is available from old tcpmodule.cc:calcRetransTimer().
    //

    // update smoothed RTT estimate (srtt) and variance (rttvar)
    const double g = 0.125;    // 1 / 8; (1 - alpha) where alpha == 7 / 8;
    simtime_t newRTT = tAcked - tSent;

    simtime_t& srtt = this->srtt;
    simtime_t& rttvar = this->rttvar;

    simtime_t err = newRTT - srtt;

    srtt += g * err;
    rttvar += g * (fabs(err) - rttvar);

    // assign RTO (here: rexmit_timeout) a new value
    simtime_t rto = srtt + 4 * rttvar;

    if (rto > MAX_REXMIT_TIMEOUT)
        rto = MAX_REXMIT_TIMEOUT;
    else if (rto < MIN_REXMIT_TIMEOUT)
        rto = MIN_REXMIT_TIMEOUT;

    rexmit_timeout = rto;

    // record statistics
    EV_DETAIL << "Measured RTT=" << (newRTT * 1000) << "ms, updated SRTT=" << (srtt * 1000)
              << "ms, new RTO=" << (rto * 1000) << "ms\n";

    rtt = newRTT;
}


void QuicNewReno::rttMeasurementCompleteUsingTS(uint32 echoedTS) {
    // Note: The TS option is using uint32 values (ms precision) therefore we convert the current simTime also to a uint32 value (ms precision)
    // and then convert back to simtime_t to use rttMeasurementComplete() to update srtt and rttvar
    uint32 now = convertSimtimeToTS(simTime());
    simtime_t tSent = convertTSToSimtime(echoedTS);
    simtime_t tAcked = convertTSToSimtime(now);
    rttMeasurementComplete(tSent, tAcked);
}


simtime_t QuicNewReno::convertTSToSimtime(uint32 timestamp) {
    ASSERT(SimTime::getScaleExp() <= -3);
    simtime_t simtime(timestamp, SIMTIME_MS);
    return simtime;
}


uint32 QuicNewReno::convertSimtimeToTS(simtime_t simtime) {
    ASSERT(SimTime::getScaleExp() <= -3);
    uint32 timestamp = (uint32)(simtime.inUnit(SIMTIME_MS));
    return timestamp;
}


bool QuicNewReno::getLossRecovery() {
    return lossRecovery;
}


int32 QuicNewReno::getSndCwnd(){
    return snd_cwnd;
}


simtime_t QuicNewReno::getRto(){
    return rexmit_timeout;
}


simtime_t QuicNewReno::getRtt(){
    return rtt;
}


void QuicNewReno::setSndCwnd(uint32 snd_cwnd_){
    snd_cwnd = snd_cwnd_;
}


void QuicNewReno::setSndWnd(uint32 snd_wnd_){
    snd_wnd = snd_wnd_;
}


void QuicNewReno::setSndMss(uint32 snd_mss_){
    snd_mss=snd_mss_;
}


void QuicNewReno::setSsThresh(uint32 ssthresh_){
    ssthresh=ssthresh_;
}


void QuicNewReno::setSndMax(uint32 bytes_sent){
    snd_max+=bytes_sent;
}


void QuicNewReno::setSndUna(uint32 snd_una_){
    snd_una=snd_una_;
}



} /* namespace inet */
