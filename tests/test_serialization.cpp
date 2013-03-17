#include <cxxtest/TestSuite.h>
#include "net/network.h"

using namespace Ponyca;

class TestType : public Net::AbstractSerializable {
public:
    virtual std::string serialize() const {
        std::string buffer;
        buffer += serializeInt32(fieldInt);
        buffer += serializeFloat32(fieldFloat);
        return buffer;
    }
    virtual uint16_t unserialize(std::string const &buffer) {
        char const *cstr = buffer.c_str();
        uint16_t offset = 0;
        offset += unserializeInt32(cstr+offset, fieldInt);
        offset += unserializeFloat32(cstr+offset, fieldFloat);
        return offset;
    }
     
    int32_t fieldInt;
    float fieldFloat;
};

class TestSerialization : public CxxTest::TestSuite
{
public:
    void testWithTestType(void)
    {
        TestType t;
        t.fieldInt = 42;
        t.fieldFloat = 13.37;

        std::string serialized(t.serialize());

        TestType t2;
        t2.unserialize(serialized);

        TS_ASSERT_EQUALS(t2.fieldInt, 42);
        TS_ASSERT_DELTA(t2.fieldFloat, 13.37, 0.001);
    }
};
