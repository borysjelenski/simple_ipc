#include "utils.h"

size_t utils::write_32be(uint8_t* buf, uint32_t value)
{
  buf[0] = (value >> 24) & 0xFF;
  buf[1] = (value >> 16) & 0xFF;
  buf[2] = (value >> 8) & 0xFF;
  buf[3] = value & 0xFF;

  return 4;
}

uint32_t utils::read_32be(const uint8_t* buf)
{
  uint32_t value = 0;
  value |= buf[0] << 24;
  value |= buf[1] << 16;
  value |= buf[2] << 8;
  value |= buf[3];

  return value;
}

const uint8_t* utils::read_32be(const uint8_t* buf, uint32_t& out_value)
{
  out_value = 0;
  out_value |= buf[0] << 24;
  out_value |= buf[1] << 16;
  out_value |= buf[2] << 8;
  out_value |= buf[3];

  return buf + 4;
}

std::optional<uint32_t> utils::str_to_u32(const std::string& str)
{
  try {
    const auto value = std::stoll(str, 0, 0);

    if (value < std::numeric_limits<uint32_t>::min() ||
        value > std::numeric_limits<uint32_t>::max()) {
      return {};
    }

    return (uint32_t)value;
  } catch (...) {
    return {};
  }
}
