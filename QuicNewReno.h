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

#ifndef INET_APPLICATIONS_QUICAPP_QUICNEWRENO_H_
#define INET_APPLICATIONS_QUICAPP_QUICNEWRENO_H_

#include <omnetpp.h>
#include "inet/common/INETDefs.h"

#define MIN_REXMIT_TIMEOUT     1.0   // 1s
#define MAX_REXMIT_TIMEOUT     240   // 2 * MSL (RFC 1122)

typedef uint32_t uint32;


namespace inet {

class QuicNewReno {
public:
    QuicNewReno();
    virtual ~QuicNewReno();

  /** Utility function to recalculate ssthresh */
  void recalculateSlowStartThreshold();

  /** Redefine what should happen on retransmission */
  void processRexmitTimer();

  /** Redefine what should happen when data got acked, to add congestion window management */
  void receivedDataAck(uint32 old_send_una);

  /** Redefine what should happen when dupAck was received, to add congestion window management */
  void receivedDuplicateAck();

  inline bool seqGE(uint32 a, uint32 b) { return (a - b) < (1UL << 31); }

  void rttMeasurementCompleteUsingTS(uint32 echoedTS);
  simtime_t convertTSToSimtime(uint32 timestamp);
  void rttMeasurementComplete(simtime_t tSent, simtime_t tAcked);
  uint32 convertSimtimeToTS(simtime_t simtime);
  void SetSndCwnd(uint32 snd_cwnd);
  int32 GetSndCwnd();
  void SetSndWnd(uint32 snd_wnd);
  void SetSndMss(uint32 snd_mss);
  void SetSsThresh(uint32 ssthresh);
  void SetSndMax(uint32 snd_max);
  void SetSndUnA(uint32 snd_una);


protected:
  uint32 snd_cwnd;    /// congestion window
  uint32 snd_wnd;    // send window
  uint32 snd_mss;    // sender maximum segment size (without headers, i.e. only segment text); see RFC 2581, page 1.
                     // This will be set to the minimum of the local smss parameter and the value specified in the
                     // MSS option received during connection setup.
  uint32 ssthresh;    ///< slow start threshold
  uint32 recover;    ///< recover (RFC 3782)
  bool firstPartialACK;    ///< first partial acknowledgement (RFC 3782)

  uint32 snd_max;    // max seq number sent (needed because snd_nxt is re-set on retransmission)
  bool lossRecovery;    // indicates if algorithm is in loss recovery phase

  uint32 snd_una;    // send unacknowledged
  uint32 dupacks;    // current number of received consecutive duplicate ACKs

  simtime_t srtt;    ///< smoothed round-trip time
  simtime_t rttvar;    ///< variance of round-trip time

  int rexmit_count;    ///< number of retransmissions (=1 after first rexmit)
  simtime_t rexmit_timeout;    ///< current retransmission timeout (aka RTO)

};

} /* namespace inet */

#endif /* INET_APPLICATIONS_QUICAPP_QUICNEWRENO_H_ */
