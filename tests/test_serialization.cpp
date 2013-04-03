#include <cxxtest/TestSuite.h>
#include "net/network.h"

using namespace Ponyca;

class TestContainer : public Net::AbstractSerializable {
public:
    virtual std::string serialize() const {
        std::string buffer;
        buffer += value.serialize();
        return buffer;
    }
    virtual uint16_t unserialize(const char *buffer, uint16_t availableBytes) {
        uint16_t offset = 0;
        offset += value.unserialize(buffer+offset, availableBytes-offset);
        return offset;
    }
    Net::Int32Wrapper value;
};

class TestType : public Net::AbstractSerializable {
public:
    virtual std::string serialize() const {
        std::string buffer;
        buffer += fieldInt.serialize();
        buffer += fieldFloat.serialize();
        buffer += fieldString.serialize();
        buffer += fieldString2.serialize();
        buffer += fieldList.serialize();
        buffer += fieldMap.serialize();
        return buffer;
    }
    virtual uint16_t unserialize(const char *buffer, uint16_t availableBytes) {
        uint16_t offset = 0;
        offset += fieldInt.unserialize(buffer+offset, availableBytes-offset);
        offset += fieldFloat.unserialize(buffer+offset, availableBytes-offset);
        offset += fieldString.unserialize(buffer+offset, availableBytes-offset);
        offset += fieldString2.unserialize(buffer+offset, availableBytes-offset);
        offset += fieldList.unserialize(buffer+offset, availableBytes-offset);
        offset += fieldMap.unserialize(buffer+offset, availableBytes-offset);
        return offset;
    }
     
    Net::Int32Wrapper fieldInt;
    Net::Float32Wrapper fieldFloat;
    Net::String fieldString;
    Net::String fieldString2;
    Net::List<TestContainer> fieldList;
    Net::Map<TestContainer> fieldMap;
};

class TestSerialization : public CxxTest::TestSuite
{
public:
    void testWithTestType(void)
    {
        TestType t;
        t.fieldInt = 42;
        t.fieldFloat = 13.37;
        t.fieldString = std::string("ponyca");
        t.fieldString2 = std::string(u8"café");

        TestContainer c1, c2;
        c1.value = 42;
        c2.value = 43;
        t.fieldList.vector.push_back(c1);
        t.fieldList.vector.push_back(c2);

        t.fieldMap.map["woot"] = c1;
        t.fieldMap.map[u8"café"] = c2;

        std::string serialized(t.serialize());

        TestType t2;
        t2.unserialize(serialized.c_str(), serialized.size());

        TS_ASSERT_EQUALS(t2.fieldInt, 42);/*
        TS_ASSERT_DELTA(t2.fieldFloat, 13.37, 0.001);
        TS_ASSERT_EQUALS(t2.fieldString, "ponyca");
        TS_ASSERT_EQUALS(t2.fieldString2, u8"café");
        TS_ASSERT_EQUALS(t2.fieldList.vector[0].value, 42);
        TS_ASSERT_EQUALS(t2.fieldList.vector[1].value, 43);
        TS_ASSERT_EQUALS(t2.fieldMap.map["woot"].value, 42);
        TS_ASSERT_EQUALS(t2.fieldMap.map[u8"café"].value, 43);
        */
    }
};
