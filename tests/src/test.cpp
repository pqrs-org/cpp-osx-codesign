#include <boost/ut.hpp>
#include <pqrs/osx/codesign.hpp>

int main() {
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
    expect("macOS Software Signing"sv == *actual);
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
      expect("macOS Software Signing"sv == actual);
    }
  };

  "make_signing_requirement"_test = [] {
    using namespace pqrs::osx::codesign;
    expect(make_signing_requirement({}).error() == signing_requirement_error::missing_anchor);
    expect(make_signing_requirement({
               .anchor = anchor_type::apple,
           }) == "anchor apple");

    auto compile = [](const signing_requirement_parameters& parameters) {
      auto text = make_signing_requirement(parameters);
      auto string = pqrs::cf::make_cf_string(*text);

      SecRequirementRef raw = nullptr;
      auto status = SecRequirementCreateWithString(*string,
                                                   kSecCSDefaultFlags,
                                                   &raw);

      auto requirement = pqrs::cf::adopt_cf_ptr(raw);
      expect(status == errSecSuccess) << *text << status;

      return requirement;
    };

    auto code = get_code_of_file("data/apps/Applications/valid-signature.app");
    auto requirement = compile({
        .anchor = anchor_type::apple_generic,
        .team_id = team_id("G43BCU2T37"),
        .identifier = "org.pqrs.cpp-osx-codesign.valid-signature",
    });
    expect(verify_code(*code, *requirement) == errSecSuccess);
    expect(verify_code(*get_code_of_file("data/apps/Applications/spoofed-identifier.app"), *requirement) != errSecSuccess);
    expect(verify_code(*get_code_of_file("data/apps/Applications/unsigned.app"), *requirement) != errSecSuccess);
    expect(verify_code(*code, *compile({
                                  .anchor = anchor_type::apple_generic,
                                  .team_id = team_id("G43BCU2T37"),
                              })) == errSecSuccess);
    expect(verify_code(*code, *compile({
                                  .anchor = anchor_type::apple_generic,
                                  .team_id = team_id("G43BCU2T37"),
                                  .identifier = "org.pqrs.cpp-osx-codesign.valid-signature",
                              })) == errSecSuccess);
    expect(verify_code(*code, *compile({
                                  .anchor = anchor_type::apple,
                                  .team_id = team_id("G43BCU2T37"),
                              })) != errSecSuccess);
    expect(verify_code(*code, *compile({
                                  .anchor = anchor_type::apple_generic,
                                  .team_id = team_id("AAAAAAAAAA"),
                              })) != errSecSuccess);
    expect(verify_code(*code, *compile({
                                  .anchor = anchor_type::apple_generic,
                                  .team_id = team_id("G43BCU2T37"),
                                  .identifier = "org.pqrs.dummy",
                              })) != errSecSuccess);

    auto invalid = [](const signing_requirement_parameters& parameters) {
      auto result = make_signing_requirement(parameters);
      expect(!result);
      if (!result) {
        expect(result.error() != signing_requirement_error::missing_anchor);
      }
    };
    invalid({.anchor = anchor_type::none});
    invalid({.anchor = static_cast<anchor_type>(999)});
    invalid({.anchor = anchor_type::apple_generic, .team_id = team_id("")});
    invalid({.anchor = anchor_type::apple_generic, .identifier = ""});
    invalid({.anchor = anchor_type::apple_generic, .identifier = "a\\b"});
    invalid({.anchor = anchor_type::apple_generic, .identifier = std::string("a\0b", 3)});
    invalid({.anchor = anchor_type::apple_generic, .identifier = "a\nb"});
    invalid({.anchor = anchor_type::apple_generic, .certificate_common_name = ""});
    invalid({.anchor = anchor_type::apple_generic, .leaf_certificate_sha1 = "abc"});
    invalid({.anchor = anchor_type::apple_generic, .cdhash = "0123456789abcdef0123456789abcdef012345678g"});
  };

  return 0;
}
