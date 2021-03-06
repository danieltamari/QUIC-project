//
// Generated file, do not edit! Created by nedtool 5.6 from inet/applications/quicapp/headers_and_frames/StopSendingFrame.msg.
//

#ifndef __INET_STOPSENDINGFRAME_M_H
#define __INET_STOPSENDINGFRAME_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0506
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif

// dll export symbol
#ifndef INET_API
#  if defined(INET_EXPORT)
#    define INET_API  OPP_DLLEXPORT
#  elif defined(INET_IMPORT)
#    define INET_API  OPP_DLLIMPORT
#  else
#    define INET_API
#  endif
#endif


namespace inet {

class StopSendingFrame;
} // namespace inet

#include "inet/common/INETDefs_m.h" // import inet.common.INETDefs

#include "inet/applications/base/ApplicationPacket_m.h" // import inet.applications.base.ApplicationPacket

#include "inet/common/packet/chunk/Chunk_m.h" // import inet.common.packet.chunk.Chunk

#include "inet/applications/quicapp/headers_and_frames/QuicFrame_m.h" // import inet.applications.quicapp.headers_and_frames.QuicFrame


namespace inet {

/**
 * Class generated from <tt>inet/applications/quicapp/headers_and_frames/StopSendingFrame.msg:25</tt> by nedtool.
 * <pre>
 * //
 * // TODO generated message class
 * //
 * class StopSendingFrame extends QuicFrame
 * {
 *     //An endpoint uses a STOP_SENDING frame (type=0x05) to communicate that incoming data is being discarded on
 *     //receipt at application request. STOP_SENDING requests that a peer cease transmission on a stream.(RFC 19.5)
 *     int stream_id; // stream being ignored
 *     int error_code; // application protocol error code that indicates the reason the sender is ignoring the stream
 * }
 * </pre>
 */
class INET_API StopSendingFrame : public ::inet::QuicFrame
{
  protected:
    int stream_id = 0;
    int error_code = 0;

  private:
    void copy(const StopSendingFrame& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const StopSendingFrame&);

  public:
    StopSendingFrame();
    StopSendingFrame(const StopSendingFrame& other);
    virtual ~StopSendingFrame();
    StopSendingFrame& operator=(const StopSendingFrame& other);
    virtual StopSendingFrame *dup() const override {return new StopSendingFrame(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getStream_id() const;
    virtual void setStream_id(int stream_id);
    virtual int getError_code() const;
    virtual void setError_code(int error_code);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const StopSendingFrame& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, StopSendingFrame& obj) {obj.parsimUnpack(b);}

} // namespace inet

#endif // ifndef __INET_STOPSENDINGFRAME_M_H

