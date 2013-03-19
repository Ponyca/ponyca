#include <cxxtest/TestSuite.h>
#include "net/network.h"

using namespace Ponyca;

class TestContainer : public Net::AbstractSerializable {
public:
    virtual std::string serialize() const {
        std::string buffer;
        buffer += serializeInt32(value);
        return buffer;
    }
    virtual uint16_t unserialize(const char *buffer) {
        uint16_t offset = 0;
        offset += unserializeInt32(buffer+offset, value);
        return offset;
    }
    int32_t value;
};

class TestType : public Net::AbstractSerializable {
public:
    virtual std::string serialize() const {
        std::string buffer;
        buffer += serializeInt32(fieldInt);
        buffer += serializeFloat32(fieldFloat);
        buffer += serializeString(fieldString);
        buffer += serializeString(fieldString2);
        buffer += serializeList(fieldList);
        buffer += serializeMap(fieldMap);
        return buffer;
    }
    virtual uint16_t unserialize(const char *buffer) {
        uint16_t offset = 0;
        offset += unserializeInt32(buffer+offset, fieldInt);
        offset += unserializeFloat32(buffer+offset, fieldFloat);
        offset += unserializeString(buffer+offset, fieldString);
        offset += unserializeString(buffer+offset, fieldString2);
        offset += unserializeList(buffer+offset, fieldList);
        offset += unserializeMap(buffer+offset, fieldMap);
        return offset;
    }
     
    int32_t fieldInt;
    float fieldFloat;
    std::string fieldString;
    std::string fieldString2;
    std::vector<TestContainer> fieldList;
    std::map<std::string, TestContainer> fieldMap;
};

class TestSerialization : public CxxTest::TestSuite
{
public:
    void testWithTestType(void)
    {
        TestType t;
        t.fieldInt = 42;
        t.fieldFloat = 13.37;
        t.fieldString = "ponyca";
        t.fieldString2 = u8"café";

        TestContainer c1, c2;
        c1.value = 42;
        c2.value = 43;
        t.fieldList.push_back(c1);
        t.fieldList.push_back(c2);

        t.fieldMap["woot"] = c1;
        t.fieldMap[u8"café"] = c2;

        const char *serialized(t.serialize().c_str());

        TestType t2;
        t2.unserialize(serialized);

        TS_ASSERT_EQUALS(t2.fieldInt, 42);
        TS_ASSERT_DELTA(t2.fieldFloat, 13.37, 0.001);
        TS_ASSERT_EQUALS(t2.fieldString, "ponyca");
        TS_ASSERT_EQUALS(t2.fieldString2, u8"café");
        TS_ASSERT_EQUALS(t2.fieldList[0].value, 42);
        TS_ASSERT_EQUALS(t2.fieldList[1].value, 43);
        TS_ASSERT_EQUALS(t2.fieldMap["woot"].value, 42);
        TS_ASSERT_EQUALS(t2.fieldMap[u8"café"].value, 43);
    }
};
