# Change Log

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog]() and this project adheres to
[Semantic Versioning]().

## [Unreleased]

## [2017-03-14] 0.0.6

### Changed

- Refactored "insert_sort_partial()" and "binary_insert_sort()". Functions have
  been made more concise, have been changed to use only size_t for indexing,
  and in the case of "binary_insert_sort()", have been made significantly more
  efficient. 

## [2017-03-13] 0.0.5

### Added

- Project documentation in both HTML and LaTeX. Documentation is generated
  using Doxygen. 
- Basic README. README will also provide content for the Doxygen-generated 
  index.html.

### Changed

- Documentation comments to make them Doxygen compliant.

## [2017-03-10] 0.0.4

### Added

- Galloping mode to Timsort. Galloping mode affects how runs are merged and is 
  implemented in such a way that it adds minimal overhead when the data doesn't
  support it (e.g. when data is random) but offers a significant performance 
  boost when it does.

## [2017-03-02] 0.0.3

### Added

- Working (but unoptimized) Timsort for full sorting. Current implementation
  does not include "galloping mode", so all merges are performed like those in 
  standard merge sort.

## [2017-02-26] 0.0.2

### Added

- Comb sort for full sorting.

## [2017-02-25] 0.0.1

### Added

- Defaut directory layout and files (INSTALL, MAKEFILE, README, BUGS, TODO).
- Add MinUnit test framework and basic spec file.
- Insertion sort for both full and partial sorting.
- Selection sort for full sorting.
- Merge sort for full sorting.
- Quicksort for full sorting.
