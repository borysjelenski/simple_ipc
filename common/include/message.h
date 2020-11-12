#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <memory>
#include <istream>
#include <ostream>
#include <string>

class Msg_visitor;

class Message {
public:
  static std::unique_ptr<Message> deserialize(const uint8_t*, size_t);
  virtual size_t serialize(uint8_t*, size_t) const = 0;
  virtual size_t length() const = 0;
  virtual void accept(Msg_visitor& visitor) = 0;
  virtual ~Message() = default;
};

enum class Msg_type : uint8_t {
  set_width = 0,
  set_height,
  draw_rectangle,
  draw_triangle,
  render,
  _cnt
};

class Set_width_msg;
class Set_height_msg;
class Draw_rectangle_msg;
class Draw_triangle_msg;
class Render_msg;

class Msg_visitor {
public:
  virtual void visit(Set_width_msg&) = 0;
  virtual void visit(Set_height_msg&) = 0;
  virtual void visit(Draw_rectangle_msg&) = 0;
  virtual void visit(Draw_triangle_msg&) = 0;
  virtual void visit(Render_msg&) = 0;
};

class Set_width_msg : public Message {
public:
  static constexpr uint8_t fixed_length = sizeof(Msg_type) + sizeof(uint32_t);
  static std::unique_ptr<Message> deserialize(const uint8_t*, size_t);
  
  Set_width_msg(uint32_t width);
  size_t serialize(uint8_t*, size_t) const override;
  size_t length() const override { return fixed_length; }
  void accept(Msg_visitor& visitor) override { visitor.visit(*this); }
  
  uint32_t width;
};

class Set_height_msg : public Message {
public:
  static constexpr uint8_t fixed_length = sizeof(Msg_type) + sizeof(uint32_t);
  static std::unique_ptr<Message> deserialize(const uint8_t*, size_t);

  Set_height_msg(uint32_t height);
  size_t serialize(uint8_t*, size_t) const override;
  size_t length() const override { return fixed_length; }
  void accept(Msg_visitor& visitor) override { visitor.visit(*this); }

  uint32_t height;
};

class Draw_rectangle_msg : public Message {
public:
  static constexpr uint8_t fixed_length = sizeof(Msg_type) + 4 * sizeof(uint32_t);
  static std::unique_ptr<Message> deserialize(const uint8_t*, size_t);

  Draw_rectangle_msg(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
  size_t serialize(uint8_t*, size_t) const override;
  size_t length() const override { return fixed_length; }
  void accept(Msg_visitor& visitor) override { visitor.visit(*this); }

  uint32_t x;
  uint32_t y;
  uint32_t width;
  uint32_t height;
};

class Draw_triangle_msg : public Message {
public:
  static constexpr uint8_t fixed_length = sizeof(Msg_type) + 6 * sizeof(uint32_t);
  static std::unique_ptr<Message> deserialize(const uint8_t*, size_t);

  Draw_triangle_msg(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t x3, uint32_t y3);
  size_t serialize(uint8_t*, size_t) const override;
  size_t length() const override { return fixed_length; }
  void accept(Msg_visitor& visitor) override { visitor.visit(*this); }

  uint32_t x1;
  uint32_t y1;
  uint32_t x2;
  uint32_t y2;
  uint32_t x3;
  uint32_t y3;
};

class Render_msg : public Message {
public:
  static std::unique_ptr<Message> deserialize(const uint8_t*, size_t);

  Render_msg(const std::string& filename);
  size_t serialize(uint8_t*, size_t) const override;
  size_t length() const override { return 2 + filename.length(); };
  void accept(Msg_visitor& visitor) override { visitor.visit(*this); }

  std::string filename;
};

#endif
