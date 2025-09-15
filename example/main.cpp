#include <charconv>
#include <iostream>
#include <pqrs/osx/codesign.hpp>

int main(int argc, char* argv[]) {
  std::vector<pid_t> pids;

  if (argc >= 2) {
    for (int i = 1; i < argc; ++i) {
      std::string_view sv{argv[i]};
      int value{};
      auto [ptr, ec] = std::from_chars(std::begin(sv), std::end(sv), value);
      if (ec == std::errc{} && ptr == std::end(sv)) {
        pids.push_back(value);
      }
    }

  } else {
    for (pid_t i = 0; i < 1000; ++i) {
      pids.push_back(i);
    }
  }

  for (auto&& pid : pids) {
    if (auto name = pqrs::osx::codesign::find_common_name_of_process(pid)) {
      std::cout << "pid:" << pid << " common_name: " << *name << std::endl;
    }

    auto signing_information = pqrs::osx::codesign::get_signing_information_of_process(pid);
    if (auto value = signing_information.team_id) {
      std::cout << "pid:" << pid << " team_id: " << *value << std::endl;
    }
    if (auto value = signing_information.identifier) {
      std::cout << "pid:" << pid << " identifier: " << *value << std::endl;
    }
  }

  return 0;
}
