param(
    [string]$QtRoot = "C:\Qt\6.10.3\wasm_multithread",
    [string]$EmsdkRoot = "D:\DevBundle\emsdk",
    [string]$ScatterplotSource = "D:\DevBundle\revamp_archiving\source\Scatterplot",
    [string]$BuildDir = "D:\DevBundle\revamp_archiving\source\core\build-wasm-scatterplot",
    [string]$InstallDir = "D:\DevBundle\revamp_archiving\source\core\build-wasm-scatterplot\install"
)

$ErrorActionPreference = "Stop"
$sourceDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$qtToolchain = Join-Path $QtRoot "lib\cmake\Qt6\qt.toolchain.cmake"
$emscriptenToolchain = Join-Path $EmsdkRoot "upstream\emscripten\cmake\Modules\Platform\Emscripten.cmake"

if (!(Test-Path $qtToolchain)) { throw "Qt WebAssembly toolchain not found: $qtToolchain" }
if (!(Test-Path $emscriptenToolchain)) { throw "Emscripten toolchain not found: $emscriptenToolchain" }
if (!(Test-Path $ScatterplotSource)) { throw "Scatterplot source not found: $ScatterplotSource" }

$env:EMSDK = $EmsdkRoot
$env:PATH = "$EmsdkRoot;$EmsdkRoot\upstream\emscripten;$env:PATH"

$ninja = Get-Command ninja.exe -ErrorAction SilentlyContinue
if ($ninja) { $ninjaPath = $ninja.Source }
if (!$ninjaPath) {
    $ninjaFile = Get-ChildItem "C:\Qt" -Filter ninja.exe -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($ninjaFile) { $ninjaPath = $ninjaFile.FullName }
}
if (!$ninjaPath -or !(Test-Path $ninjaPath)) { throw "ninja.exe was not found under C:\Qt or PATH." }

# Always re-run configuration so a previously failed/partial cache cannot
# retain an empty or invalid CMAKE_MAKE_PROGRAM value.
cmake -S $sourceDir -B $BuildDir -G Ninja `
    "-DCMAKE_MAKE_PROGRAM=$ninjaPath" `
    "-DCMAKE_TOOLCHAIN_FILE=$qtToolchain" `
    "-DQT_HOST_PATH=C:\Qt\6.10.3\mingw_64" `
    "-DQT_CHAINLOAD_TOOLCHAIN_FILE=$emscriptenToolchain" `
    "-DMV_SCATTERPLOT_SOURCE_DIR=$ScatterplotSource" `
    "-DMV_INSTALL_DIR=$InstallDir"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

cmake --build $BuildDir --parallel
exit $LASTEXITCODE
