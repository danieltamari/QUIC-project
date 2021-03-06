//
// Generated file, do not edit! Created by nedtool 5.6 from inet/applications/quicapp/headers_and_frames/QuicHandShakeData.msg.
//

#ifndef __INET_QUICHANDSHAKEDATA_M_H
#define __INET_QUICHANDSHAKEDATA_M_H

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

class QuicHandShakeData;
} // namespace inet

#include "inet/common/INETDefs_m.h" // import inet.common.INETDefs

#include "inet/applications/base/ApplicationPacket_m.h" // import inet.applications.base.ApplicationPacket

#include "inet/common/packet/chunk/Chunk_m.h" // import inet.common.packet.chunk.Chunk

#include "inet/applications/quicapp/headers_and_frames/QuicFrame_m.h" // import inet.applications.quicapp.headers_and_frames.QuicFrame


namespace inet {

/**
 * Class generated from <tt>inet/applications/quicapp/headers_and_frames/QuicHandShakeData.msg:27</tt> by nedtool.
 * <pre>
 * class QuicHandShakeData extends QuicFrame
 * {
 *     // The value that the endpoint included in the Source Connection ID field of the first
 *     // Initial packet it sends for the connection
 *     int initial_source_connection_id; // source_ID initiated by client
 * 
 *     // The value of the Destination Connection ID field from the first Initial packet sent
 *     // by the client; This transport parameter is only sent by a server. (RFC 18.2)
 *     int original_destination_connection_id; // inital Dest_ID server get in the first packet
 * 
 *     // limits the size of UDP payloads that the endpoint is willing to receive. UDP packets with
 *     // payloads larger than this limit are not likely to be processed by the receiver. (RFC 18.2)
 *     int max_udp_payload_size;
 * 
 *     // contains the initial value for the maximum amount of data that can be sent on the 
 *     // connection. This is equivalent to sending a MAX_DATA for the connection immediately
 *     // after completing the handshake.
 *     int initial_max_data;
 * 
 *     // specifying the initial flow control limit for locally-initiated bidirectional streams. 
 *     int initial_max_stream_data;
 * 
 * 
 * 
 * }
 * </pre>
 */
class INET_API QuicHandShakeData : public ::inet::QuicFrame
{
  protected:
    int initial_source_connection_id = 0;
    int original_destination_connection_id = 0;
    int max_udp_payload_size = 0;
    int initial_max_data = 0;
    int initial_max_stream_data = 0;

  private:
    void copy(const QuicHandShakeData& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const QuicHandShakeData&);

  public:
    QuicHandShakeData();
    QuicHandShakeData(const QuicHandShakeData& other);
    virtual ~QuicHandShakeData();
    QuicHandShakeData& operator=(const QuicHandShakeData& other);
    virtual QuicHandShakeData *dup() const override {return new QuicHandShakeData(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getInitial_source_connection_id() const;
    virtual void setInitial_source_connection_id(int initial_source_connection_id);
    virtual int getOriginal_destination_connection_id() const;
    virtual void setOriginal_destination_connection_id(int original_destination_connection_id);
    virtual int getMax_udp_payload_size() const;
    virtual void setMax_udp_payload_size(int max_udp_payload_size);
    virtual int getInitial_max_data() const;
    virtual void setInitial_max_data(int initial_max_data);
    virtual int getInitial_max_stream_data() const;
    virtual void setInitial_max_stream_data(int initial_max_stream_data);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const QuicHandShakeData& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, QuicHandShakeData& obj) {obj.parsimUnpack(b);}

} // namespace inet

#endif // ifndef __INET_QUICHANDSHAKEDATA_M_H

