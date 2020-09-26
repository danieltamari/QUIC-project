//
// Generated file, do not edit! Created by nedtool 5.6 from inet/applications/quicapp/QuicData.msg.
//

#ifndef __INET_QUICDATA_M_H
#define __INET_QUICDATA_M_H

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

class QuicData;
} // namespace inet

#include "inet/common/INETDefs_m.h" // import inet.common.INETDefs

#include "inet/applications/base/ApplicationPacket_m.h" // import inet.applications.base.ApplicationPacket

#include "inet/common/packet/chunk/Chunk_m.h" // import inet.common.packet.chunk.Chunk

// cplusplus {{
#include "inet/applications/quicapp/StreamsData.h"
// }}


namespace inet {

/**
 * Class generated from <tt>inet/applications/quicapp/QuicData.msg:33</tt> by nedtool.
 * <pre>
 * //
 * // TODO generated message class
 * //
 * class QuicData extends FieldsChunk
 * {
 *     StreamsData *stream_frames;
 * }
 * </pre>
 */
class INET_API QuicData : public ::inet::FieldsChunk
{
  protected:
    StreamsData * stream_frames = nullptr;

  private:
    void copy(const QuicData& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const QuicData&);

  public:
    QuicData();
    QuicData(const QuicData& other);
    virtual ~QuicData();
    QuicData& operator=(const QuicData& other);
    virtual QuicData *dup() const override {return new QuicData(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual StreamsData * getStream_frames() const;
    virtual StreamsData * getStream_framesForUpdate() { handleChange();return const_cast<StreamsData *>(const_cast<QuicData*>(this)->getStream_frames());}
    virtual void setStream_frames(StreamsData * stream_frames);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const QuicData& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, QuicData& obj) {obj.parsimUnpack(b);}

} // namespace inet

#endif // ifndef __INET_QUICDATA_M_H