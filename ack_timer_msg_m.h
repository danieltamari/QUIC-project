//
// Generated file, do not edit! Created by nedtool 5.6 from inet/applications/quicapp/ack_timer_msg.msg.
//

#ifndef __INET_ACK_TIMER_MSG_M_H
#define __INET_ACK_TIMER_MSG_M_H

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

class ack_timer_msg;
} // namespace inet

#include "inet/common/INETDefs_m.h" // import inet.common.INETDefs

#include "inet/applications/base/ApplicationPacket_m.h" // import inet.applications.base.ApplicationPacket

#include "inet/common/packet/chunk/Chunk_m.h" // import inet.common.packet.chunk.Chunk


namespace inet {

/**
 * Class generated from <tt>inet/applications/quicapp/ack_timer_msg.msg:25</tt> by nedtool.
 * <pre>
 * //
 * // TODO generated message class
 * //
 * class ack_timer_msg extends cMessage
 * {
 *     int dest_connection_ID;
 * 
 * }
 * </pre>
 */
class INET_API ack_timer_msg : public ::omnetpp::cMessage
{
  protected:
    int dest_connection_ID = 0;

  private:
    void copy(const ack_timer_msg& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const ack_timer_msg&);

  public:
    ack_timer_msg(const char *name=nullptr);
    ack_timer_msg(const ack_timer_msg& other);
    virtual ~ack_timer_msg();
    ack_timer_msg& operator=(const ack_timer_msg& other);
    virtual ack_timer_msg *dup() const override {return new ack_timer_msg(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getDest_connection_ID() const;
    virtual void setDest_connection_ID(int dest_connection_ID);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const ack_timer_msg& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, ack_timer_msg& obj) {obj.parsimUnpack(b);}

} // namespace inet

#endif // ifndef __INET_ACK_TIMER_MSG_M_H

