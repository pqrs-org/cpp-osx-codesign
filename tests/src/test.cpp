#include <boost/ut.hpp>
#include <pqrs/osx/codesign.hpp>

int main(void) {
  using namespace boost::ut;
  using namespace boost::ut::literals;
  using namespace std::literals;

  "get_signing_information_of_process"_test = [] {
    auto actual = pqrs::osx::codesign::get_signing_information_of_process(1);
    expect(std::nullopt == actual.get_verified_team_id());
    expect("com.apple.xpc.launchd"sv == *(actual.get_identifier()));
  };

  "get_signing_information_of_file"_test = [] {
    {
      auto actual = pqrs::osx::codesign::get_signing_information_of_file("data/apps/Applications/spoofed-identifier.app");
      expect(pqrs::osx::codesign::anchor_type::apple_generic == actual.get_verified_anchor_type());
      expect(pqrs::osx::codesign::team_id("G43BCU2T37") == actual.get_verified_team_id());
      expect("com.apple.dummy"sv == actual.get_identifier());
    }
    {
      auto actual = pqrs::osx::codesign::get_signing_information_of_file("data/apps/Applications/unsigned.app");
      expect(pqrs::osx::codesign::anchor_type::none == actual.get_verified_anchor_type());
      expect(std::nullopt == actual.get_verified_team_id());
      expect(std::nullopt == actual.get_identifier());
    }
    {
      auto actual = pqrs::osx::codesign::get_signing_information_of_file("data/apps/Applications/valid-signature.app");
      expect(pqrs::osx::codesign::anchor_type::apple_generic == actual.get_verified_anchor_type());
      expect(pqrs::osx::codesign::team_id("G43BCU2T37") == actual.get_verified_team_id());
      expect("org.pqrs.cpp-osx-codesign.valid-signature"sv == actual.get_identifier());
    }
    {
      auto actual = pqrs::osx::codesign::get_signing_information_of_file("/System/Applications/Utilities/Terminal.app");
      expect(pqrs::osx::codesign::anchor_type::apple == actual.get_verified_anchor_type());
      expect(std::nullopt == actual.get_verified_team_id());
      expect("com.apple.Terminal"sv == actual.get_identifier());
    }
  };

  "find_common_name_of_process"_test = [] {
    auto actual = pqrs::osx::codesign::find_common_name_of_process(1);
    expect("Software Signing"sv == *actual);
  };

  "find_common_name_of_file"_test = [] {
    {
      auto actual = pqrs::osx::codesign::find_common_name_of_file("data/apps/Applications/spoofed-identifier.app");
      expect("Developer ID Application: Fumihiko Takayama (G43BCU2T37)"sv == actual);
    }
    {
      auto actual = pqrs::osx::codesign::find_common_name_of_file("data/apps/Applications/unsigned.app");
      expect(std::nullopt == actual);
    }
    {
      auto actual = pqrs::osx::codesign::find_common_name_of_file("data/apps/Applications/valid-signature.app");
      expect("Developer ID Application: Fumihiko Takayama (G43BCU2T37)"sv == actual);
    }
    {
      auto actual = pqrs::osx::codesign::find_common_name_of_file("/System/Applications/Utilities/Terminal.app");
      expect("Software Signing"sv == actual);
    }
  };

  return 0;
}
