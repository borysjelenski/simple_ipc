#ifndef UTILS_H_
#define UTILS_H_

#include <cstdint>
#include <limits>
#include <optional>
#include <string>
#include <ostream>

namespace utils
{

size_t write_32be(uint8_t* buf, uint32_t value);
uint32_t read_32be(const uint8_t* buf);
const uint8_t* read_32be(const uint8_t* buf, uint32_t& out_value);
std::optional<uint32_t> str_to_u32(const std::string& str);

}

#endif
