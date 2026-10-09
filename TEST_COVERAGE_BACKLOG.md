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

- [x] `ManiVault/src/util/Miscellaneous.cpp`: test public `getNoBytesHumanReadable()` IEC/SI formatting
  - Completed: 2026-10-08
  - Tests: `ctest --test-dir D:\DevBundle\core_testing\build\core -C Debug -R '^MV_Public_ParseByteSizeTest$' --output-on-failure` (passed)
  - Verification: 1/1 CTest tests passed
  - Coverage: not measured
  - Notes: covered IEC/SI unit boundaries in a utility with broad reporting fan-out without changing production code

- [x] `ManiVault/src/util/ColorScheme.h`: test public property construction and setter/getter round-trips
  - Completed: 2026-10-09
  - Scope: `public-core`
  - User impact: `medium`
  - Fan-out: `broad`
  - Tests: `cmake --build D:\DevBundle\core_testing\build\core --config Debug --target ManiVault/tests/public/MV_Public_ParseByteSizeTest`; `ctest --test-dir D:\DevBundle\core_testing\build\core -C Debug -R '^(MV_Public_ParseByteSizeTest|MV_Application_GifEncoderTest)$' --output-on-failure` (passed: 2/2)
  - Coverage: not measured
  - Notes: exercises observable ColorScheme state without changing production code

## Deferred
