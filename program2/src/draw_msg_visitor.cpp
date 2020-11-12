#include "draw_msg_visitor.h"

#include <stdexcept>
#include <iostream>
#include <complex>

Draw_msg_visitor::Draw_msg_visitor(std::ostream& err_stream, int width, int height) : err_stream{err_stream}
{
  recreate_surface(width, height);
}

Draw_msg_visitor::~Draw_msg_visitor()
{
  cairo_destroy(draw_ctx);
  cairo_surface_destroy(draw_surface);
}

void Draw_msg_visitor::visit(Set_width_msg& msg)
{
  if (msg.width == 0 || msg.width > max_surface_width) {
    err_stream << "SET_WIDTH " << msg.width << " : " << "invalid width";
    return;
  }

  recreate_surface(msg.width, draw_surface_height);
}

void Draw_msg_visitor::visit(Set_height_msg& msg)
{
  if (msg.height == 0 || msg.height > max_surface_height) {
    err_stream << "SET_HEIGHT " << msg.height << " : invalid height";
    return;
  }

  recreate_surface(draw_surface_width, msg.height);
}

void Draw_msg_visitor::visit(Draw_rectangle_msg& msg)
{
  cairo_set_source_rgb(draw_ctx, 0.0, 0.0, 1.0);
  cairo_rectangle(draw_ctx, msg.x, msg.y, msg.width, msg.height);
  cairo_fill(draw_ctx);
}

void Draw_msg_visitor::visit(Draw_triangle_msg& msg)
{
  const std::complex<double> v1{(double)msg.x1, (double)msg.y1};
  const std::complex<double> v2{(double)msg.x2, (double)msg.y2};
  const std::complex<double> v3{(double)msg.x3, (double)msg.y3};
  const auto a = std::abs(v1 - v2);
  const auto b = std::abs(v2 - v3);
  const auto c = std::abs(v3 - v1);
  if (!(a < b + c && b < a + c && c < a + b)) {
    err_stream << "DRAW_TRIANGLE " << msg.x1 << ", " << msg.y1 << ", " << msg.x2 << ", " << msg.y2 << ", "
      << msg.x3 << ", " << msg.y3 << " : not a triangle";
    return;
  }

  cairo_set_source_rgb (draw_ctx, 1.0, 0.0, 0.0);
  cairo_move_to(draw_ctx, msg.x1, msg.y1);
  cairo_line_to(draw_ctx, msg.x2, msg.y2);
  cairo_line_to(draw_ctx, msg.x3, msg.y3);
  cairo_close_path(draw_ctx);
  cairo_fill(draw_ctx);
}

void Draw_msg_visitor::visit(Render_msg& msg)
{
  const auto rc = cairo_surface_write_to_png(draw_surface, msg.filename.c_str());
  if (rc != CAIRO_STATUS_SUCCESS) {
    err_stream << "RENDER " << msg.filename.c_str() << " : failed to write to file";
  }
}

void Draw_msg_visitor::recreate_surface(int width, int height)
{
  if (width <= 0 || width >= max_surface_width ||
      height <= 0 || height >= max_surface_height) {
    throw std::invalid_argument{"Invalid surface dimensions"};
  }

  if (draw_ctx) { cairo_destroy(draw_ctx); }
  if (draw_surface) { cairo_surface_destroy(draw_surface); }

  draw_surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, width, height);
  if (cairo_surface_status(draw_surface) != CAIRO_STATUS_SUCCESS) {
    throw std::runtime_error{"Failed to create Cairo surface"};
  }

  draw_ctx = cairo_create(draw_surface);
  if (cairo_status(draw_ctx) != CAIRO_STATUS_SUCCESS) {
    throw std::runtime_error{"Failed to create Cairo drawing context"};
  }

  draw_surface_width = width;
  draw_surface_height = height;
}
