#include <cxxtest/TestSuite.h>
#include <cstring>
#include "common/config.h"

using namespace Ponyca;

class TestConfig : public CxxTest::TestSuite
{
public:
    void testWithConfigManager() {
        ConfigManager &i = ConfigManager::GetInstance();
        char const *help = "the answer to life, the universe and everything";
        i["answer"].define(42, help);

        TS_ASSERT_EQUALS((int32_t)i["answer"].value, 42);
        TS_ASSERT_SAME_DATA((char const*)i["answer"].value, "42", 3);
        TS_ASSERT_EQUALS((float)i["answer"].value, 42.0);
        TS_ASSERT_EQUALS((int32_t)i["answer"].defaultValue, 42);
        TS_ASSERT_SAME_DATA(i["answer"].helpText.c_str(), help, strlen(help)+1);
    }
};
