# Test Coverage Backlog

## Candidates

## In Progress

## Completed

- [x] `ManiVault/tests/application/GifEncoderTest.cpp`: cover invalid frame format and duration, and verify reuse after an empty finish
  - Completed: 2026-10-07
  - Tests: `ctest --test-dir D:\DevBundle\core_testing\build\core -C Debug -R '^MV_Application_GifEncoderTest$' --output-on-failure` (passed)
  - Verification: 1/1 CTest tests passed
  - Coverage: not measured
  - Notes: targeted the existing GIF encoder test without changing production code

- [x] `ManiVault/tests/application/GifEncoderTest.cpp`: cover oversized dimensions and duplicate `begin()` calls
  - Completed: 2026-10-07
  - Tests: `ctest --test-dir D:\DevBundle\core_testing\build\core -C Debug -R '^MV_Application_GifEncoderTest$' --output-on-failure` (passed)
  - Verification: 1/1 CTest tests passed
  - Coverage: not measured
  - Notes: exercised invalid session-start and already-active-session branches without changing production code

- [x] `ManiVault/src/util/Miscellaneous.cpp`: test public `parseByteSize()` normalization and invalid-input behavior
  - Completed: 2026-10-07
  - Tests: `ctest --test-dir D:\DevBundle\core_testing\build\core -C Debug -R '^MV_Public_ParseByteSizeTest$' --output-on-failure` (passed)
  - Verification: 1/1 CTest tests passed
  - Coverage: not measured
  - Notes: added the first public-core test target; covered normalized/fractional sizes and malformed input without changing production code

- [x] `ManiVault/src/LinkedData.cpp`: cover public `SelectionMap` indexed and image-pyramid mapping behavior
  - Completed: 2026-10-13
  - Tests: `ctest --test-dir D:\DevBundle\core_testing\build\core -C Debug -R '^MV_Public_SelectionMapTest$' --output-on-failure` (passed)
  - Verification: 1/1 CTest tests passed; `git diff --check` passed
  - Coverage: not measured
  - Notes: added a dedicated public test executable covering indexed lookup, missing mappings, image-pyramid expansion, and image-boundary behavior without changing production code

## Deferred
