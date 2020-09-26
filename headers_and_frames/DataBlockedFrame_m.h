//
// Generated file, do not edit! Created by nedtool 5.6 from inet/applications/quicapp/headers_and_frames/DataBlockedFrame.msg.
//

#ifndef __INET_DATABLOCKEDFRAME_M_H
#define __INET_DATABLOCKEDFRAME_M_H

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

class DataBlockedFrame;
} // namespace inet

#include "inet/common/INETDefs_m.h" // import inet.common.INETDefs

#include "inet/applications/base/ApplicationPacket_m.h" // import inet.applications.base.ApplicationPacket

#include "inet/common/packet/chunk/Chunk_m.h" // import inet.common.packet.chunk.Chunk

#include "inet/applications/quicapp/headers_and_frames/QuicFrame_m.h" // import inet.applications.quicapp.headers_and_frames.QuicFrame


namespace inet {

/**
 * Class generated from <tt>inet/applications/quicapp/headers_and_frames/DataBlockedFrame.msg:25</tt> by nedtool.
 * <pre>
 * //
 * // TODO generated message class
 * //
 * class DataBlockedFrame extends QuicFrame
 * {
 *     //A sender SHOULD send a DATA_BLOCKED frame (type=0x14) when it wishes to send data, but is unable to do so
 *     //due to connection-level flow control; DATA_BLOCKED frames can be used as input to tuning of flow control 
 *     //algorithms; (RFC 19.12)	
 *     int maximum_data; // the connection-level limit at which blocking occurred
 * }
 * </pre>
 */
class INET_API DataBlockedFrame : public ::inet::QuicFrame
{
  protected:
    int maximum_data = 0;

  private:
    void copy(const DataBlockedFrame& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const DataBlockedFrame&);

  public:
    DataBlockedFrame();
    DataBlockedFrame(const DataBlockedFrame& other);
    virtual ~DataBlockedFrame();
    DataBlockedFrame& operator=(const DataBlockedFrame& other);
    virtual DataBlockedFrame *dup() const override {return new DataBlockedFrame(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getMaximum_data() const;
    virtual void setMaximum_data(int maximum_data);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const DataBlockedFrame& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, DataBlockedFrame& obj) {obj.parsimUnpack(b);}

} // namespace inet

#endif // ifndef __INET_DATABLOCKEDFRAME_M_H

