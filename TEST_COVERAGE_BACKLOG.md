# Test Coverage Backlog

## Candidates

## In Progress

## Completed

- [x] `ManiVault/src/private/GifRecorder.cpp`: add deterministic application-level lifecycle coverage
  - Completed: 2026-10-09
  - Scope: `application-specific`
  - User impact: `low`
  - Fan-out: `narrow`
  - Tests: `cmake -S D:\DevBundle\core_testing\source -B D:\DevBundle\core_testing\build -DMV_BUILD_TESTING=ON`; `cmake --build D:\DevBundle\core_testing\build\core --config Debug --target ManiVault/tests/application/MV_Application_GifRecorderTest`; `ctest --test-dir D:\DevBundle\core_testing\build\core -C Debug -R '^MV_Application_GifRecorderTest$' --output-on-failure` (passed: 1/1)
  - Coverage: not measured
  - Notes: added an injectable frame provider and configurable duration while preserving the production constructor defaults; no display server or real window is required

- [x] `ManiVault/src/util/BlobCodec.cpp`: test public blob codec type serialization and case-insensitive parsing
  - Completed: 2026-10-09
  - Scope: `public-core`
  - User impact: `high`
  - Fan-out: `broad`
  - Tests: `cmake -S D:\DevBundle\core_testing\source -B D:\DevBundle\core_testing\build -DMV_BUILD_TESTING=ON`; `cmake --build D:\DevBundle\core_testing\build\core --config Debug --target ManiVault/tests/public/MV_Public_BlobCodecTest`; `ctest --test-dir D:\DevBundle\core_testing\build\core -C Debug -R '^(MV_Public_BlobCodecTest|MV_Public_ParseByteSizeTest|MV_Public_ColorSchemeTest)$' --output-on-failure` (passed: 3/3)
  - Coverage: not measured
  - Notes: covered all valid codec mappings and case-insensitive parsing without changing production code

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

- `ManiVault/src/private/GifRecorder.cpp`: inject startup, capture-provider and encoder failures independently
  - Deferred because the current recorder owns native window capture and asynchronous encoder execution directly; adding failure injection would require a broader abstraction than this time-box allows.
  - Follow-up: consider an internal encoder/finalization seam if these failure paths become a priority.

- `ManiVault/src/util/BlobCodec.cpp`: test rejection of unknown codec strings
  - Deferred because constructing `ManiVaultException` requires initialized global error-manager state that is unavailable in the standalone public test executable.
  - Follow-up: add the test through an application-initialized target or introduce a narrowly scoped test fixture for the error manager.
