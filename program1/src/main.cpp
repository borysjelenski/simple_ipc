#include <cstdlib>
#include <cstring>
#include <memory>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include "message.h"
#include "cmd_parser.h"

#define HANDLE_SYS_ERR(rc, msg) \
  if ((rc) < 0) { \
    std::cerr << "\n[Program 1 error]\n"; \
    perror((msg)); \
    return EXIT_FAILURE; \
  }
  
static volatile bool m_exit_flag = false;

static void signal_handler(int)
{
  m_exit_flag = true;
}

int main(int argc, char* argv[])
{
  std::ifstream input_file;
  if (argc > 1) {
    input_file.open(argv[1]);
    if (input_file.fail()) {
      std::cerr << "Failed to open the input file " << argv[1] << "\n";
      return EXIT_FAILURE;
    }
  }

  // setup signals
  struct sigaction signal_action;
  signal_action.sa_handler = signal_handler;
  sigemptyset(&signal_action.sa_mask);
  signal_action.sa_flags = 0;
  sigaction(SIGINT, &signal_action, NULL);
  sigaction(SIGPIPE, &signal_action, NULL);

  static constexpr const char* req_pipe_filename = "./req_pipe";
  static constexpr const char* resp_pipe_filename = "./resp_pipe";

  // unlink pipe files upon exit
  const auto pipe_file_del = [](auto* p) { if (unlink(*p) < 0) { perror("Failed to unlink pipe"); } };

  // create the request pipe
  HANDLE_SYS_ERR(mkfifo(req_pipe_filename, 0666), "Failed to create the request pipe");
  std::unique_ptr<const char* const, decltype(pipe_file_del)> req_pipe_file_raii{&req_pipe_filename, pipe_file_del};

  // create the response pipe
  HANDLE_SYS_ERR(mkfifo(resp_pipe_filename, 0666), "Failed to create the response pipe");
  std::unique_ptr<const char* const, decltype(pipe_file_del)> resp_pipe_file_raii{&resp_pipe_filename, pipe_file_del};

  // run Program 2 in a child process
  const auto child_pid = fork();
  HANDLE_SYS_ERR(child_pid, "Failed to create a child process");
  if (child_pid == 0) {
    static constexpr auto program2_filename = "./program2_exec";
    HANDLE_SYS_ERR(execlp(program2_filename, program2_filename, req_pipe_filename, resp_pipe_filename, (char*)NULL),
      "Failed to execute a child program");
  }

  // close pipe file descriptor upon exit
  const auto pipe_fd_del = [](auto* p) { if (close(*p) < 0) { perror("Failed to close pipe"); } };

  // open the request pipe
  const int req_pipe_fd = open(req_pipe_filename, O_CREAT | O_WRONLY);
  HANDLE_SYS_ERR(req_pipe_fd, "Failed to open the request pipe");
  std::unique_ptr<const int, decltype(pipe_fd_del)> req_pipe_fd_raii{&req_pipe_fd, pipe_fd_del};

  // open the response pipe
  const int resp_pipe_fd = open(resp_pipe_filename, O_CREAT | O_RDONLY);
  HANDLE_SYS_ERR(resp_pipe_fd, "Failed to open the response pipe");
  std::unique_ptr<const int, decltype(pipe_fd_del)> resp_pipe_fd_raii{&resp_pipe_fd, pipe_fd_del};

  std::ofstream log_file{"log.txt"};

  while (!m_exit_flag) {
    static std::array<char, 1024> input_line;

    if (input_file.is_open()) {
      input_file.getline(input_line.data(), input_line.size());
      if (!input_file.good()) { break; }
    } else {
      std::cin.getline(input_line.data(), input_line.size());
      if (std::cin.eof() || std::cin.fail()) { return {}; }
    }

    const auto msg = cmd_parser::parse_cmd_line(input_line.data());
    if (!msg) {
      std::cerr << "Error: invalid command\n";
      continue;
    }

    {
      static uint8_t write_buf[256];

      // write message length (first byte of data)
      write_buf[0] = msg->length();
      const auto msg_size = msg->serialize(&write_buf[1], sizeof(write_buf) - 1);
      // write message payload
      const int rc = write(req_pipe_fd, write_buf, msg_size + 1);
      HANDLE_SYS_ERR(rc, "Failed to write to the request pipe");
    }
    
    {
      // read response length (first byte of data)
      uint8_t resp_len;
      const int rc = read(resp_pipe_fd, &resp_len, 1);
      if (errno == EINTR) { return EXIT_SUCCESS; }
      HANDLE_SYS_ERR(rc, "Failed to read response length");
      if (rc == 0) {
        // end of communication
        break;
      }

      // NOTE: response of length '0' indicates that the last command was successful

      if (resp_len > 0) {
        static char resp_buf[256];

        // read response payload
        const int rc = read(resp_pipe_fd, resp_buf, resp_len);
        if (errno == EINTR) { return EXIT_SUCCESS; }
        HANDLE_SYS_ERR(rc, "Failed to read from response pipe");
        if (rc == 0) {
          // end of communication
          break;
        }

        std::string resp_str{(const char*)resp_buf, resp_len};
        log_file << resp_str << "\n";
      }
    }
  }

  HANDLE_SYS_ERR(close(req_pipe_fd), "Failed to close the request pipe");
  req_pipe_fd_raii.release();

  int child_status;
  HANDLE_SYS_ERR(waitpid(child_pid, &child_status, 0), "Failed while waiting for child to terminate");

  return EXIT_SUCCESS;
}
