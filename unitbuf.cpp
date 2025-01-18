#include <chrono>
#include <iostream>
#include <thread>
/*
unitbuf is a manipulator in C++ used with input/output streams (like std::cout
or std::cerr). It ensures that the stream is flushed after every output
operation, which means the output is written to the console or the destination
buffer immediately without waiting for the buffer to fill up.

Without unitbuf, the numbers will appear all at once after the loop finishes.
With unitbuf, the numbers will appear one by one as they are produced.
In summary, unitbuf is a handy tool for controlling stream flushing,
particularly useful in scenarios requiring immediate feedback or debugging.
*/
int main() {
  // Default behavior (buffered output)
  std::cout << "Buffered: ";
  for (int i = 0; i < 3; ++i) {
    std::cout << i << " ";
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  std::cout << std::endl;

  // Enable immediate flushing
  std::cout << std::unitbuf;
  std::cout << "Unbuffered: ";
  for (int i = 0; i < 3; ++i) {
    std::cout << i << " ";
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  std::cout << std::nounitbuf; // Disable immediate flushing
  std::cout << std::endl;

  return 0;
}
