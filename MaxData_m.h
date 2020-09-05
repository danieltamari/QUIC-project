//
// Generated file, do not edit! Created by nedtool 5.6 from inet/applications/quicapp/MaxData.msg.
//

#ifndef __INET_MAXDATA_M_H
#define __INET_MAXDATA_M_H

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

class MaxData;
} // namespace inet

#include "inet/common/INETDefs_m.h" // import inet.common.INETDefs

#include "inet/applications/base/ApplicationPacket_m.h" // import inet.applications.base.ApplicationPacket

#include "inet/common/packet/chunk/Chunk_m.h" // import inet.common.packet.chunk.Chunk


namespace inet {

/**
 * Class generated from <tt>inet/applications/quicapp/MaxData.msg:24</tt> by nedtool.
 * <pre>
 * //
 * // TODO generated message class
 * //
 * class MaxData extends FieldsChunk
 * {
 *     int Maximum_Data;
 * }
 * </pre>
 */
class INET_API MaxData : public ::inet::FieldsChunk
{
  protected:
    int Maximum_Data = 0;

  private:
    void copy(const MaxData& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const MaxData&);

  public:
    MaxData();
    MaxData(const MaxData& other);
    virtual ~MaxData();
    MaxData& operator=(const MaxData& other);
    virtual MaxData *dup() const override {return new MaxData(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getMaximum_Data() const;
    virtual void setMaximum_Data(int Maximum_Data);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const MaxData& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, MaxData& obj) {obj.parsimUnpack(b);}

} // namespace inet

#endif // ifndef __INET_MAXDATA_M_H

