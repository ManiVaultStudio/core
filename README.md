# core

# CI-CD

Pushing to the hdps/core develop wil trigger the conan CI_CD to start a build via the configured Webhook.

Currently the following build marix is performed

OS | Architecture | Compiler
--- | --- | ---
Windows | x64 | MSVC 2017 

On success the binary and include package is available at [hdps-core in the LKEB Artifactory database](http://cytosplore.lumc.nl:8081/artifactory/webapp/#/artifacts/browse/tree/General/conan-local/bvanlew/hdps-core)
