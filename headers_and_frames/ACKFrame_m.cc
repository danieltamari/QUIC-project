//
// Generated file, do not edit! Created by nedtool 5.6 from inet/applications/quicapp/headers_and_frames/ACKFrame.msg.
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
#include "ACKFrame_m.h"

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

range::range()
{
    this->gap = 0;
    this->ACK_range_length = 0;
}

void __doPacking(omnetpp::cCommBuffer *b, const range& a)
{
    doParsimPacking(b,a.gap);
    doParsimPacking(b,a.ACK_range_length);
}

void __doUnpacking(omnetpp::cCommBuffer *b, range& a)
{
    doParsimUnpacking(b,a.gap);
    doParsimUnpacking(b,a.ACK_range_length);
}

class rangeDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
    enum FieldConstants {
        FIELD_gap,
        FIELD_ACK_range_length,
    };
  public:
    rangeDescriptor();
    virtual ~rangeDescriptor();

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

Register_ClassDescriptor(rangeDescriptor)

rangeDescriptor::rangeDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(range)), "")
{
    propertynames = nullptr;
}

rangeDescriptor::~rangeDescriptor()
{
    delete[] propertynames;
}

bool rangeDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<range *>(obj)!=nullptr;
}

const char **rangeDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *rangeDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int rangeDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount() : 2;
}

unsigned int rangeDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_gap
        FD_ISEDITABLE,    // FIELD_ACK_range_length
    };
    return (field >= 0 && field < 2) ? fieldTypeFlags[field] : 0;
}

const char *rangeDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "gap",
        "ACK_range_length",
    };
    return (field >= 0 && field < 2) ? fieldNames[field] : nullptr;
}

int rangeDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0] == 'g' && strcmp(fieldName, "gap") == 0) return base+0;
    if (fieldName[0] == 'A' && strcmp(fieldName, "ACK_range_length") == 0) return base+1;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *rangeDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_gap
        "int",    // FIELD_ACK_range_length
    };
    return (field >= 0 && field < 2) ? fieldTypeStrings[field] : nullptr;
}

const char **rangeDescriptor::getFieldPropertyNames(int field) const
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

const char *rangeDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int rangeDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    range *pp = (range *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *rangeDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    range *pp = (range *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string rangeDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    range *pp = (range *)object; (void)pp;
    switch (field) {
        case FIELD_gap: return long2string(pp->gap);
        case FIELD_ACK_range_length: return long2string(pp->ACK_range_length);
        default: return "";
    }
}

bool rangeDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    range *pp = (range *)object; (void)pp;
    switch (field) {
        case FIELD_gap: pp->gap = string2long(value); return true;
        case FIELD_ACK_range_length: pp->ACK_range_length = string2long(value); return true;
        default: return false;
    }
}

const char *rangeDescriptor::getFieldStructName(int field) const
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

void *rangeDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    range *pp = (range *)object; (void)pp;
    switch (field) {
        default: return nullptr;
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

Register_Class(ACKFrame)

ACKFrame::ACKFrame() : ::inet::FieldsChunk()
{
}

ACKFrame::ACKFrame(const ACKFrame& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

ACKFrame::~ACKFrame()
{
    delete [] this->ACK_ranges;
}

ACKFrame& ACKFrame::operator=(const ACKFrame& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void ACKFrame::copy(const ACKFrame& other)
{
    this->largest_acknowledged = other.largest_acknowledged;
    this->ACK_delay = other.ACK_delay;
    this->ACK_range_count = other.ACK_range_count;
    this->first_ACK_range = other.first_ACK_range;
    delete [] this->ACK_ranges;
    this->ACK_ranges = (other.ACK_ranges_arraysize==0) ? nullptr : new range[other.ACK_ranges_arraysize];
    ACK_ranges_arraysize = other.ACK_ranges_arraysize;
    for (size_t i = 0; i < ACK_ranges_arraysize; i++) {
        this->ACK_ranges[i] = other.ACK_ranges[i];
    }
}

void ACKFrame::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->largest_acknowledged);
    doParsimPacking(b,this->ACK_delay);
    doParsimPacking(b,this->ACK_range_count);
    doParsimPacking(b,this->first_ACK_range);
    b->pack(ACK_ranges_arraysize);
    doParsimArrayPacking(b,this->ACK_ranges,ACK_ranges_arraysize);
}

void ACKFrame::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->largest_acknowledged);
    doParsimUnpacking(b,this->ACK_delay);
    doParsimUnpacking(b,this->ACK_range_count);
    doParsimUnpacking(b,this->first_ACK_range);
    delete [] this->ACK_ranges;
    b->unpack(ACK_ranges_arraysize);
    if (ACK_ranges_arraysize == 0) {
        this->ACK_ranges = nullptr;
    } else {
        this->ACK_ranges = new range[ACK_ranges_arraysize];
        doParsimArrayUnpacking(b,this->ACK_ranges,ACK_ranges_arraysize);
    }
}

int ACKFrame::getLargest_acknowledged() const
{
    return this->largest_acknowledged;
}

void ACKFrame::setLargest_acknowledged(int largest_acknowledged)
{
    handleChange();
    this->largest_acknowledged = largest_acknowledged;
}

int ACKFrame::getACK_delay() const
{
    return this->ACK_delay;
}

void ACKFrame::setACK_delay(int ACK_delay)
{
    handleChange();
    this->ACK_delay = ACK_delay;
}

int ACKFrame::getACK_range_count() const
{
    return this->ACK_range_count;
}

void ACKFrame::setACK_range_count(int ACK_range_count)
{
    handleChange();
    this->ACK_range_count = ACK_range_count;
}

int ACKFrame::getFirst_ACK_range() const
{
    return this->first_ACK_range;
}

void ACKFrame::setFirst_ACK_range(int first_ACK_range)
{
    handleChange();
    this->first_ACK_range = first_ACK_range;
}

size_t ACKFrame::getACK_rangesArraySize() const
{
    return ACK_ranges_arraysize;
}

const range& ACKFrame::getACK_ranges(size_t k) const
{
    if (k >= ACK_ranges_arraysize) throw omnetpp::cRuntimeError("Array of size ACK_ranges_arraysize indexed by %lu", (unsigned long)k);
    return this->ACK_ranges[k];
}

void ACKFrame::setACK_rangesArraySize(size_t newSize)
{
    handleChange();
    range *ACK_ranges2 = (newSize==0) ? nullptr : new range[newSize];
    size_t minSize = ACK_ranges_arraysize < newSize ? ACK_ranges_arraysize : newSize;
    for (size_t i = 0; i < minSize; i++)
        ACK_ranges2[i] = this->ACK_ranges[i];
    delete [] this->ACK_ranges;
    this->ACK_ranges = ACK_ranges2;
    ACK_ranges_arraysize = newSize;
}

void ACKFrame::setACK_ranges(size_t k, const range& ACK_ranges)
{
    if (k >= ACK_ranges_arraysize) throw omnetpp::cRuntimeError("Array of size  indexed by %lu", (unsigned long)k);
    handleChange();
    this->ACK_ranges[k] = ACK_ranges;
}

void ACKFrame::insertACK_ranges(size_t k, const range& ACK_ranges)
{
    handleChange();
    if (k > ACK_ranges_arraysize) throw omnetpp::cRuntimeError("Array of size  indexed by %lu", (unsigned long)k);
    size_t newSize = ACK_ranges_arraysize + 1;
    range *ACK_ranges2 = new range[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        ACK_ranges2[i] = this->ACK_ranges[i];
    ACK_ranges2[k] = ACK_ranges;
    for (i = k + 1; i < newSize; i++)
        ACK_ranges2[i] = this->ACK_ranges[i-1];
    delete [] this->ACK_ranges;
    this->ACK_ranges = ACK_ranges2;
    ACK_ranges_arraysize = newSize;
}

void ACKFrame::insertACK_ranges(const range& ACK_ranges)
{
    insertACK_ranges(ACK_ranges_arraysize, ACK_ranges);
}

void ACKFrame::eraseACK_ranges(size_t k)
{
    if (k >= ACK_ranges_arraysize) throw omnetpp::cRuntimeError("Array of size  indexed by %lu", (unsigned long)k);
    handleChange();
    size_t newSize = ACK_ranges_arraysize - 1;
    range *ACK_ranges2 = (newSize == 0) ? nullptr : new range[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        ACK_ranges2[i] = this->ACK_ranges[i];
    for (i = k; i < newSize; i++)
        ACK_ranges2[i] = this->ACK_ranges[i+1];
    delete [] this->ACK_ranges;
    this->ACK_ranges = ACK_ranges2;
    ACK_ranges_arraysize = newSize;
}

class ACKFrameDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
    enum FieldConstants {
        FIELD_largest_acknowledged,
        FIELD_ACK_delay,
        FIELD_ACK_range_count,
        FIELD_first_ACK_range,
        FIELD_ACK_ranges,
    };
  public:
    ACKFrameDescriptor();
    virtual ~ACKFrameDescriptor();

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

Register_ClassDescriptor(ACKFrameDescriptor)

ACKFrameDescriptor::ACKFrameDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::ACKFrame)), "inet::FieldsChunk")
{
    propertynames = nullptr;
}

ACKFrameDescriptor::~ACKFrameDescriptor()
{
    delete[] propertynames;
}

bool ACKFrameDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ACKFrame *>(obj)!=nullptr;
}

const char **ACKFrameDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *ACKFrameDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int ACKFrameDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 5+basedesc->getFieldCount() : 5;
}

unsigned int ACKFrameDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_largest_acknowledged
        FD_ISEDITABLE,    // FIELD_ACK_delay
        FD_ISEDITABLE,    // FIELD_ACK_range_count
        FD_ISEDITABLE,    // FIELD_first_ACK_range
        FD_ISARRAY | FD_ISCOMPOUND,    // FIELD_ACK_ranges
    };
    return (field >= 0 && field < 5) ? fieldTypeFlags[field] : 0;
}

const char *ACKFrameDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "largest_acknowledged",
        "ACK_delay",
        "ACK_range_count",
        "first_ACK_range",
        "ACK_ranges",
    };
    return (field >= 0 && field < 5) ? fieldNames[field] : nullptr;
}

int ACKFrameDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0] == 'l' && strcmp(fieldName, "largest_acknowledged") == 0) return base+0;
    if (fieldName[0] == 'A' && strcmp(fieldName, "ACK_delay") == 0) return base+1;
    if (fieldName[0] == 'A' && strcmp(fieldName, "ACK_range_count") == 0) return base+2;
    if (fieldName[0] == 'f' && strcmp(fieldName, "first_ACK_range") == 0) return base+3;
    if (fieldName[0] == 'A' && strcmp(fieldName, "ACK_ranges") == 0) return base+4;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *ACKFrameDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_largest_acknowledged
        "int",    // FIELD_ACK_delay
        "int",    // FIELD_ACK_range_count
        "int",    // FIELD_first_ACK_range
        "range",    // FIELD_ACK_ranges
    };
    return (field >= 0 && field < 5) ? fieldTypeStrings[field] : nullptr;
}

const char **ACKFrameDescriptor::getFieldPropertyNames(int field) const
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

const char *ACKFrameDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int ACKFrameDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    ACKFrame *pp = (ACKFrame *)object; (void)pp;
    switch (field) {
        case FIELD_ACK_ranges: return pp->getACK_rangesArraySize();
        default: return 0;
    }
}

const char *ACKFrameDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    ACKFrame *pp = (ACKFrame *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string ACKFrameDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    ACKFrame *pp = (ACKFrame *)object; (void)pp;
    switch (field) {
        case FIELD_largest_acknowledged: return long2string(pp->getLargest_acknowledged());
        case FIELD_ACK_delay: return long2string(pp->getACK_delay());
        case FIELD_ACK_range_count: return long2string(pp->getACK_range_count());
        case FIELD_first_ACK_range: return long2string(pp->getFirst_ACK_range());
        case FIELD_ACK_ranges: {std::stringstream out; out << pp->getACK_ranges(i); return out.str();}
        default: return "";
    }
}

bool ACKFrameDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    ACKFrame *pp = (ACKFrame *)object; (void)pp;
    switch (field) {
        case FIELD_largest_acknowledged: pp->setLargest_acknowledged(string2long(value)); return true;
        case FIELD_ACK_delay: pp->setACK_delay(string2long(value)); return true;
        case FIELD_ACK_range_count: pp->setACK_range_count(string2long(value)); return true;
        case FIELD_first_ACK_range: pp->setFirst_ACK_range(string2long(value)); return true;
        default: return false;
    }
}

const char *ACKFrameDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case FIELD_ACK_ranges: return omnetpp::opp_typename(typeid(range));
        default: return nullptr;
    };
}

void *ACKFrameDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    ACKFrame *pp = (ACKFrame *)object; (void)pp;
    switch (field) {
        case FIELD_ACK_ranges: return toVoidPtr(&pp->getACK_ranges(i)); break;
        default: return nullptr;
    }
}

} // namespace inet

