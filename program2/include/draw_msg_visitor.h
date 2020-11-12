#ifndef DRAW_MSG_VISITOR_H
#define DRAW_MSG_VISITOR_H

#include <iostream>
#include <cairo/cairo.h>
#include "message.h"

class Draw_msg_visitor : public Msg_visitor {
public:
  static constexpr auto default_surface_width = 800;
  static constexpr auto default_surface_height = 600;
  static constexpr auto max_surface_width = 4096;
  static constexpr auto max_surface_height = 4096;

  Draw_msg_visitor(std::ostream& err_stream, int width = default_surface_width, int height = default_surface_height);
  Draw_msg_visitor(const Draw_msg_visitor&) = delete;
  Draw_msg_visitor& operator=(const Draw_msg_visitor&) = delete;
  Draw_msg_visitor(Draw_msg_visitor&&) = delete;
  Draw_msg_visitor& operator=(Draw_msg_visitor&&) = delete;
  ~Draw_msg_visitor();

  void visit(Set_width_msg&) override;
  void visit(Set_height_msg&) override;
  void visit(Draw_rectangle_msg&) override;
  void visit(Draw_triangle_msg&) override;
  void visit(Render_msg&) override;

private:
  void recreate_surface(int width, int height);

  std::ostream& err_stream;
  int draw_surface_width = default_surface_width;
  int draw_surface_height = default_surface_height;
  cairo_surface_t* draw_surface = nullptr;
  cairo_t* draw_ctx = nullptr;
};

#endif
