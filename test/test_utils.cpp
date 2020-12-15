#include <gtest/gtest.h>

extern "C"
{
#include "utils.h"
}


TEST(Utils, multiplyBy2)
{
    EXPECT_EQ(8, multiplyBy2(4));
}
