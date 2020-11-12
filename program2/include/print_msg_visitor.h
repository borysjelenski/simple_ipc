#ifndef DRAWING_MSG_VISITOR_
#define DRAWING_MSG_VISITOR_

#include <iostream>
#include "message.h"

class Print_msg_visitor : public Msg_visitor {
public:
  void visit(Set_width_msg& msg) override
  {
    std::cout << "Set_width_msg: " << msg.width << "\n";
  }

  void visit(Set_height_msg& msg) override
  {
    std::cout << "Set_height_msg: " << msg.height << "\n";
  }

  void visit(Draw_rectangle_msg& msg) override
  {
    std::cout << "Draw_rectangle_msg: " << msg.x << ", " << msg.y << ", "
      << msg.width << ", " << msg.height << "\n";
  }

  void visit(Draw_triangle_msg& msg) override
  {
    std::cout << "Draw_triangle_msg: " << msg.x1 << ", " << msg.y1 << ", "
      << msg.x2 << ", " << msg.y2 << ", " << msg.x3 << ", " << msg.y3 << "\n";
  }

  void visit(Render_msg& msg) override
  {
    std::cout << "Render_msg: " << msg.filename << "\n";
  }
};

#endif
