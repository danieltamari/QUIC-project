//
// Generated file, do not edit! Created by nedtool 5.6 from inet/applications/quicapp/QuicPacketHeader.msg.
//

#ifndef __INET_QUICPACKETHEADER_M_H
#define __INET_QUICPACKETHEADER_M_H

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

class QuicPacketHeader;
} // namespace inet

#include "inet/common/INETDefs_m.h" // import inet.common.INETDefs

#include "inet/applications/base/ApplicationPacket_m.h" // import inet.applications.base.ApplicationPacket

#include "inet/common/packet/chunk/Chunk_m.h" // import inet.common.packet.chunk.Chunk


namespace inet {

/**
 * Class generated from <tt>inet/applications/quicapp/QuicPacketHeader.msg:25</tt> by nedtool.
 * <pre>
 * //
 * // TODO generated message class
 * //
 * class QuicPacketHeader extends FieldsChunk
 * {
 *     int dest_connectionID;
 *     int src_connectionID;
 *     int packet_number;
 *     int packet_type;
 * }
 * </pre>
 */
class INET_API QuicPacketHeader : public ::inet::FieldsChunk
{
  protected:
    int dest_connectionID = 0;
    int src_connectionID = 0;
    int packet_number = 0;
    int packet_type = 0;

  private:
    void copy(const QuicPacketHeader& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const QuicPacketHeader&);

  public:
    QuicPacketHeader();
    QuicPacketHeader(const QuicPacketHeader& other);
    virtual ~QuicPacketHeader();
    QuicPacketHeader& operator=(const QuicPacketHeader& other);
    virtual QuicPacketHeader *dup() const override {return new QuicPacketHeader(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getDest_connectionID() const;
    virtual void setDest_connectionID(int dest_connectionID);
    virtual int getSrc_connectionID() const;
    virtual void setSrc_connectionID(int src_connectionID);
    virtual int getPacket_number() const;
    virtual void setPacket_number(int packet_number);
    virtual int getPacket_type() const;
    virtual void setPacket_type(int packet_type);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const QuicPacketHeader& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, QuicPacketHeader& obj) {obj.parsimUnpack(b);}

} // namespace inet

#endif // ifndef __INET_QUICPACKETHEADER_M_H

