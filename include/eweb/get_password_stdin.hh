#pragma once

#include <iostream>
#include <string>

#ifdef WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

namespace eweb {

// From https://stackoverflow.com/questions/1413445/reading-a-password-from-stdcin
inline void set_stdin_echo(bool enable) {
#ifdef WIN32
  HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
  DWORD mode;
  GetConsoleMode(hStdin, &mode);
  if(enable) {
    mode |= ENABLE_ECHO_INPUT;
  } else {
    mode &= ~ENABLE_ECHO_INPUT;
  }

  SetConsoleMode(hStdin, mode);

#else
  struct termios tty;
  tcgetattr(STDIN_FILENO, &tty);
  if(enable) {
    tty.c_lflag |= ECHO;
  } else {
    tty.c_lflag &= ~ECHO;
  }

  (void) tcsetattr(STDIN_FILENO, TCSANOW, &tty);

#endif
}

inline std::string get_password_stdin() {
  std::cout << "Password: " << std::flush;

  set_stdin_echo(false);

  std::string output;
  std::cin >> output;

  set_stdin_echo(true);

  std::cout << std::endl;

  return output;
}

}
