//
// Generated file, do not edit! Created by nedtool 5.6 from inet/applications/quicapp/connection_config_data.msg.
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
#include "connection_config_data_m.h"

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

Register_Class(connection_config_data)

connection_config_data::connection_config_data() : ::inet::FieldsChunk()
{
}

connection_config_data::connection_config_data(const connection_config_data& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

connection_config_data::~connection_config_data()
{
    delete [] this->connection_data;
}

connection_config_data& connection_config_data::operator=(const connection_config_data& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void connection_config_data::copy(const connection_config_data& other)
{
    delete [] this->connection_data;
    this->connection_data = (other.connection_data_arraysize==0) ? nullptr : new int[other.connection_data_arraysize];
    connection_data_arraysize = other.connection_data_arraysize;
    for (size_t i = 0; i < connection_data_arraysize; i++) {
        this->connection_data[i] = other.connection_data[i];
    }
    this->connectAddress = other.connectAddress;
}

void connection_config_data::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    b->pack(connection_data_arraysize);
    doParsimArrayPacking(b,this->connection_data,connection_data_arraysize);
    doParsimPacking(b,this->connectAddress);
}

void connection_config_data::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    delete [] this->connection_data;
    b->unpack(connection_data_arraysize);
    if (connection_data_arraysize == 0) {
        this->connection_data = nullptr;
    } else {
        this->connection_data = new int[connection_data_arraysize];
        doParsimArrayUnpacking(b,this->connection_data,connection_data_arraysize);
    }
    doParsimUnpacking(b,this->connectAddress);
}

size_t connection_config_data::getConnection_dataArraySize() const
{
    return connection_data_arraysize;
}

int connection_config_data::getConnection_data(size_t k) const
{
    if (k >= connection_data_arraysize) throw omnetpp::cRuntimeError("Array of size connection_data_arraysize indexed by %lu", (unsigned long)k);
    return this->connection_data[k];
}

void connection_config_data::setConnection_dataArraySize(size_t newSize)
{
    handleChange();
    int *connection_data2 = (newSize==0) ? nullptr : new int[newSize];
    size_t minSize = connection_data_arraysize < newSize ? connection_data_arraysize : newSize;
    for (size_t i = 0; i < minSize; i++)
        connection_data2[i] = this->connection_data[i];
    for (size_t i = minSize; i < newSize; i++)
        connection_data2[i] = 0;
    delete [] this->connection_data;
    this->connection_data = connection_data2;
    connection_data_arraysize = newSize;
}

void connection_config_data::setConnection_data(size_t k, int connection_data)
{
    if (k >= connection_data_arraysize) throw omnetpp::cRuntimeError("Array of size  indexed by %lu", (unsigned long)k);
    handleChange();
    this->connection_data[k] = connection_data;
}

void connection_config_data::insertConnection_data(size_t k, int connection_data)
{
    handleChange();
    if (k > connection_data_arraysize) throw omnetpp::cRuntimeError("Array of size  indexed by %lu", (unsigned long)k);
    size_t newSize = connection_data_arraysize + 1;
    int *connection_data2 = new int[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        connection_data2[i] = this->connection_data[i];
    connection_data2[k] = connection_data;
    for (i = k + 1; i < newSize; i++)
        connection_data2[i] = this->connection_data[i-1];
    delete [] this->connection_data;
    this->connection_data = connection_data2;
    connection_data_arraysize = newSize;
}

void connection_config_data::insertConnection_data(int connection_data)
{
    insertConnection_data(connection_data_arraysize, connection_data);
}

void connection_config_data::eraseConnection_data(size_t k)
{
    if (k >= connection_data_arraysize) throw omnetpp::cRuntimeError("Array of size  indexed by %lu", (unsigned long)k);
    handleChange();
    size_t newSize = connection_data_arraysize - 1;
    int *connection_data2 = (newSize == 0) ? nullptr : new int[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        connection_data2[i] = this->connection_data[i];
    for (i = k; i < newSize; i++)
        connection_data2[i] = this->connection_data[i+1];
    delete [] this->connection_data;
    this->connection_data = connection_data2;
    connection_data_arraysize = newSize;
}

const char * connection_config_data::getConnectAddress() const
{
    return this->connectAddress.c_str();
}

void connection_config_data::setConnectAddress(const char * connectAddress)
{
    handleChange();
    this->connectAddress = connectAddress;
}

class connection_config_dataDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
    enum FieldConstants {
        FIELD_connection_data,
        FIELD_connectAddress,
    };
  public:
    connection_config_dataDescriptor();
    virtual ~connection_config_dataDescriptor();

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

Register_ClassDescriptor(connection_config_dataDescriptor)

connection_config_dataDescriptor::connection_config_dataDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::connection_config_data)), "inet::FieldsChunk")
{
    propertynames = nullptr;
}

connection_config_dataDescriptor::~connection_config_dataDescriptor()
{
    delete[] propertynames;
}

bool connection_config_dataDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<connection_config_data *>(obj)!=nullptr;
}

const char **connection_config_dataDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *connection_config_dataDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int connection_config_dataDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount() : 2;
}

unsigned int connection_config_dataDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISARRAY | FD_ISEDITABLE,    // FIELD_connection_data
        FD_ISEDITABLE,    // FIELD_connectAddress
    };
    return (field >= 0 && field < 2) ? fieldTypeFlags[field] : 0;
}

const char *connection_config_dataDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "connection_data",
        "connectAddress",
    };
    return (field >= 0 && field < 2) ? fieldNames[field] : nullptr;
}

int connection_config_dataDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0] == 'c' && strcmp(fieldName, "connection_data") == 0) return base+0;
    if (fieldName[0] == 'c' && strcmp(fieldName, "connectAddress") == 0) return base+1;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *connection_config_dataDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_connection_data
        "string",    // FIELD_connectAddress
    };
    return (field >= 0 && field < 2) ? fieldTypeStrings[field] : nullptr;
}

const char **connection_config_dataDescriptor::getFieldPropertyNames(int field) const
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

const char *connection_config_dataDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int connection_config_dataDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    connection_config_data *pp = (connection_config_data *)object; (void)pp;
    switch (field) {
        case FIELD_connection_data: return pp->getConnection_dataArraySize();
        default: return 0;
    }
}

const char *connection_config_dataDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    connection_config_data *pp = (connection_config_data *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string connection_config_dataDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    connection_config_data *pp = (connection_config_data *)object; (void)pp;
    switch (field) {
        case FIELD_connection_data: return long2string(pp->getConnection_data(i));
        case FIELD_connectAddress: return oppstring2string(pp->getConnectAddress());
        default: return "";
    }
}

bool connection_config_dataDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    connection_config_data *pp = (connection_config_data *)object; (void)pp;
    switch (field) {
        case FIELD_connection_data: pp->setConnection_data(i,string2long(value)); return true;
        case FIELD_connectAddress: pp->setConnectAddress((value)); return true;
        default: return false;
    }
}

const char *connection_config_dataDescriptor::getFieldStructName(int field) const
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

void *connection_config_dataDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    connection_config_data *pp = (connection_config_data *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

} // namespace inet

