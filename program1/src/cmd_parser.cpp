#include <cstring>
#include <map>
#include "cmd_parser.h"
#include "utils.h"

cmd_parser::Token_list cmd_parser::tokenize_cmd_line(char* cmd_line)
{
  static constexpr auto delims = " \t,";

  Token_list tokens;
  auto token_ptr = std::strtok(cmd_line, delims);
  
  while (token_ptr) {
    tokens.emplace_back(token_ptr);
    token_ptr = std::strtok(nullptr, delims);
  }

  return tokens;
}

std::unique_ptr<Message> cmd_parser::parse_cmd_tokens(const Token_list& tokens)
{
  static const std::map<std::string, Parse_func> cmd_parsers = {
    {"SET_WIDTH", parse_set_width_cmd},
    {"SET_HEIGHT", parse_set_height_cmd},
    {"DRAW_RECTANGLE", parse_draw_rectangle_cmd},
    {"DRAW_TRIANGLE", parse_draw_triangle_cmd},
    {"RENDER", parse_render_cmd}
  };

  if (tokens.size() == 0) { return {}; }

  const auto& cmd_name = tokens[0];
  const auto parser_it = cmd_parsers.find(cmd_name);
  if (parser_it == cmd_parsers.end()) { return {}; }

  return (parser_it->second)(tokens);
}

std::unique_ptr<Message> cmd_parser::parse_set_width_cmd(const Token_list& tokens)
{
  if (tokens.size() < 2) { return {}; }

  const auto width = utils::str_to_u32(tokens[1]);
  if (!width) { return {}; }

  return std::make_unique<Set_width_msg>(width.value());
}

std::unique_ptr<Message> cmd_parser::parse_set_height_cmd(const Token_list& tokens)
{
  if (tokens.size() < 2) { return {}; }

  const auto height = utils::str_to_u32(tokens[1]);
  if (!height) { return {}; }

  return std::make_unique<Set_height_msg>(height.value());
}

std::unique_ptr<Message> cmd_parser::parse_draw_rectangle_cmd(const Token_list& tokens)
{
  if (tokens.size() < 5) { return {}; }

  const auto x = utils::str_to_u32(tokens[1]);
  const auto y = utils::str_to_u32(tokens[2]);
  const auto height = utils::str_to_u32(tokens[3]);
  const auto width = utils::str_to_u32(tokens[4]);
  if (!x || !y || !width || !height) { return {}; }

  return std::make_unique<Draw_rectangle_msg>(x.value(), y.value(), width.value(), height.value());
}

std::unique_ptr<Message> cmd_parser::parse_draw_triangle_cmd(const Token_list& tokens)
{
  if (tokens.size() < 7) { return {}; }
  
  const auto x1 = utils::str_to_u32(tokens[1]);
  const auto y1 = utils::str_to_u32(tokens[2]);
  const auto x2 = utils::str_to_u32(tokens[3]);
  const auto y2 = utils::str_to_u32(tokens[4]);
  const auto x3 = utils::str_to_u32(tokens[5]);
  const auto y3 = utils::str_to_u32(tokens[6]);
  if (!x1 || !y1 || !x2 || !y2 || !x3 || !y3) { return {}; }

  return std::make_unique<Draw_triangle_msg>(x1.value(), y1.value(), x2.value(), y2.value(), x3.value(), y3.value());
}

std::unique_ptr<Message> cmd_parser::parse_render_cmd(const Token_list& tokens)
{
  if (tokens.size() < 2) { return {}; }
  if (tokens[1].empty()) { return {}; }

  const auto filename = tokens[1];

  return std::make_unique<Render_msg>(filename);
}

std::unique_ptr<Message> cmd_parser::parse_cmd_line(char* cmd_line)
{
  const auto tokens = tokenize_cmd_line(cmd_line);
  return parse_cmd_tokens(tokens);
}
