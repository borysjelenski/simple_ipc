#include <cstring>
#include <memory>
#include <array>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include "message.h"
#include "print_msg_visitor.h"
#include "draw_msg_visitor.h"

#define HANDLE_SYS_ERR(rc, msg) \
  if ((rc) < 0) { \
    std::cerr << "\n[Program 2 error]\n"; \
    perror((msg)); \
    return EXIT_FAILURE; \
  }

static volatile bool m_exit_flag = false;

static void sigint_handler(int)
{
  m_exit_flag = true;
}

int main(int argc, char* argv[])
{
  // setup signals
  struct sigaction signal_action;
  signal_action.sa_handler = sigint_handler;
  sigemptyset(&signal_action.sa_mask);
  signal_action.sa_flags = 0;
  sigaction(SIGINT, &signal_action, NULL);
  sigaction(SIGPIPE, &signal_action, NULL);

  if (argc < 3) {
    std::cerr << "Missing arguments\n";
    return EXIT_FAILURE;
  }

  static const auto req_pipe_filename = argv[1];
  static const auto resp_pipe_filename = argv[2];

  // close pipe file descriptor upon exit
  const auto pipe_fd_del = [](auto* p) { if (close(*p) < 0) { perror("Failed to close pipe"); } };

  // open the request pipe
  const int req_pipe_fd = open(req_pipe_filename, O_RDONLY);
  HANDLE_SYS_ERR(req_pipe_fd, "Failed to open the request pipe");
  std::unique_ptr<const int, decltype(pipe_fd_del)> req_pipe_fd_raii{&req_pipe_fd, pipe_fd_del};

  // open the response pipe
  const int resp_pipe_fd = open(resp_pipe_filename, O_WRONLY);
  HANDLE_SYS_ERR(resp_pipe_fd, "Failed to open the response pipe");
  std::unique_ptr<const int, decltype(pipe_fd_del)> resp_pipe_fd_raii{&resp_pipe_fd, pipe_fd_del};

  std::ostringstream err_stream;
  Draw_msg_visitor draw_visitor{err_stream};
  Print_msg_visitor print_visitor;

  while (!m_exit_flag) {
    static uint8_t msg_buf[256];
    static uint8_t resp_buf[256];
    auto resp_buf_len_ptr = &resp_buf[0];
    auto resp_buf_data_ptr = &resp_buf[1];

    uint8_t msg_len;

    {
      // read message length (first byte of data)
      const int rc = read(req_pipe_fd, &msg_len, 1);
      if (errno == EINTR) { return EXIT_SUCCESS; }
      HANDLE_SYS_ERR(rc, "Failed to read message length");
      if (rc == 0) {
        // end of communication
        break;
      }
    }
    
    // read the message payload
    const int rc = read(req_pipe_fd, msg_buf, msg_len);
    if (errno == EINTR) { return EXIT_SUCCESS; }
    HANDLE_SYS_ERR(rc, "Failed to read the message");
    if (rc == 0) {
      // end of communication
      break;
    }

    // NOTE: after processing the message, a response is sent in the [length, 1 byte][text, 0-255 bytes] format
    // NOTE: a success is reported by sending only the length byte with the value '0'

    const auto msg = Message::deserialize(msg_buf, msg_len);
    if (msg) {
      // process the message
      // NOTE: errors are written to the error stream, nothing is written on success
      err_stream.str({});
      msg->accept(draw_visitor);
      // prepare the response
      const auto err_resp_len = std::min(err_stream.str().length(), sizeof(resp_buf) - 1);
      *resp_buf_len_ptr = err_resp_len;
      memcpy(resp_buf_data_ptr, err_stream.str().c_str(), err_resp_len);
    } else {
      // message could not be serialized, should not happen
      static constexpr auto resp_msg = "IPC error";
      *resp_buf_len_ptr = strlen(resp_msg);
      memcpy(resp_buf_data_ptr, resp_msg, strlen(resp_msg));
    }

    // send the response
    const auto resp_len = *resp_buf_len_ptr;
    HANDLE_SYS_ERR(write(resp_pipe_fd, resp_buf, 1 + resp_len), "Failed to write to request pipe");
  }

  return 0;
}
