//
// Generated file, do not edit! Created by opp_msgtool 6.0 from wmsg.msg.
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
#include <type_traits>
#include "wmsg_m.h"

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

Register_Class(wmsg)

wmsg::wmsg(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

wmsg::wmsg(const wmsg& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

wmsg::~wmsg()
{
}

wmsg& wmsg::operator=(const wmsg& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void wmsg::copy(const wmsg& other)
{
    this->sourceID = other.sourceID;
    this->destID = other.destID;
    this->sourceRank = other.sourceRank;
    this->sourcePriority = other.sourcePriority;
    this->sourceBattery = other.sourceBattery;
    this->assignpriority = other.assignpriority;
    this->sourceNetworkedFlag = other.sourceNetworkedFlag;
    this->x = other.x;
    this->y = other.y;
    this->lastActionTime = other.lastActionTime;
    this->networkingToSonFlagMsg = other.networkingToSonFlagMsg;
    this->networkingToSonFlagSelectedId = other.networkingToSonFlagSelectedId;
    this->sourcePacketLength = other.sourcePacketLength;
    this->backwardScore = other.backwardScore;
}

void wmsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->sourceID);
    doParsimPacking(b,this->destID);
    doParsimPacking(b,this->sourceRank);
    doParsimPacking(b,this->sourcePriority);
    doParsimPacking(b,this->sourceBattery);
    doParsimPacking(b,this->assignpriority);
    doParsimPacking(b,this->sourceNetworkedFlag);
    doParsimPacking(b,this->x);
    doParsimPacking(b,this->y);
    doParsimPacking(b,this->lastActionTime);
    doParsimPacking(b,this->networkingToSonFlagMsg);
    doParsimPacking(b,this->networkingToSonFlagSelectedId);
    doParsimPacking(b,this->sourcePacketLength);
    doParsimPacking(b,this->backwardScore);
}

void wmsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->sourceID);
    doParsimUnpacking(b,this->destID);
    doParsimUnpacking(b,this->sourceRank);
    doParsimUnpacking(b,this->sourcePriority);
    doParsimUnpacking(b,this->sourceBattery);
    doParsimUnpacking(b,this->assignpriority);
    doParsimUnpacking(b,this->sourceNetworkedFlag);
    doParsimUnpacking(b,this->x);
    doParsimUnpacking(b,this->y);
    doParsimUnpacking(b,this->lastActionTime);
    doParsimUnpacking(b,this->networkingToSonFlagMsg);
    doParsimUnpacking(b,this->networkingToSonFlagSelectedId);
    doParsimUnpacking(b,this->sourcePacketLength);
    doParsimUnpacking(b,this->backwardScore);
}

int wmsg::getSourceID() const
{
    return this->sourceID;
}

void wmsg::setSourceID(int sourceID)
{
    this->sourceID = sourceID;
}

int wmsg::getDestID() const
{
    return this->destID;
}

void wmsg::setDestID(int destID)
{
    this->destID = destID;
}

int wmsg::getSourceRank() const
{
    return this->sourceRank;
}

void wmsg::setSourceRank(int sourceRank)
{
    this->sourceRank = sourceRank;
}

int wmsg::getSourcePriority() const
{
    return this->sourcePriority;
}

void wmsg::setSourcePriority(int sourcePriority)
{
    this->sourcePriority = sourcePriority;
}

double wmsg::getSourceBattery() const
{
    return this->sourceBattery;
}

void wmsg::setSourceBattery(double sourceBattery)
{
    this->sourceBattery = sourceBattery;
}

int wmsg::getAssignpriority() const
{
    return this->assignpriority;
}

void wmsg::setAssignpriority(int assignpriority)
{
    this->assignpriority = assignpriority;
}

int wmsg::getSourceNetworkedFlag() const
{
    return this->sourceNetworkedFlag;
}

void wmsg::setSourceNetworkedFlag(int sourceNetworkedFlag)
{
    this->sourceNetworkedFlag = sourceNetworkedFlag;
}

double wmsg::getX() const
{
    return this->x;
}

void wmsg::setX(double x)
{
    this->x = x;
}

double wmsg::getY() const
{
    return this->y;
}

void wmsg::setY(double y)
{
    this->y = y;
}

int wmsg::getLastActionTime() const
{
    return this->lastActionTime;
}

void wmsg::setLastActionTime(int lastActionTime)
{
    this->lastActionTime = lastActionTime;
}

int wmsg::getNetworkingToSonFlagMsg() const
{
    return this->networkingToSonFlagMsg;
}

void wmsg::setNetworkingToSonFlagMsg(int networkingToSonFlagMsg)
{
    this->networkingToSonFlagMsg = networkingToSonFlagMsg;
}

int wmsg::getNetworkingToSonFlagSelectedId() const
{
    return this->networkingToSonFlagSelectedId;
}

void wmsg::setNetworkingToSonFlagSelectedId(int networkingToSonFlagSelectedId)
{
    this->networkingToSonFlagSelectedId = networkingToSonFlagSelectedId;
}

int wmsg::getSourcePacketLength() const
{
    return this->sourcePacketLength;
}

void wmsg::setSourcePacketLength(int sourcePacketLength)
{
    this->sourcePacketLength = sourcePacketLength;
}

double wmsg::getBackwardScore() const
{
    return this->backwardScore;
}

void wmsg::setBackwardScore(double backwardScore)
{
    this->backwardScore = backwardScore;
}

class wmsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_sourceID,
        FIELD_destID,
        FIELD_sourceRank,
        FIELD_sourcePriority,
        FIELD_sourceBattery,
        FIELD_assignpriority,
        FIELD_sourceNetworkedFlag,
        FIELD_x,
        FIELD_y,
        FIELD_lastActionTime,
        FIELD_networkingToSonFlagMsg,
        FIELD_networkingToSonFlagSelectedId,
        FIELD_sourcePacketLength,
        FIELD_backwardScore,
    };
  public:
    wmsgDescriptor();
    virtual ~wmsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(wmsgDescriptor)

wmsgDescriptor::wmsgDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(wmsg)), "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

wmsgDescriptor::~wmsgDescriptor()
{
    delete[] propertyNames;
}

bool wmsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<wmsg *>(obj)!=nullptr;
}

const char **wmsgDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *wmsgDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int wmsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 14+base->getFieldCount() : 14;
}

unsigned int wmsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_sourceID
        FD_ISEDITABLE,    // FIELD_destID
        FD_ISEDITABLE,    // FIELD_sourceRank
        FD_ISEDITABLE,    // FIELD_sourcePriority
        FD_ISEDITABLE,    // FIELD_sourceBattery
        FD_ISEDITABLE,    // FIELD_assignpriority
        FD_ISEDITABLE,    // FIELD_sourceNetworkedFlag
        FD_ISEDITABLE,    // FIELD_x
        FD_ISEDITABLE,    // FIELD_y
        FD_ISEDITABLE,    // FIELD_lastActionTime
        FD_ISEDITABLE,    // FIELD_networkingToSonFlagMsg
        FD_ISEDITABLE,    // FIELD_networkingToSonFlagSelectedId
        FD_ISEDITABLE,    // FIELD_sourcePacketLength
        FD_ISEDITABLE,    // FIELD_backwardScore
    };
    return (field >= 0 && field < 14) ? fieldTypeFlags[field] : 0;
}

const char *wmsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "sourceID",
        "destID",
        "sourceRank",
        "sourcePriority",
        "sourceBattery",
        "assignpriority",
        "sourceNetworkedFlag",
        "x",
        "y",
        "lastActionTime",
        "networkingToSonFlagMsg",
        "networkingToSonFlagSelectedId",
        "sourcePacketLength",
        "backwardScore",
    };
    return (field >= 0 && field < 14) ? fieldNames[field] : nullptr;
}

int wmsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "sourceID") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "destID") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "sourceRank") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "sourcePriority") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "sourceBattery") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "assignpriority") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "sourceNetworkedFlag") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "x") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "y") == 0) return baseIndex + 8;
    if (strcmp(fieldName, "lastActionTime") == 0) return baseIndex + 9;
    if (strcmp(fieldName, "networkingToSonFlagMsg") == 0) return baseIndex + 10;
    if (strcmp(fieldName, "networkingToSonFlagSelectedId") == 0) return baseIndex + 11;
    if (strcmp(fieldName, "sourcePacketLength") == 0) return baseIndex + 12;
    if (strcmp(fieldName, "backwardScore") == 0) return baseIndex + 13;
    return base ? base->findField(fieldName) : -1;
}

const char *wmsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_sourceID
        "int",    // FIELD_destID
        "int",    // FIELD_sourceRank
        "int",    // FIELD_sourcePriority
        "double",    // FIELD_sourceBattery
        "int",    // FIELD_assignpriority
        "int",    // FIELD_sourceNetworkedFlag
        "double",    // FIELD_x
        "double",    // FIELD_y
        "int",    // FIELD_lastActionTime
        "int",    // FIELD_networkingToSonFlagMsg
        "int",    // FIELD_networkingToSonFlagSelectedId
        "int",    // FIELD_sourcePacketLength
        "double",    // FIELD_backwardScore
    };
    return (field >= 0 && field < 14) ? fieldTypeStrings[field] : nullptr;
}

const char **wmsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *wmsgDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int wmsgDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    wmsg *pp = omnetpp::fromAnyPtr<wmsg>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void wmsgDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    wmsg *pp = omnetpp::fromAnyPtr<wmsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'wmsg'", field);
    }
}

const char *wmsgDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    wmsg *pp = omnetpp::fromAnyPtr<wmsg>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string wmsgDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    wmsg *pp = omnetpp::fromAnyPtr<wmsg>(object); (void)pp;
    switch (field) {
        case FIELD_sourceID: return long2string(pp->getSourceID());
        case FIELD_destID: return long2string(pp->getDestID());
        case FIELD_sourceRank: return long2string(pp->getSourceRank());
        case FIELD_sourcePriority: return long2string(pp->getSourcePriority());
        case FIELD_sourceBattery: return double2string(pp->getSourceBattery());
        case FIELD_assignpriority: return long2string(pp->getAssignpriority());
        case FIELD_sourceNetworkedFlag: return long2string(pp->getSourceNetworkedFlag());
        case FIELD_x: return double2string(pp->getX());
        case FIELD_y: return double2string(pp->getY());
        case FIELD_lastActionTime: return long2string(pp->getLastActionTime());
        case FIELD_networkingToSonFlagMsg: return long2string(pp->getNetworkingToSonFlagMsg());
        case FIELD_networkingToSonFlagSelectedId: return long2string(pp->getNetworkingToSonFlagSelectedId());
        case FIELD_sourcePacketLength: return long2string(pp->getSourcePacketLength());
        case FIELD_backwardScore: return double2string(pp->getBackwardScore());
        default: return "";
    }
}

void wmsgDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    wmsg *pp = omnetpp::fromAnyPtr<wmsg>(object); (void)pp;
    switch (field) {
        case FIELD_sourceID: pp->setSourceID(string2long(value)); break;
        case FIELD_destID: pp->setDestID(string2long(value)); break;
        case FIELD_sourceRank: pp->setSourceRank(string2long(value)); break;
        case FIELD_sourcePriority: pp->setSourcePriority(string2long(value)); break;
        case FIELD_sourceBattery: pp->setSourceBattery(string2double(value)); break;
        case FIELD_assignpriority: pp->setAssignpriority(string2long(value)); break;
        case FIELD_sourceNetworkedFlag: pp->setSourceNetworkedFlag(string2long(value)); break;
        case FIELD_x: pp->setX(string2double(value)); break;
        case FIELD_y: pp->setY(string2double(value)); break;
        case FIELD_lastActionTime: pp->setLastActionTime(string2long(value)); break;
        case FIELD_networkingToSonFlagMsg: pp->setNetworkingToSonFlagMsg(string2long(value)); break;
        case FIELD_networkingToSonFlagSelectedId: pp->setNetworkingToSonFlagSelectedId(string2long(value)); break;
        case FIELD_sourcePacketLength: pp->setSourcePacketLength(string2long(value)); break;
        case FIELD_backwardScore: pp->setBackwardScore(string2double(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'wmsg'", field);
    }
}

omnetpp::cValue wmsgDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    wmsg *pp = omnetpp::fromAnyPtr<wmsg>(object); (void)pp;
    switch (field) {
        case FIELD_sourceID: return pp->getSourceID();
        case FIELD_destID: return pp->getDestID();
        case FIELD_sourceRank: return pp->getSourceRank();
        case FIELD_sourcePriority: return pp->getSourcePriority();
        case FIELD_sourceBattery: return pp->getSourceBattery();
        case FIELD_assignpriority: return pp->getAssignpriority();
        case FIELD_sourceNetworkedFlag: return pp->getSourceNetworkedFlag();
        case FIELD_x: return pp->getX();
        case FIELD_y: return pp->getY();
        case FIELD_lastActionTime: return pp->getLastActionTime();
        case FIELD_networkingToSonFlagMsg: return pp->getNetworkingToSonFlagMsg();
        case FIELD_networkingToSonFlagSelectedId: return pp->getNetworkingToSonFlagSelectedId();
        case FIELD_sourcePacketLength: return pp->getSourcePacketLength();
        case FIELD_backwardScore: return pp->getBackwardScore();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'wmsg' as cValue -- field index out of range?", field);
    }
}

void wmsgDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    wmsg *pp = omnetpp::fromAnyPtr<wmsg>(object); (void)pp;
    switch (field) {
        case FIELD_sourceID: pp->setSourceID(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_destID: pp->setDestID(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_sourceRank: pp->setSourceRank(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_sourcePriority: pp->setSourcePriority(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_sourceBattery: pp->setSourceBattery(value.doubleValue()); break;
        case FIELD_assignpriority: pp->setAssignpriority(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_sourceNetworkedFlag: pp->setSourceNetworkedFlag(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_x: pp->setX(value.doubleValue()); break;
        case FIELD_y: pp->setY(value.doubleValue()); break;
        case FIELD_lastActionTime: pp->setLastActionTime(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_networkingToSonFlagMsg: pp->setNetworkingToSonFlagMsg(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_networkingToSonFlagSelectedId: pp->setNetworkingToSonFlagSelectedId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_sourcePacketLength: pp->setSourcePacketLength(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_backwardScore: pp->setBackwardScore(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'wmsg'", field);
    }
}

const char *wmsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr wmsgDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    wmsg *pp = omnetpp::fromAnyPtr<wmsg>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void wmsgDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    wmsg *pp = omnetpp::fromAnyPtr<wmsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'wmsg'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

