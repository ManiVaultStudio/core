# ManiVault Core

## Installation & Building
See https://github.com/ManiVaultStudio/core/wiki/Installation

## Plugin State Table
See the current state of available plugins at: https://github.com/ManiVaultStudio/PublicWiki/wiki#plugin-state-table

## CI-CD

Linux, Macos, Windows - GithubActions

![Build Status](https://github.com/ManiVaultStudio/core/actions/workflows/build.yml/badge.svg)


Pushing to the ManiVaultStudio/core develop will trigger the Conan CI_CD to start a build via the configured Webhook.

Currently the following build matrix is performed

OS | Architecture | Compiler
--- | --- | ---
Windows | x64 | MSVC 2019 
Macos | x86_64 | clang 12
<!--- Linux | x86_64 | gcc 9 -->

On success the binary and include package is available at [hdps-core in the LKEB Artifactory database](http://cytosplore.lumc.nl:8081/artifactory/webapp/#/artifacts/browse/tree/General/conan-local/bvanlew/hdps-core)

## License

ManiVault is licensed under LGPL v3.0, refer to the [LICENSE file](https://github.com/ManiVaultStudio/core/blob/main/LICENSE) in the top level directory.

Copyright © 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)