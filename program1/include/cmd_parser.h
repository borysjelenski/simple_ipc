#ifndef CMD_PARSER_H
#define CMD_PARSER_H

#include <memory>
#include <vector>
#include <string>
#include "message.h"

namespace cmd_parser
{

using Token_list = std::vector<std::string>;
using Parse_func = std::unique_ptr<Message> (*)(const Token_list&);

Token_list tokenize_cmd_line(char* cmd_line);
std::unique_ptr<Message> parse_cmd_tokens(const Token_list&);
std::unique_ptr<Message> parse_set_width_cmd(const Token_list&);
std::unique_ptr<Message> parse_set_height_cmd(const Token_list&);
std::unique_ptr<Message> parse_draw_rectangle_cmd(const Token_list&);
std::unique_ptr<Message> parse_draw_triangle_cmd(const Token_list&);
std::unique_ptr<Message> parse_render_cmd(const Token_list&);
std::unique_ptr<Message> parse_cmd_line(char* cmd_line);

}

#endif
