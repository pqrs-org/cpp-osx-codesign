#include <cerrno>
#include <charconv>
#include <csignal>
#include <iostream>
#include <pqrs/osx/codesign.hpp>

namespace {
bool process_exists(pid_t pid) {
  if (pid <= 0) {
    return false;
  }

  // kill signal 0 will cause error checking to be performed (with no signal being sent).
  if (kill(pid, 0) == 0) {
    return true;
  }

  return errno == EPERM;
}
} // namespace

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
      if (process_exists(i)) {
        pids.push_back(i);
      }
    }
  }

  for (auto&& pid : pids) {
    std::cout << "pid:" << pid << std::endl;

    auto signing_information = pqrs::osx::codesign::get_signing_information_of_process(pid);
    std::cout << "    verified_anchor_type: " << signing_information.get_verified_anchor_type() << std::endl;

    if (auto value = signing_information.get_verified_team_id()) {
      std::cout << "    verified_team_id: " << *value << std::endl;
    }
    if (auto value = signing_information.get_identifier()) {
      std::cout << "    identifier: " << *value << std::endl;
    }

    if (auto name = pqrs::osx::codesign::find_common_name_of_process(pid)) {
      std::cout << "    common_name: " << *name << std::endl;
    }
  }

  return 0;
}
