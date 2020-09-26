//
// Generated file, do not edit! Created by nedtool 5.6 from inet/applications/quicapp/headers_and_frames/MaxStreamDataFrame.msg.
//

#ifndef __INET_MAXSTREAMDATAFRAME_M_H
#define __INET_MAXSTREAMDATAFRAME_M_H

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

class MaxStreamDataFrame;
} // namespace inet

#include "inet/common/INETDefs_m.h" // import inet.common.INETDefs

#include "inet/applications/base/ApplicationPacket_m.h" // import inet.applications.base.ApplicationPacket

#include "inet/common/packet/chunk/Chunk_m.h" // import inet.common.packet.chunk.Chunk

#include "inet/applications/quicapp/headers_and_frames/QuicFrame_m.h" // import inet.applications.quicapp.headers_and_frames.QuicFrame


namespace inet {

/**
 * Class generated from <tt>inet/applications/quicapp/headers_and_frames/MaxStreamDataFrame.msg:24</tt> by nedtool.
 * <pre>
 * //
 * // TODO generated message class
 * //
 * class MaxStreamDataFrame extends QuicFrame
 * {
 *     //MAX_STREAM_DATA frame (type=0x11) is used in flow control to inform a peer of the maximum amount of data
 *     //that can be sent on a stream. (RFC 19.10)
 *     int stream_id; // stream ID of the stream that is affected
 *     int maximum_stream_data; // maximum amount of data that can be sent on the identified stream, in units of bytes.
 * }
 * </pre>
 */
class INET_API MaxStreamDataFrame : public ::inet::QuicFrame
{
  protected:
    int stream_id = 0;
    int maximum_stream_data = 0;

  private:
    void copy(const MaxStreamDataFrame& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const MaxStreamDataFrame&);

  public:
    MaxStreamDataFrame();
    MaxStreamDataFrame(const MaxStreamDataFrame& other);
    virtual ~MaxStreamDataFrame();
    MaxStreamDataFrame& operator=(const MaxStreamDataFrame& other);
    virtual MaxStreamDataFrame *dup() const override {return new MaxStreamDataFrame(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getStream_id() const;
    virtual void setStream_id(int stream_id);
    virtual int getMaximum_stream_data() const;
    virtual void setMaximum_stream_data(int maximum_stream_data);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const MaxStreamDataFrame& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, MaxStreamDataFrame& obj) {obj.parsimUnpack(b);}

} // namespace inet

#endif // ifndef __INET_MAXSTREAMDATAFRAME_M_H

