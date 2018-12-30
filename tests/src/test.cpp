#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <pqrs/osx/codesign.hpp>

TEST_CASE("find_common_name_of_process") {
  REQUIRE(pqrs::osx::codesign::find_common_name_of_process(1));
}
