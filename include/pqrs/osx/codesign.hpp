#pragma once

// pqrs::osx::codesign v1.0

// (C) Copyright Takayama Fumihiko 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)

#include <Security/CodeSigning.h>
#include <optional>
#include <pqrs/cf/array.hpp>
#include <pqrs/cf/dictionary.hpp>
#include <pqrs/cf/number.hpp>
#include <pqrs/cf/string.hpp>
#include <string>

namespace pqrs {
namespace osx {
namespace codesign {
inline std::optional<std::string> find_common_name_of_process(pid_t pid) {
  std::optional<std::string> common_name;

  if (auto attributes = cf::make_cf_mutable_dictionary()) {
    if (auto pid_number = cf::make_cf_number(static_cast<int64_t>(pid))) {
      CFDictionarySetValue(*attributes, kSecGuestAttributePid, *pid_number);

      SecCodeRef guest;
      if (SecCodeCopyGuestWithAttributes(nullptr, *attributes, kSecCSDefaultFlags, &guest) == errSecSuccess) {
        CFDictionaryRef information;
        if (SecCodeCopySigningInformation(guest, kSecCSSigningInformation, &information) == errSecSuccess) {
          if (auto certificates = static_cast<CFArrayRef>(CFDictionaryGetValue(information, kSecCodeInfoCertificates))) {
            if (CFArrayGetCount(certificates) > 0) {
              auto certificate = cf::get_cf_array_value<SecCertificateRef>(certificates, 0);
              CFStringRef common_name_string;
              if (SecCertificateCopyCommonName(certificate, &common_name_string) == errSecSuccess) {
                common_name = cf::make_string(common_name_string);
                CFRelease(common_name_string);
              }
            }
          }

          CFRelease(information);
        }

        CFRelease(guest);
      }
    }
  }

  return common_name;
}
} // namespace codesign
} // namespace osx
} // namespace pqrs
