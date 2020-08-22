//
// Generated file, do not edit! Created by nedtool 5.6 from inet/applications/quicapp/QuicHandShakeData.msg.
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
#include "QuicHandShakeData_m.h"

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

Register_Class(QuicHandShakeData)

QuicHandShakeData::QuicHandShakeData() : ::inet::FieldsChunk()
{
}

QuicHandShakeData::QuicHandShakeData(const QuicHandShakeData& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

QuicHandShakeData::~QuicHandShakeData()
{
}

QuicHandShakeData& QuicHandShakeData::operator=(const QuicHandShakeData& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void QuicHandShakeData::copy(const QuicHandShakeData& other)
{
    this->initial_source_connection_id = other.initial_source_connection_id;
    this->original_destination_connection_id = other.original_destination_connection_id;
    this->retry_source_connection_id = other.retry_source_connection_id;
    this->max_udp_payload_size = other.max_udp_payload_size;
    this->initial_max_data = other.initial_max_data;
    this->initial_max_stream_data = other.initial_max_stream_data;
}

void QuicHandShakeData::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->initial_source_connection_id);
    doParsimPacking(b,this->original_destination_connection_id);
    doParsimPacking(b,this->retry_source_connection_id);
    doParsimPacking(b,this->max_udp_payload_size);
    doParsimPacking(b,this->initial_max_data);
    doParsimPacking(b,this->initial_max_stream_data);
}

void QuicHandShakeData::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->initial_source_connection_id);
    doParsimUnpacking(b,this->original_destination_connection_id);
    doParsimUnpacking(b,this->retry_source_connection_id);
    doParsimUnpacking(b,this->max_udp_payload_size);
    doParsimUnpacking(b,this->initial_max_data);
    doParsimUnpacking(b,this->initial_max_stream_data);
}

int QuicHandShakeData::getInitial_source_connection_id() const
{
    return this->initial_source_connection_id;
}

void QuicHandShakeData::setInitial_source_connection_id(int initial_source_connection_id)
{
    handleChange();
    this->initial_source_connection_id = initial_source_connection_id;
}

int QuicHandShakeData::getOriginal_destination_connection_id() const
{
    return this->original_destination_connection_id;
}

void QuicHandShakeData::setOriginal_destination_connection_id(int original_destination_connection_id)
{
    handleChange();
    this->original_destination_connection_id = original_destination_connection_id;
}

int QuicHandShakeData::getRetry_source_connection_id() const
{
    return this->retry_source_connection_id;
}

void QuicHandShakeData::setRetry_source_connection_id(int retry_source_connection_id)
{
    handleChange();
    this->retry_source_connection_id = retry_source_connection_id;
}

int QuicHandShakeData::getMax_udp_payload_size() const
{
    return this->max_udp_payload_size;
}

void QuicHandShakeData::setMax_udp_payload_size(int max_udp_payload_size)
{
    handleChange();
    this->max_udp_payload_size = max_udp_payload_size;
}

int QuicHandShakeData::getInitial_max_data() const
{
    return this->initial_max_data;
}

void QuicHandShakeData::setInitial_max_data(int initial_max_data)
{
    handleChange();
    this->initial_max_data = initial_max_data;
}

int QuicHandShakeData::getInitial_max_stream_data() const
{
    return this->initial_max_stream_data;
}

void QuicHandShakeData::setInitial_max_stream_data(int initial_max_stream_data)
{
    handleChange();
    this->initial_max_stream_data = initial_max_stream_data;
}

class QuicHandShakeDataDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
    enum FieldConstants {
        FIELD_initial_source_connection_id,
        FIELD_original_destination_connection_id,
        FIELD_retry_source_connection_id,
        FIELD_max_udp_payload_size,
        FIELD_initial_max_data,
        FIELD_initial_max_stream_data,
    };
  public:
    QuicHandShakeDataDescriptor();
    virtual ~QuicHandShakeDataDescriptor();

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

Register_ClassDescriptor(QuicHandShakeDataDescriptor)

QuicHandShakeDataDescriptor::QuicHandShakeDataDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::QuicHandShakeData)), "inet::FieldsChunk")
{
    propertynames = nullptr;
}

QuicHandShakeDataDescriptor::~QuicHandShakeDataDescriptor()
{
    delete[] propertynames;
}

bool QuicHandShakeDataDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<QuicHandShakeData *>(obj)!=nullptr;
}

const char **QuicHandShakeDataDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *QuicHandShakeDataDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int QuicHandShakeDataDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 6+basedesc->getFieldCount() : 6;
}

unsigned int QuicHandShakeDataDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_initial_source_connection_id
        FD_ISEDITABLE,    // FIELD_original_destination_connection_id
        FD_ISEDITABLE,    // FIELD_retry_source_connection_id
        FD_ISEDITABLE,    // FIELD_max_udp_payload_size
        FD_ISEDITABLE,    // FIELD_initial_max_data
        FD_ISEDITABLE,    // FIELD_initial_max_stream_data
    };
    return (field >= 0 && field < 6) ? fieldTypeFlags[field] : 0;
}

const char *QuicHandShakeDataDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "initial_source_connection_id",
        "original_destination_connection_id",
        "retry_source_connection_id",
        "max_udp_payload_size",
        "initial_max_data",
        "initial_max_stream_data",
    };
    return (field >= 0 && field < 6) ? fieldNames[field] : nullptr;
}

int QuicHandShakeDataDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0] == 'i' && strcmp(fieldName, "initial_source_connection_id") == 0) return base+0;
    if (fieldName[0] == 'o' && strcmp(fieldName, "original_destination_connection_id") == 0) return base+1;
    if (fieldName[0] == 'r' && strcmp(fieldName, "retry_source_connection_id") == 0) return base+2;
    if (fieldName[0] == 'm' && strcmp(fieldName, "max_udp_payload_size") == 0) return base+3;
    if (fieldName[0] == 'i' && strcmp(fieldName, "initial_max_data") == 0) return base+4;
    if (fieldName[0] == 'i' && strcmp(fieldName, "initial_max_stream_data") == 0) return base+5;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *QuicHandShakeDataDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_initial_source_connection_id
        "int",    // FIELD_original_destination_connection_id
        "int",    // FIELD_retry_source_connection_id
        "int",    // FIELD_max_udp_payload_size
        "int",    // FIELD_initial_max_data
        "int",    // FIELD_initial_max_stream_data
    };
    return (field >= 0 && field < 6) ? fieldTypeStrings[field] : nullptr;
}

const char **QuicHandShakeDataDescriptor::getFieldPropertyNames(int field) const
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

const char *QuicHandShakeDataDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int QuicHandShakeDataDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    QuicHandShakeData *pp = (QuicHandShakeData *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *QuicHandShakeDataDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    QuicHandShakeData *pp = (QuicHandShakeData *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string QuicHandShakeDataDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    QuicHandShakeData *pp = (QuicHandShakeData *)object; (void)pp;
    switch (field) {
        case FIELD_initial_source_connection_id: return long2string(pp->getInitial_source_connection_id());
        case FIELD_original_destination_connection_id: return long2string(pp->getOriginal_destination_connection_id());
        case FIELD_retry_source_connection_id: return long2string(pp->getRetry_source_connection_id());
        case FIELD_max_udp_payload_size: return long2string(pp->getMax_udp_payload_size());
        case FIELD_initial_max_data: return long2string(pp->getInitial_max_data());
        case FIELD_initial_max_stream_data: return long2string(pp->getInitial_max_stream_data());
        default: return "";
    }
}

bool QuicHandShakeDataDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    QuicHandShakeData *pp = (QuicHandShakeData *)object; (void)pp;
    switch (field) {
        case FIELD_initial_source_connection_id: pp->setInitial_source_connection_id(string2long(value)); return true;
        case FIELD_original_destination_connection_id: pp->setOriginal_destination_connection_id(string2long(value)); return true;
        case FIELD_retry_source_connection_id: pp->setRetry_source_connection_id(string2long(value)); return true;
        case FIELD_max_udp_payload_size: pp->setMax_udp_payload_size(string2long(value)); return true;
        case FIELD_initial_max_data: pp->setInitial_max_data(string2long(value)); return true;
        case FIELD_initial_max_stream_data: pp->setInitial_max_stream_data(string2long(value)); return true;
        default: return false;
    }
}

const char *QuicHandShakeDataDescriptor::getFieldStructName(int field) const
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

void *QuicHandShakeDataDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    QuicHandShakeData *pp = (QuicHandShakeData *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

} // namespace inet

