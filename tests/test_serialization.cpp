#include <cxxtest/TestSuite.h>
#include "net/network.h"

using namespace Ponyca;

class TestType : public Net::AbstractSerializable {
public:
    virtual std::string serialize() const {
        std::string buffer;
        buffer += serializeInt32(fieldInt);
        buffer += serializeFloat32(fieldFloat);
        buffer += serializeString(fieldString);
        buffer += serializeString(fieldString2);
        return buffer;
    }
    virtual uint16_t unserialize(const char *buffer) {
        uint16_t offset = 0;
        offset += unserializeInt32(buffer+offset, fieldInt);
        offset += unserializeFloat32(buffer+offset, fieldFloat);
        offset += unserializeString(buffer+offset, fieldString);
        offset += unserializeString(buffer+offset, fieldString2);
        return offset;
    }
     
    int32_t fieldInt;
    float fieldFloat;
    std::string fieldString;
    std::string fieldString2;
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

        const char *serialized(t.serialize().c_str());

        TestType t2;
        t2.unserialize(serialized);

        TS_ASSERT_EQUALS(t2.fieldInt, 42);
        TS_ASSERT_DELTA(t2.fieldFloat, 13.37, 0.001);
        TS_ASSERT_EQUALS(t2.fieldString, "ponyca");
        TS_ASSERT_EQUALS(t2.fieldString2, u8"café");
    }
};
