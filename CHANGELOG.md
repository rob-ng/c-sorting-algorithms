# Change Log

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog]() and this project adheres to
[Semantic Versioning]().

## [Unreleased]
 
## [2017-03-23] 0.1.0

### Changed

- Moved stress tests from main file to spec file.
- Made final (whatever that means) changes to Timsort-related functions.
- Renamed timort_check_invariants() to timsort_maintain_invariants() as latter
  is more descriptive of the function.

## [2017-03-19] 0.0.9

### Changed

- Massively overhauled Timsort-related functions. See git log for detailed list
  of changes.
- Improved documentation structure and updated it to reflect recent changes.

## [2017-03-16] 0.0.8

### Changed

- Refactored merge_sort().
- Renamed merge_sort_sort() to more descriptive merge_sort_recursive().
- Refactored insert_sort() and insert_sort_partial().
- Split binary_insert_sort() into binary_insert_sort() and
  binary_insert_sort_partial().
- Refactored quick_sort().
- Renamed quick_sort_sort() to more descriptive quick_sort_recursive().
- Refactored swap(). New function now behaves differently depending on the size
  of the elements being swapped and is consequently faster for smaller elements.

## [2017-03-15] 0.0.7

### Added

- New test functions and associated macro. The functions, test_sort_bounded()
  and test_sort_no_bounds(), are called using the macro mu_run_test_on_arg()
  with two arguments: function pointer to sort function, name of sort function
  as string. Nearly all of the sort functions can be tested with one of these
  functions.

### Changed

- Refactored select_sort(). Rather than iterate over integers and use those
  values to calculate offsets (e.g. ptr+(int_index * element_size)), indices
  and associated values are now incremented by 'size' (the size of an element
  in the array). By doing this, calculating offsets now takes 1 operation
  rather than 2.
- Refactored comb_sort(). Made a similar change as above with respect to
  indices. In addition, I corrected an issue with indices that was causing the
  sort to be substantially less efficient.

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
