//
// Generated file, do not edit! Created by nedtool 5.6 from inet/applications/quicapp/headers_and_frames/MaxStreamsFrame.msg.
//

#ifndef __INET_MAXSTREAMSFRAME_M_H
#define __INET_MAXSTREAMSFRAME_M_H

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

class MaxStreamsFrame;
} // namespace inet

#include "inet/common/INETDefs_m.h" // import inet.common.INETDefs

#include "inet/applications/base/ApplicationPacket_m.h" // import inet.applications.base.ApplicationPacket

#include "inet/common/packet/chunk/Chunk_m.h" // import inet.common.packet.chunk.Chunk

#include "inet/applications/quicapp/headers_and_frames/QuicFrame_m.h" // import inet.applications.quicapp.headers_and_frames.QuicFrame


namespace inet {

/**
 * Class generated from <tt>inet/applications/quicapp/headers_and_frames/MaxStreamsFrame.msg:25</tt> by nedtool.
 * <pre>
 * //
 * // TODO generated message class
 * //
 * class MaxStreamsFrame extends QuicFrame
 * {
 *     //MAX_STREAMS frame (type=0x12 or 0x13) inform the peer of the cumulative number of streams of a given type
 *     //it is permitted to open. (RFC 19.11)
 *     int maximum_streams; // A count of the cumulative number of streams that can be opened over the 
 * 	//lifetime of the connection. This value cannot exceed 2^60
 * }
 * </pre>
 */
class INET_API MaxStreamsFrame : public ::inet::QuicFrame
{
  protected:
    int maximum_streams = 0;

  private:
    void copy(const MaxStreamsFrame& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const MaxStreamsFrame&);

  public:
    MaxStreamsFrame();
    MaxStreamsFrame(const MaxStreamsFrame& other);
    virtual ~MaxStreamsFrame();
    MaxStreamsFrame& operator=(const MaxStreamsFrame& other);
    virtual MaxStreamsFrame *dup() const override {return new MaxStreamsFrame(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getMaximum_streams() const;
    virtual void setMaximum_streams(int maximum_streams);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const MaxStreamsFrame& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, MaxStreamsFrame& obj) {obj.parsimUnpack(b);}

} // namespace inet

#endif // ifndef __INET_MAXSTREAMSFRAME_M_H
