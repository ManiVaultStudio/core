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

## Deferred
