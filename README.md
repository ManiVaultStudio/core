# core

# CI-CD

Linux & Macos (Travis) | Windows (Appveyor)
--- | ---
[![Build Status](https://travis-ci.com/bldrvnlw/conan-hdps-core.svg?branch=master)](https://travis-ci.com/bldrvnlw/conan-hdps-core) | [![Build status](https://ci.appveyor.com/api/projects/status/wmef9pb28ma6gv79?svg=true)](https://ci.appveyor.com/project/bldrvnlw/conan-hdps-core)



Pushing to the hdps/core develop wil trigger the conan CI_CD to start a build via the configured Webhook.

Currently the following build marix is performed

OS | Architecture | Compiler
--- | --- | ---
Windows | x64 | MSVC 2017 
Linux | x86_64 | gcc 9
Macos | x86_64 | clang 10

On success the binary and include package is available at [hdps-core in the LKEB Artifactory database](http://cytosplore.lumc.nl:8081/artifactory/webapp/#/artifacts/browse/tree/General/conan-local/bvanlew/hdps-core)
