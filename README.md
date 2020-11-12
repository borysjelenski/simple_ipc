# Simple IPC

### Highlights

* Program 2 utilizes Cairo to perform drawing and rendering operations. It is likely that it's already installed on your Linux distribution.
  If CMake is unable to find Cairo, please install Cairo -dev package:
  
  Debian/Ubuntu:
  ```
  sudo apt-get install libcairo2-dev
  ```
  Fedora:
  ```
  sudo yum install cairo-devel
  ```
  openSUSE:
  ```
  zypper install cairo-devel
  ```
* Program 1 and Program 2 communicate with each other using named pipes which are created in the local directory. They use very simple communication protocol:
  first the length byte is sent after which the message payload follows. Program 1 sends binary requests (messages) to Program 2 which are interpreted and executed. In case of errors,
  Program 2 responds to Program 1 with human-readable error messages which are written to `log.txt` file.
* Messages form a simple class hierarchy which enables Program 2 to utilize the Visitor patter in order to perform related graphical operations.
  This approach allows to separate the drawing logic from message classes.

### Building and running

Run the `build.sh` script in order to build both programs. Navigate to the `build` directory and run the `program1_exec` executable.
It can be run with the first argument indicating an input file. A simple input file has been provided: `input.txt`.

### Potential improvements

Due to lack of time, some areas of the code could potentially be improved:
* Handling of low-level pipe communication should be moved to a class. This would allow to simplify the code in both `main()` functions. Also, the RAII idiom
  could be implemented directly without relying on smart pointers (`std::unique_ptr`) with custom deleters.
* Input parsing is very simplistic. It's based on the `std::strtok` function which introduces many limitations. For instance, it doesn't allow to pass a file path containing whitespaces.
* Message classes contain a decent amount of similar code. This could be fixed by using templates.
