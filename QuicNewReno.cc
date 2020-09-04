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

#define DUPTHRESH 3

namespace inet {

QuicNewReno::QuicNewReno() {

    snd_cwnd=0;
    snd_wnd=0;
    snd_mss=0;

    ssthresh=0;
    recover=0;
    firstPartialACK = false;

    snd_max=0;
    lossRecovery = false;

    snd_una=0;
    dupacks=0;

    srtt = 0;
    rttvar = 3.0 / 4.0;

    rexmit_count = 0;
    rexmit_timeout = 3.0;
}

QuicNewReno::~QuicNewReno() {
    // TODO Auto-generated destructor stub
}

void QuicNewReno::SetSndCwnd(uint32 snd_cwnd){
    this->snd_cwnd=snd_cwnd;
}
int32 QuicNewReno::GetSndCwnd(){
    return snd_cwnd;
}

void QuicNewReno::SetSndWnd(uint32 snd_wnd){
    this->snd_wnd=snd_wnd;
}

void QuicNewReno::SetSndMss(uint32 snd_mss){
    this->snd_mss=snd_mss;
}
void QuicNewReno::SetSsThresh(uint32 ssthresh){
    this->ssthresh=ssthresh;
}

void QuicNewReno::SetSndMax(uint32 snd_max){
    this->snd_max=snd_max;
}
void QuicNewReno::SetSndUnA(uint32 snd_una){
    this->snd_una=snd_una;
}
void QuicNewReno::recalculateSlowStartThreshold()
{
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


    //conn->emit(ssthreshSignal, state->ssthresh);
}

void QuicNewReno::processRexmitTimer()
{

    // RFC 3782, page 6:
    // "6)  Retransmit timeouts:
    // After a retransmit timeout, record the highest sequence number
    // transmitted in the variable "recover" and exit the Fast Recovery
    // procedure if applicable."
    recover = (snd_max - 1);
 //   EV_INFO << "recover=" << recover << "\n";
    lossRecovery = false;
    firstPartialACK = false;
 //   EV_INFO << "Loss Recovery terminated.\n";

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

    //conn->emit(cwndSignal, state->snd_cwnd);

  //  EV_INFO << "Begin Slow Start: resetting cwnd to " << snd_cwnd
//            << ", ssthresh=" << ssthresh << "\n";

    //state->afterRto = true;
    //conn->retransmitOneSegment(true); ## understand which function to chenge here
}

void QuicNewReno::receivedDataAck(uint32 old_send_una)
{

    // RFC 3782, page 5:
    // "5) When an ACK arrives that acknowledges new data, this ACK could be
    // the acknowledgment elicited by the retransmission from step 2, or
    // elicited by a later retransmission.
    //
    // Full acknowledgements:
    // If this ACK acknowledges all of the data up to and including
    // "recover", then the ACK acknowledges all the intermediate
    // segments sent between the original transmission of the lost
    // segment and the receipt of the third duplicate ACK.  Set cwnd to
    // either (1) min (ssthresh, FlightSize + SMSS) or (2) ssthresh,
    // where ssthresh is the value set in step 1; this is termed
    // "deflating" the window.  (We note that "FlightSize" in step 1
    // referred to the amount of data outstanding in step 1, when Fast
    // Recovery was entered, while "FlightSize" in step 5 refers to the
    // amount of data outstanding in step 5, when Fast Recovery is
    // exited.)  If the second option is selected, the implementation is
    // encouraged to take measures to avoid a possible burst of data, in
    // case the amount of data outstanding in the network is much less
    // than the new congestion window allows.  A simple mechanism is to
    // limit the number of data packets that can be sent in response to
    // a single acknowledgement; this is known as "maxburst_" in the NS
    // simulator.  Exit the Fast Recovery procedure."
    if (lossRecovery) {
        if (snd_una - 1 >= recover) {

            // Exit Fast Recovery: deflating cwnd
            //
            // option (1): set cwnd to min (ssthresh, FlightSize + SMSS)
            uint32 flight_size = snd_max - snd_una;
            snd_cwnd = std::min(ssthresh, flight_size + snd_mss);
    //        EV_INFO << "Fast Recovery - Full ACK received: Exit Fast Recovery, setting cwnd to " << snd_cwnd << "\n";
            // option (2): set cwnd to ssthresh
            // state->snd_cwnd = state->ssthresh;
            // tcpEV << "Fast Recovery - Full ACK received: Exit Fast Recovery, setting cwnd to ssthresh=" << state->ssthresh << "\n";
            // TODO - If the second option (2) is selected, take measures to avoid a possible burst of data (maxburst)!
            //conn->emit(cwndSignal, state->snd_cwnd);

            lossRecovery = false;
            firstPartialACK = false;
      //      EV_INFO << "Loss Recovery terminated.\n";
        }
        else {
            // RFC 3782, page 5:
            // "Partial acknowledgements:
            // If this ACK does *not* acknowledge all of the data up to and
            // including "recover", then this is a partial ACK.  In this case,
            // retransmit the first unacknowledged segment.  Deflate the
            // congestion window by the amount of new data acknowledged by the
            // cumulative acknowledgement field.  If the partial ACK
            // acknowledges at least one SMSS of new data, then add back SMSS
            // bytes to the congestion window.  As in Step 3, this artificially
            // inflates the congestion window in order to reflect the additional
            // segment that has left the network.  Send a new segment if
            // permitted by the new value of cwnd.  This "partial window
            // deflation" attempts to ensure that, when Fast Recovery eventually
            // ends, approximately ssthresh amount of data will be outstanding
            // in the network.  Do not exit the Fast Recovery procedure (i.e.,
            // if any duplicate ACKs subsequently arrive, execute Steps 3 and 4
            // above).
            //
            // For the first partial ACK that arrives during Fast Recovery, also
            // reset the retransmit timer.  Timer management is discussed in
            // more detail in Section 4."

    //        EV_INFO << "Fast Recovery - Partial ACK received: retransmitting the first unacknowledged segment"<<"\n";
            // retransmit first unacknowledged segment

            //conn->retransmitOneSegment(false); //###CHANGE FUNCTION

            // deflate cwnd by amount of new data acknowledged by cumulative acknowledgement field
            snd_cwnd -= snd_una - old_send_una;

            //conn->emit(cwndSignal, state->snd_cwnd);

      //      EV_INFO << "Fast Recovery: deflating cwnd by amount of new data acknowledged, new cwnd=" << snd_cwnd << "\n";

            // if the partial ACK acknowledges at least one SMSS of new data, then add back SMSS bytes to the cwnd
            if (snd_una - old_send_una >= snd_mss) {
                snd_cwnd += snd_mss;

                //conn->emit(cwndSignal, state->snd_cwnd);

   //             EV_DETAIL << "Fast Recovery: inflating cwnd by SMSS, new cwnd=" << snd_cwnd << "\n";
            }

            // try to send a new segment if permitted by the new value of cwnd
            //sendData(false);##########

            // reset REXMIT timer for the first partial ACK that arrives during Fast Recovery
            if (lossRecovery) {
                if (!firstPartialACK) {
                    firstPartialACK = true;
         //           EV_DETAIL << "First partial ACK arrived during recovery, restarting REXMIT timer.\n";

                    //restartRexmitTimer();//WE NEED TO COPY BaseAlg
                }
            }
        }
    }
    else {
        //
        // Perform slow start and congestion avoidance.
        //
        if (snd_cwnd < ssthresh) {
      //      EV_DETAIL << "cwnd <= ssthresh: Slow Start: increasing cwnd by SMSS bytes to ";

            // perform Slow Start. RFC 2581: "During slow start, a TCP increments cwnd
            // by at most SMSS bytes for each ACK received that acknowledges new data."
            snd_cwnd += snd_mss;

            // Note: we could increase cwnd based on the number of bytes being
            // acknowledged by each arriving ACK, rather than by the number of ACKs
            // that arrive. This is called "Appropriate Byte Counting" (ABC) and is
            // described in RFC 3465. This RFC is experimental and probably not
            // implemented in real-life TCPs, hence it's commented out. Also, the ABC
            // RFC would require other modifications as well in addition to the
            // two lines below.
            //
            // int bytesAcked = state->snd_una - firstSeqAcked;
            // state->snd_cwnd += bytesAcked * state->snd_mss;

            //conn->emit(cwndSignal, state->snd_cwnd);

     //       EV_DETAIL << "cwnd=" <<snd_cwnd << "\n";
        }
        else {
            // perform Congestion Avoidance (RFC 2581)
            uint32 incr = snd_mss * snd_mss / snd_cwnd;

            if (incr == 0)
                incr = 1;

            snd_cwnd += incr;

            //conn->emit(cwndSignal, state->snd_cwnd);

            //
            // Note: some implementations use extra additive constant mss / 8 here
            // which is known to be incorrect (RFC 2581 p5)
            //
            // Note 2: RFC 3465 (experimental) "Appropriate Byte Counting" (ABC)
            // would require maintaining a bytes_acked variable here which we don't do
            //

            //EV_DETAIL << "cwnd > ssthresh: Congestion Avoidance: increasing cwnd linearly, to " << snd_cwnd << "\n";
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

    //sendData(false);
}

void QuicNewReno::receivedDuplicateAck()
{

    if (dupacks == DUPTHRESH) {    // DUPTHRESH = 3, remember where to update it #######
        if (!lossRecovery) {
            // RFC 3782, page 4:
            // "1) Three duplicate ACKs:
            // When the third duplicate ACK is received and the sender is not
            // already in the Fast Recovery procedure, check to see if the
            // Cumulative Acknowledgement field covers more than "recover".  If
            // so, go to Step 1A.  Otherwise, go to Step 1B."
            //
            // RFC 3782, page 6:
            // "Step 1 specifies a check that the Cumulative Acknowledgement field
            // covers more than "recover".  Because the acknowledgement field
            // contains the sequence number that the sender next expects to receive,
            // the acknowledgement "ack_number" covers more than "recover" when:
            //      ack_number - 1 > recover;"
            if (snd_una - 1 > recover) {
              //  EV_INFO << "NewReno on dupAcks == DUPTHRESH(=3): perform Fast Retransmit, and enter Fast Recovery:";

                // RFC 3782, page 4:
                // "1A) Invoking Fast Retransmit:
                // If so, then set ssthresh to no more than the value given in
                // equation 1 below.  (This is equation 3 from [RFC2581]).
                //      ssthresh = max (FlightSize / 2, 2*SMSS)           (1)
                // In addition, record the highest sequence number transmitted in
                // the variable "recover", and go to Step 2."
                recalculateSlowStartThreshold();
                recover = (snd_max - 1);
                firstPartialACK = false;
                lossRecovery = true;
     //           EV_INFO << " set recover=" << recover;

                // RFC 3782, page 4:
                // "2) Entering Fast Retransmit:
                // Retransmit the lost segment and set cwnd to ssthresh plus 3 * SMSS.
                // This artificially "inflates" the congestion window by the number
                // of segments (three) that have left the network and the receiver
                // has buffered."
                snd_cwnd = ssthresh + 3 * snd_mss;

                //conn->emit(cwndSignal, state->snd_cwnd);

     //           EV_DETAIL << " , cwnd=" << snd_cwnd << ", ssthresh=" << ssthresh << "\n";

                //conn->retransmitOneSegment(false);

                // RFC 3782, page 5:
                // "4) Fast Recovery, continued:
                // Transmit a segment, if allowed by the new value of cwnd and the
                // receiver's advertised window."

                //sendData(false);
            }
            else {
            //    EV_INFO << "NewReno on dupAcks == DUPTHRESH(=3): not invoking Fast Retransmit and Fast Recovery\n";

                // RFC 3782, page 4:
                // "1B) Not invoking Fast Retransmit:
                // Do not enter the Fast Retransmit and Fast Recovery procedure.  In
                // particular, do not change ssthresh, do not go to Step 2 to
                // retransmit the "lost" segment, and do not execute Step 3 upon
                // subsequent duplicate ACKs."
            }
        }
        EV_INFO << "NewReno on dupAcks == DUPTHRESH(=3): TCP is already in Fast Recovery procedure\n";
    }
    else if (dupacks > DUPTHRESH) {    // DUPTHRESH = 3
        if (lossRecovery) {
            // RFC 3782, page 4:
            // "3) Fast Recovery:
            // For each additional duplicate ACK received while in Fast
            // Recovery, increment cwnd by SMSS.  This artificially inflates the
            // congestion window in order to reflect the additional segment that
            // has left the network."
            snd_cwnd += snd_mss;

            //conn->emit(cwndSignal, state->snd_cwnd);

            EV_DETAIL << "NewReno on dupAcks > DUPTHRESH(=3): Fast Recovery: inflating cwnd by SMSS, new cwnd=" << snd_cwnd << "\n";

            // RFC 3782, page 5:
            // "4) Fast Recovery, continued:
            // Transmit a segment, if allowed by the new value of cwnd and the
            // receiver's advertised window."

            //sendData(false);
        }
    }
}


void QuicNewReno::rttMeasurementComplete(simtime_t tSent, simtime_t tAcked)
{
    //
    // Jacobson's algorithm for estimating RTT and adaptively setting RTO.
    //
    // Note: this implementation calculates in doubles. An impl. which uses
    // 500ms ticks is available from old tcpmodule.cc:calcRetransTimer().
    //

    // update smoothed RTT estimate (srtt) and variance (rttvar)
    const double g = 0.125;    // 1 / 8; (1 - alpha) where alpha == 7 / 8;
    simtime_t newRTT = tAcked - tSent;

    simtime_t& srtt = srtt;
    simtime_t& rttvar = rttvar;

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

    //conn->emit(rttSignal, newRTT);
    //conn->emit(srttSignal, srtt);
    //conn->emit(rttvarSignal, rttvar);
    //conn->emit(rtoSignal, rto);
}

void QuicNewReno::rttMeasurementCompleteUsingTS(uint32 echoedTS)
{
    // Note: The TS option is using uint32 values (ms precision) therefore we convert the current simTime also to a uint32 value (ms precision)
    // and then convert back to simtime_t to use rttMeasurementComplete() to update srtt and rttvar
    uint32 now = this->convertSimtimeToTS(simTime());
    simtime_t tSent = this->convertTSToSimtime(echoedTS);
    simtime_t tAcked = this->convertTSToSimtime(now);
    rttMeasurementComplete(tSent, tAcked);
}

simtime_t QuicNewReno::convertTSToSimtime(uint32 timestamp)
{
    ASSERT(SimTime::getScaleExp() <= -3);

    simtime_t simtime(timestamp, SIMTIME_MS);
    return simtime;
}

uint32 QuicNewReno::convertSimtimeToTS(simtime_t simtime)
{
    ASSERT(SimTime::getScaleExp() <= -3);

    uint32 timestamp = (uint32)(simtime.inUnit(SIMTIME_MS));
    return timestamp;
}

} /* namespace inet */