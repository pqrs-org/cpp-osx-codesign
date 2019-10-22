[![Build Status](https://github.com/pqrs-org/cpp-osx-codesign/workflows/CI/badge.svg)](https://github.com/pqrs-org/cpp-osx-codesign/actions)
[![License](https://img.shields.io/badge/license-Boost%20Software%20License-blue.svg)](https://github.com/pqrs-org/cpp-osx-codesign/blob/master/LICENSE.md)

# cpp-osx-codesign

A utility of `SecStaticCodeRef`.

## Requirements

cpp-osx-codesign depends the following classes.

- [pqrs::cf::array](https://github.com/pqrs-org/cpp-cf-array).
- [pqrs::cf::dictionary](https://github.com/pqrs-org/cpp-cf-dictionary).
- [pqrs::cf::number](https://github.com/pqrs-org/cpp-cf-number).
- [pqrs::cf::string](https://github.com/pqrs-org/cpp-cf-string).

## Install

### Using package manager

You can install `include/pqrs` by using [cget](https://github.com/pfultz2/cget).

```shell
cget install pqrs-org/cget-recipes
cget install pqrs-org/cpp-osx-codesign
```

### Manual install

Copy `include/pqrs` directory into your include directory.
