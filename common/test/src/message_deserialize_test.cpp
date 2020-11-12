#include <CppUTest/TestHarness.h>
#include <cstring>
#include <sstream>
#include "message.h"

TEST_GROUP(MessageDeserialize_TestGroup)
{
};

TEST(MessageDeserialize_TestGroup, Deserialize_SetWidthMsg)
{
  const uint8_t data[] = { (uint8_t)Msg_type::set_width, 0x1A, 0x2B, 0x3C, 0x4D };

  const auto msg = Message::deserialize(data, sizeof(data));
  const auto concrete_msg = dynamic_cast<const Set_width_msg*>(msg.get());

  CHECK((bool)msg);
  CHECK_EQUAL(0x1A2B3C4D, concrete_msg->width);
}

TEST(MessageDeserialize_TestGroup, Deserialize_Render)
{
  const char* filename = "/home/images/landscape.png";
  uint8_t buf[256];
  buf[0] = (uint8_t)Msg_type::render;
  buf[1] = strlen(filename);
  strcpy((char*)&buf[2], filename);

  const auto msg = Message::deserialize(buf, 2 + strlen(filename));
  const auto concrete_msg = dynamic_cast<const Render_msg*>(msg.get());

  CHECK((bool)msg);
  CHECK_EQUAL(concrete_msg->filename.length(), strlen(filename));
  STRCMP_EQUAL(filename, concrete_msg->filename.c_str());
}
