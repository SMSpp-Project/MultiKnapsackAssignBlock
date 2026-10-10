# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Changed

- the instances of `data/` are downloaded and extracted by the targets
  `download_mkab_<fmt>` and `extract_mkab_<fmt>`, written as in every
  module that keeps its instances in the Package Registry, and the marker
  of the extraction carries the format in its name, so that a tree
  extracted before extracts once more

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
