# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- the makefiles build the C++20 module `smspp.multiknapsackassignblock`
  when `SMSppMOD` is not empty (e.g., `make SMSppMOD=1`), with clang++ 16
  or later [see `lib/makefile-mod` of SMS++]

- the C++20 module `smspp.multiknapsackassignblock`
  (`src/MultiKnapsackAssignBlock.cppm`), which exports every name that the
  headers declare at namespace scope and re-exports
  `smspp.binaryknapsackblock`, so that a source file can write `import
  smspp.multiknapsackassignblock;` in place of the `#include` of the
  headers; it is built with the CMake option `SMSpp_BUILD_MODULE`, as the
  target `SMS++::MultiKnapsackAssignBlock_module`

### Changed

- the pipeline builds the module also with its makefiles, after CMake, and
  runs on a change of the makefiles too [see `ci/makefile` of the umbrella]

- `SMSpp_BUILD_MODULE` no longer stops the configuration when the C++20
  module cannot be built, i.e., with a CMake older than 3.28, a generator
  other than Ninja or Visual Studio, a compiler other than Clang 16 (with
  `clang-scan-deps`) or MSVC 19.34, or a module it imports that is not
  there: the option is turned off with a warning that says why, and the
  build goes on with the headers [see `cmake/CheckModule.cmake` of SMS++]

- the instances of `data/` are downloaded and extracted by the targets
  `download_mkab_<fmt>` and `extract_mkab_<fmt>`, written as in every
  module that keeps its instances in the Package Registry, and the marker
  of the extraction carries the format in its name, so that a tree
  extracted before extracts once more

### Fixed

- the makefiles of the tests and of the tools build also with g++: they
  take the option that stops at the first error and the one of the
  debugging information, which clang++ and g++ spell differently
  (`-ferror-limit=1` and `-glldb` against `-fmax-errors=1` and `-ggdb`),
  from `SMSppSWERR` and `SMSppSWDBG` of `lib/makefile-OS` of SMS++


## [0.1.0] - 2026-10-09

### Added

- the module has the layout, the build files and the CI of ModuleTemplate,
  and builds with CMake as well as with the makefiles, which now include
  BinaryKnapsackBlock

- `get_knapsack()`, the sub-Block of a knapsack and a class, and
  `get_Classes()`

- the instances in `data/txt`, out of the generator `data/generate`, which
  CMake downloads from the GitLab Package Registry (target
  `extract_mkab_txt`)

- the unit test `MultiKnapsackAssignBlock_unit_test`, on the SMS++ core and
  BinaryKnapsackBlock alone

### Fixed

- `generate_abstract_constraints()` generates the Variable of the sub-Block
  before using them, and issues no Modification

- `load()` and `deserialize()` discard the previous instance also when it
  has no items, and `print()` honours its verbosity

- `get_x()` and `get_y()` return the value of the Variable, which need not be
  integer, and find the position of an item among these of its class in
  constant time

- the classes are written to netCDF as unsigned integers, and read from any
  integer or floating point type

[Unreleased]: https://gitlab.com/smspp/multiknapsackassignblock/-/compare/0.1.0...develop
[0.1.0]: https://gitlab.com/smspp/multiknapsackassignblock/-/tags/0.1.0
