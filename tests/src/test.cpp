#include <boost/ut.hpp>
#include <pqrs/osx/codesign.hpp>

int main(void) {
  using namespace boost::ut;
  using namespace boost::ut::literals;
  using namespace std::literals;

  "get_signing_information_of_process"_test = [] {
    auto actual = pqrs::osx::codesign::get_signing_information_of_process(1);
    expect(actual.get_team_id() == std::nullopt);
    expect(actual.get_identifier() != std::nullopt);
    expect("com.apple.xpc.launchd"sv == *(actual.get_identifier()));
  };

  "find_common_name_of_process"_test = [] {
    auto actual = pqrs::osx::codesign::find_common_name_of_process(1);
    expect(actual != std::nullopt);
    expect("Software Signing"sv == *actual);
  };

  return 0;
}
