//
// Generated file, do not edit! Created by nedtool 5.6 from inet/applications/quicapp/headers_and_frames/QuicPacketHeader.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include "QuicPacketHeader_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

namespace {
template <class T> inline
typename std::enable_if<std::is_polymorphic<T>::value && std::is_base_of<omnetpp::cObject,T>::value, void *>::type
toVoidPtr(T* t)
{
    return (void *)(static_cast<const omnetpp::cObject *>(t));
}

template <class T> inline
typename std::enable_if<std::is_polymorphic<T>::value && !std::is_base_of<omnetpp::cObject,T>::value, void *>::type
toVoidPtr(T* t)
{
    return (void *)dynamic_cast<const void *>(t);
}

template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void *>::type
toVoidPtr(T* t)
{
    return (void *)static_cast<const void *>(t);
}

}

namespace inet {

// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule to generate operator<< for shared_ptr<T>
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const std::shared_ptr<T>& t) { return out << t.get(); }

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');

    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

Register_Class(QuicPacketHeader)

QuicPacketHeader::QuicPacketHeader() : ::inet::FieldsChunk()
{
}

QuicPacketHeader::QuicPacketHeader(const QuicPacketHeader& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

QuicPacketHeader::~QuicPacketHeader()
{
}

QuicPacketHeader& QuicPacketHeader::operator=(const QuicPacketHeader& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void QuicPacketHeader::copy(const QuicPacketHeader& other)
{
    this->dest_connection_id_length = other.dest_connection_id_length;
    this->dest_connection_ID = other.dest_connection_ID;
    this->packet_number_length = other.packet_number_length;
    this->packet_number = other.packet_number;
    this->header_form = other.header_form;
    this->fixed_bit = other.fixed_bit;
}

void QuicPacketHeader::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->dest_connection_id_length);
    doParsimPacking(b,this->dest_connection_ID);
    doParsimPacking(b,this->packet_number_length);
    doParsimPacking(b,this->packet_number);
    doParsimPacking(b,this->header_form);
    doParsimPacking(b,this->fixed_bit);
}

void QuicPacketHeader::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->dest_connection_id_length);
    doParsimUnpacking(b,this->dest_connection_ID);
    doParsimUnpacking(b,this->packet_number_length);
    doParsimUnpacking(b,this->packet_number);
    doParsimUnpacking(b,this->header_form);
    doParsimUnpacking(b,this->fixed_bit);
}

unsigned int QuicPacketHeader::getDest_connection_id_length() const
{
    return this->dest_connection_id_length;
}

void QuicPacketHeader::setDest_connection_id_length(unsigned int dest_connection_id_length)
{
    handleChange();
    this->dest_connection_id_length = dest_connection_id_length;
}

int QuicPacketHeader::getDest_connection_ID() const
{
    return this->dest_connection_ID;
}

void QuicPacketHeader::setDest_connection_ID(int dest_connection_ID)
{
    handleChange();
    this->dest_connection_ID = dest_connection_ID;
}

B QuicPacketHeader::getPacket_number_length() const
{
    return this->packet_number_length;
}

void QuicPacketHeader::setPacket_number_length(B packet_number_length)
{
    handleChange();
    this->packet_number_length = packet_number_length;
}

int QuicPacketHeader::getPacket_number() const
{
    return this->packet_number;
}

void QuicPacketHeader::setPacket_number(int packet_number)
{
    handleChange();
    this->packet_number = packet_number;
}

b QuicPacketHeader::getHeader_form() const
{
    return this->header_form;
}

void QuicPacketHeader::setHeader_form(b header_form)
{
    handleChange();
    this->header_form = header_form;
}

b QuicPacketHeader::getFixed_bit() const
{
    return this->fixed_bit;
}

void QuicPacketHeader::setFixed_bit(b fixed_bit)
{
    handleChange();
    this->fixed_bit = fixed_bit;
}

class QuicPacketHeaderDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
    enum FieldConstants {
        FIELD_dest_connection_id_length,
        FIELD_dest_connection_ID,
        FIELD_packet_number_length,
        FIELD_packet_number,
        FIELD_header_form,
        FIELD_fixed_bit,
    };
  public:
    QuicPacketHeaderDescriptor();
    virtual ~QuicPacketHeaderDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(QuicPacketHeaderDescriptor)

QuicPacketHeaderDescriptor::QuicPacketHeaderDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::QuicPacketHeader)), "inet::FieldsChunk")
{
    propertynames = nullptr;
}

QuicPacketHeaderDescriptor::~QuicPacketHeaderDescriptor()
{
    delete[] propertynames;
}

bool QuicPacketHeaderDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<QuicPacketHeader *>(obj)!=nullptr;
}

const char **QuicPacketHeaderDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *QuicPacketHeaderDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int QuicPacketHeaderDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 6+basedesc->getFieldCount() : 6;
}

unsigned int QuicPacketHeaderDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_dest_connection_id_length
        FD_ISEDITABLE,    // FIELD_dest_connection_ID
        FD_ISEDITABLE,    // FIELD_packet_number_length
        FD_ISEDITABLE,    // FIELD_packet_number
        FD_ISEDITABLE,    // FIELD_header_form
        FD_ISEDITABLE,    // FIELD_fixed_bit
    };
    return (field >= 0 && field < 6) ? fieldTypeFlags[field] : 0;
}

const char *QuicPacketHeaderDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "dest_connection_id_length",
        "dest_connection_ID",
        "packet_number_length",
        "packet_number",
        "header_form",
        "fixed_bit",
    };
    return (field >= 0 && field < 6) ? fieldNames[field] : nullptr;
}

int QuicPacketHeaderDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0] == 'd' && strcmp(fieldName, "dest_connection_id_length") == 0) return base+0;
    if (fieldName[0] == 'd' && strcmp(fieldName, "dest_connection_ID") == 0) return base+1;
    if (fieldName[0] == 'p' && strcmp(fieldName, "packet_number_length") == 0) return base+2;
    if (fieldName[0] == 'p' && strcmp(fieldName, "packet_number") == 0) return base+3;
    if (fieldName[0] == 'h' && strcmp(fieldName, "header_form") == 0) return base+4;
    if (fieldName[0] == 'f' && strcmp(fieldName, "fixed_bit") == 0) return base+5;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *QuicPacketHeaderDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "unsigned int",    // FIELD_dest_connection_id_length
        "int",    // FIELD_dest_connection_ID
        "inet::B",    // FIELD_packet_number_length
        "int",    // FIELD_packet_number
        "inet::b",    // FIELD_header_form
        "inet::b",    // FIELD_fixed_bit
    };
    return (field >= 0 && field < 6) ? fieldTypeStrings[field] : nullptr;
}

const char **QuicPacketHeaderDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *QuicPacketHeaderDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int QuicPacketHeaderDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    QuicPacketHeader *pp = (QuicPacketHeader *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *QuicPacketHeaderDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    QuicPacketHeader *pp = (QuicPacketHeader *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string QuicPacketHeaderDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    QuicPacketHeader *pp = (QuicPacketHeader *)object; (void)pp;
    switch (field) {
        case FIELD_dest_connection_id_length: return ulong2string(pp->getDest_connection_id_length());
        case FIELD_dest_connection_ID: return long2string(pp->getDest_connection_ID());
        case FIELD_packet_number_length: return unit2string(pp->getPacket_number_length());
        case FIELD_packet_number: return long2string(pp->getPacket_number());
        case FIELD_header_form: return unit2string(pp->getHeader_form());
        case FIELD_fixed_bit: return unit2string(pp->getFixed_bit());
        default: return "";
    }
}

bool QuicPacketHeaderDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    QuicPacketHeader *pp = (QuicPacketHeader *)object; (void)pp;
    switch (field) {
        case FIELD_dest_connection_id_length: pp->setDest_connection_id_length(string2ulong(value)); return true;
        case FIELD_dest_connection_ID: pp->setDest_connection_ID(string2long(value)); return true;
        case FIELD_packet_number_length: pp->setPacket_number_length(B(string2long(value))); return true;
        case FIELD_packet_number: pp->setPacket_number(string2long(value)); return true;
        case FIELD_header_form: pp->setHeader_form(b(string2long(value))); return true;
        case FIELD_fixed_bit: pp->setFixed_bit(b(string2long(value))); return true;
        default: return false;
    }
}

const char *QuicPacketHeaderDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *QuicPacketHeaderDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    QuicPacketHeader *pp = (QuicPacketHeader *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

} // namespace inet

