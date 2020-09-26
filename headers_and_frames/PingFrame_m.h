//
// Generated file, do not edit! Created by nedtool 5.6 from inet/applications/quicapp/headers_and_frames/PingFrame.msg.
//

#ifndef __INET_PINGFRAME_M_H
#define __INET_PINGFRAME_M_H

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

class PingFrame;
} // namespace inet

#include "inet/common/INETDefs_m.h" // import inet.common.INETDefs

#include "inet/applications/base/ApplicationPacket_m.h" // import inet.applications.base.ApplicationPacket

#include "inet/common/packet/chunk/Chunk_m.h" // import inet.common.packet.chunk.Chunk

#include "inet/applications/quicapp/headers_and_frames/QuicFrame_m.h" // import inet.applications.quicapp.headers_and_frames.QuicFrame


namespace inet {

/**
 * Class generated from <tt>inet/applications/quicapp/headers_and_frames/PingFrame.msg:25</tt> by nedtool.
 * <pre>
 * //
 * // TODO generated message class
 * //
 * class PingFrame extends QuicFrame
 * {
 * }
 * </pre>
 */
class INET_API PingFrame : public ::inet::QuicFrame
{
  protected:

  private:
    void copy(const PingFrame& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const PingFrame&);

  public:
    PingFrame();
    PingFrame(const PingFrame& other);
    virtual ~PingFrame();
    PingFrame& operator=(const PingFrame& other);
    virtual PingFrame *dup() const override {return new PingFrame(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const PingFrame& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, PingFrame& obj) {obj.parsimUnpack(b);}

} // namespace inet

#endif // ifndef __INET_PINGFRAME_M_H

