#include <cstring>
#include "message.h"
#include "utils.h"

std::unique_ptr<Message> Message::deserialize(const uint8_t* data, size_t data_len)
{
  if (data_len == 0) { return {}; }

  const auto msg_type = (Msg_type)data[0];
  switch ((Msg_type)msg_type) {
    case Msg_type::set_width: return Set_width_msg::deserialize(data, data_len);
    case Msg_type::set_height: return Set_height_msg::deserialize(data, data_len);
    case Msg_type::draw_rectangle: return Draw_rectangle_msg::deserialize(data, data_len);
    case Msg_type::draw_triangle: return Draw_triangle_msg::deserialize(data, data_len);
    case Msg_type::render: return Render_msg::deserialize(data, data_len);
    default: return {};
  }
}

Set_width_msg::Set_width_msg(uint32_t width) : width{width} {}

size_t Set_width_msg::serialize(uint8_t* out_buf, size_t out_buf_size) const
{
  if (out_buf_size < fixed_length) { return 0; }

  out_buf[0] = (uint8_t)Msg_type::set_width;
  utils::write_32be(&out_buf[1], width);

  return fixed_length;
}

std::unique_ptr<Message> Set_width_msg::deserialize(const uint8_t* data, size_t data_len)
{
  if (data_len < fixed_length) { return {}; }
  if ((Msg_type)data[0] != Msg_type::set_width) { return {}; }

  const auto width = utils::read_32be(&data[1]);

  return std::make_unique<Set_width_msg>(width);
}

Set_height_msg::Set_height_msg(uint32_t height) : height{height} {}

size_t Set_height_msg::serialize(uint8_t* out_buf, size_t out_buf_size) const
{
  if (out_buf_size < fixed_length) { return 0; }

  out_buf[0] = (uint8_t)Msg_type::set_height;
  utils::write_32be(&out_buf[1], height);

  return fixed_length;
}

std::unique_ptr<Message> Set_height_msg::deserialize(const uint8_t* data, size_t data_len)
{
  if (data_len < fixed_length) { return {}; }
  if ((Msg_type)data[0] != Msg_type::set_height) { return {}; }

  const auto height = utils::read_32be(&data[1]);

  return std::make_unique<Set_height_msg>(height);
}

Draw_rectangle_msg::Draw_rectangle_msg(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
  : x{x}, y{y}, width{width}, height{height} {}

size_t Draw_rectangle_msg::serialize(uint8_t* out_buf, size_t out_buf_size) const
{
  if (out_buf_size < fixed_length) { return 0; }

  out_buf[0] = (uint8_t)Msg_type::draw_rectangle;
  auto out_ptr = &out_buf[1];
  out_ptr += utils::write_32be(out_ptr, x);
  out_ptr += utils::write_32be(out_ptr, y);
  out_ptr += utils::write_32be(out_ptr, width);
  out_ptr += utils::write_32be(out_ptr, height);

  return fixed_length;
}

std::unique_ptr<Message> Draw_rectangle_msg::deserialize(const uint8_t* data, size_t data_len)
{
  if (data_len < fixed_length) { return {}; }
  if ((Msg_type)data[0] != Msg_type::draw_rectangle) { return {}; }
  
  const auto* data_ptr = &data[1];
  uint32_t x, y, width, height;
  data_ptr = utils::read_32be(data_ptr, x);
  data_ptr = utils::read_32be(data_ptr, y);
  data_ptr = utils::read_32be(data_ptr, width);
  data_ptr = utils::read_32be(data_ptr, height);
  
  return std::make_unique<Draw_rectangle_msg>(x, y, width ,height);
}

Draw_triangle_msg::Draw_triangle_msg(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t x3, uint32_t y3)
  : x1{x1}, y1{y1}, x2{x2}, y2{y2}, x3{x3}, y3{y3} {}

size_t Draw_triangle_msg::serialize(uint8_t* out_buf, size_t out_buf_size) const
{
  if (out_buf_size < fixed_length) { return 0; }

  out_buf[0] = (uint8_t)Msg_type::draw_triangle;
  auto out_ptr = &out_buf[1];
  out_ptr += utils::write_32be(out_ptr, x1);
  out_ptr += utils::write_32be(out_ptr, y1);
  out_ptr += utils::write_32be(out_ptr, x2);
  out_ptr += utils::write_32be(out_ptr, y2);
  out_ptr += utils::write_32be(out_ptr, x3);
  out_ptr += utils::write_32be(out_ptr, y3);

  return fixed_length;
}

std::unique_ptr<Message> Draw_triangle_msg::deserialize(const uint8_t* data, size_t data_len)
{
  if (data_len < fixed_length) { return {}; }
  if ((Msg_type)data[0] != Msg_type::draw_triangle) { return {}; }
  
  const auto* data_ptr = &data[1];
  uint32_t x1, y1, x2, y2, x3, y3;
  data_ptr = utils::read_32be(data_ptr, x1);
  data_ptr = utils::read_32be(data_ptr, y1);
  data_ptr = utils::read_32be(data_ptr, x2);
  data_ptr = utils::read_32be(data_ptr, y2);
  data_ptr = utils::read_32be(data_ptr, x3);
  data_ptr = utils::read_32be(data_ptr, y3);
  
  return std::make_unique<Draw_triangle_msg>(x1, y1, x2, y2, x3, y3);
}

Render_msg::Render_msg(const std::string& filename) : filename{filename} {}

size_t Render_msg::serialize(uint8_t* out_buf, size_t out_buf_size) const
{
  if (out_buf_size < std::min(filename.length(), sizeof(uint8_t) - 2)) { return 0; }

  out_buf[0] = (uint8_t)Msg_type::render;
  out_buf[1] = filename.length();
  memcpy(&out_buf[2], filename.c_str(), filename.length());

  return 2 + filename.length();
}

std::unique_ptr<Message> Render_msg::deserialize(const uint8_t* data, size_t data_len)
{
  if (data_len < 2) { return {}; }
  if ((Msg_type)data[0] != Msg_type::render) { return {}; }

  const auto filename_len = data[1];
  if (filename_len > data_len - 2) { return {}; }

  std::string filename{(const char*)&data[2], filename_len};

  return std::make_unique<Render_msg>(std::move(filename));
}
