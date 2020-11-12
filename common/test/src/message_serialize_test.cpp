#include <CppUTest/TestHarness.h>
#include <cstring>
#include <array>
#include <sstream>
#include "message.h"

TEST_GROUP(MessageSerialize_TestGroup)
{
};

TEST(MessageSerialize_TestGroup, Serialize_SetWidthMsg)
{
  Set_width_msg msg{0x1A2B3C4D};
  const uint8_t expected[] = { (uint8_t)Msg_type::set_width, 0x1A, 0x2B, 0x3C, 0x4D };

  uint8_t actual[32];
  const auto msg_len = msg.serialize(actual, sizeof(actual));
  
  CHECK_EQUAL(sizeof(expected), msg_len);
  MEMCMP_EQUAL(expected, actual, msg_len);
}

TEST(MessageSerialize_TestGroup, Serialize_Render)
{
  const char* filename = "/home/images/landscape.png";
  Render_msg msg{filename};

  uint8_t buf[256];
  const auto msg_len = msg.serialize(buf, sizeof(buf));
  buf[msg_len] = 0;

  CHECK_EQUAL(2 + strlen(filename), msg_len);
  MEMCMP_EQUAL(filename, &buf[2], strlen(filename));
}
