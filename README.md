# HDPS Core

# CI-CD

Linux, Macos, Windows - GithubActions

![Build Status](https://github.com/hdps/core/actions/workflows/build.yml/badge.svg)


Pushing to the hdps/core develop wil trigger the conan CI_CD to start a build via the configured Webhook.

Currently the following build matrix is performed

OS | Architecture | Compiler
--- | --- | ---
Windows | x64 | MSVC 2017 
Linux | x86_64 | gcc 9
Macos | x86_64 | clang 10

On success the binary and include package is available at [hdps-core in the LKEB Artifactory database](http://cytosplore.lumc.nl:8081/artifactory/webapp/#/artifacts/browse/tree/General/conan-local/bvanlew/hdps-core)

# Plugin State Table
See the current state of available plugins at: https://github.com/hdps/PublicWiki/wiki#plugin-state-table

# Installation & Building
See https://github.com/hdps/core/wiki/Installation
