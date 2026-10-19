param(
    [string] $Rows = "10000",
    [string] $Columns = "128",
    [string] $RowTileSizes = "128,256,512",
    [string] $ColumnTileSizes = "4,8,16",
    [string] $CacheCapacities = "4,16,64",
    [UInt64] $Samples = 100000,
    [UInt64] $Repeats = 3,
    [UInt64] $Warmups = 1,
    [switch] $IncludeEager,
    [string] $Output = ""
)

$ErrorActionPreference = "Stop"
$benchmarkPath = Join-Path $PSScriptRoot "PointDataBenchmark.exe"

function ConvertTo-UInt64List([string] $text) {
    return @($text -split '[,;]' | ForEach-Object {
        [UInt64]::Parse($_.Trim())
    })
}

$rowValues = ConvertTo-UInt64List $Rows
$columnValues = ConvertTo-UInt64List $Columns
$rowTileValues = ConvertTo-UInt64List $RowTileSizes
$columnTileValues = ConvertTo-UInt64List $ColumnTileSizes
$cacheValues = ConvertTo-UInt64List $CacheCapacities

if (-not (Test-Path -LiteralPath $benchmarkPath)) {
    throw "Unable to find PointDataBenchmark.exe next to this script: $benchmarkPath"
}

if ([string]::IsNullOrWhiteSpace($Output)) {
    $timestamp = Get-Date -Format "yyyyMMdd_HHmmss"
    $Output = Join-Path (Get-Location) "pointdata_benchmark_$timestamp.csv"
}

$results = [System.Collections.Generic.List[object]]::new()
$configurationCount = $rowValues.Count * $columnValues.Count * $rowTileValues.Count * $columnTileValues.Count * $cacheValues.Count
$configurationIndex = 0

foreach ($rowCount in $rowValues) {
    foreach ($columnCount in $columnValues) {
        foreach ($rowTileSize in $rowTileValues) {
            foreach ($columnTileSize in $columnTileValues) {
                foreach ($cacheCapacity in $cacheValues) {
                    ++$configurationIndex
                    Write-Output "Configuration $configurationIndex/${configurationCount}: rows=$rowCount columns=$columnCount rowTile=$rowTileSize columnTile=$columnTileSize cache=$cacheCapacity"

                    $arguments = @(
                        "--rows", $rowCount,
                        "--columns", $columnCount,
                        "--row-tile-size", $rowTileSize,
                        "--column-tile-size", $columnTileSize,
                        "--cache-capacity", $cacheCapacity,
                        "--samples", $Samples
                    )

                    if ($IncludeEager) {
                        $arguments += "--include-eager"
                    }

                    for ($warmup = 0; $warmup -lt $Warmups; ++$warmup) {
                        & $benchmarkPath @arguments | Out-Null

                        if ($LASTEXITCODE -ne 0) {
                            throw "Benchmark warm-up failed with exit code $LASTEXITCODE"
                        }
                    }

                    for ($repeat = 1; $repeat -le $Repeats; ++$repeat) {
                        $lines = @(& $benchmarkPath @arguments)

                        if ($LASTEXITCODE -ne 0) {
                            throw "Benchmark run failed with exit code $LASTEXITCODE"
                        }

                        $parsed = $lines | ConvertFrom-Csv -Delimiter "`t"

                        foreach ($result in $parsed) {
                            $results.Add([pscustomobject]@{
                                timestamp          = (Get-Date).ToString("o")
                                repeat             = $repeat
                                rows               = $rowCount
                                columns            = $columnCount
                                row_tile_size      = $rowTileSize
                                column_tile_size   = $columnTileSize
                                cache_capacity     = $cacheCapacity
                                samples            = $Samples
                                backend            = $result.backend
                                access_pattern     = $result.access_pattern
                                operations         = $result.operations
                                milliseconds       = $result.milliseconds
                                checksum           = $result.checksum
                                cache_hits        = $result.cache_hits
                                cache_misses      = $result.cache_misses
                                evictions          = $result.evictions
                            })
                        }
                    }
                }
            }
        }
    }
}

$results | Export-Csv -LiteralPath $Output -NoTypeInformation
Write-Output "Wrote $($results.Count) benchmark results to $Output"
