#include <iostream>
#include <pqrs/osx/codesign.hpp>

int main(void) {
  for (pid_t i = 0; i < 1000; ++i) {
    if (auto name = pqrs::osx::codesign::find_common_name_of_process(i)) {
      std::cout << "pid:" << i << " " << *name << std::endl;
    }
  }

  return 0;
}
