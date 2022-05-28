#include <boost/ut.hpp>
#include <pqrs/osx/codesign.hpp>

int main(void) {
  using namespace boost::ut;
  using namespace boost::ut::literals;

  "find_common_name_of_process"_test = [] {
    expect(pqrs::osx::codesign::find_common_name_of_process(1) != std::nullopt);
  };

  return 0;
}
