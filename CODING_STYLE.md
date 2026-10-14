# ManiVault C++ Style Guidelines

This document describes the C++ formatting conventions used in ManiVault. The goal is readable, compact code that follows the style already used throughout the core. These rules are intended as review guidance; they are not currently represented by a repository-wide formatter configuration.

## General principles

- Prefer the existing style of the surrounding file over introducing a new local convention.
- Keep related statements visually grouped and separate logical steps with empty lines.
- Keep straightforward code compact. Use additional line breaks when they materially improve readability, not merely because a line is long.
- Preserve meaningful alignment in groups of related declarations or assignments.
- Do not introduce a formatting tool configuration solely to encode these rules unless the repository adopts that tool explicitly.

## Function signatures

Keep function signatures on one line, including long signatures. This applies to declarations and definitions.

```cpp
std::shared_ptr<PointDataTiledStorage> fromGenerator(std::uint64_t rows, std::uint64_t columns, std::size_t elementSize, std::size_t rowTileSize, std::size_t columnTileSize, TileGenerator generator);
```

Do not split the parameter list over multiple lines just to reduce line length.

## Function calls

Keep ordinary function calls on one line when they remain understandable.

```cpp
const auto storage = PointDataTiledStorage::fromDense(reinterpret_cast<const char*>(values.data()), 5, 4, sizeof(float), 2, 3);
```

For calls that take a lambda, keep the ordinary arguments and the lambda introducer on the opening line. Indent the lambda body normally.

```cpp
auto storage = std::make_shared<PointDataTiledStorage>(rows, columns, elementSize, rowTileSize, columnTileSize, [](const TileDescriptor& descriptor) {
    return mv::util::bytesFromBlobVariantMap(descriptor.blob);
});
```

Do not put every argument on a separate line when the call can be expressed clearly on one line. A genuinely complex nested expression may still be formatted over multiple lines, but the call should remain easy to scan.

## Lambdas and callbacks

Use the compact opening form for lambdas:

```cpp
connect(_dataSynthesisMenu, &QMenu::aboutToShow, this, [this] {
    if (_dataSynthesisPopulated)
        return;

    developer::populateMenu(*_dataSynthesisMenu, parentWidget());
    _dataSynthesisPopulated = true;
});
```

Keep short callbacks concise. Use an empty line inside a longer callback to separate logical operations.

## Assignments and declarations

Align related assignments and declarations when doing so makes the relationship clearer.

```cpp
const auto totalTiles       = tileRowCount * tileColumnCount;
const auto progressInterval = std::max<std::size_t>(1, totalTiles / 100);
std::size_t tileIndex       = 0;
```

Alignment is local to a related group. Do not add artificial alignment across unrelated code.

## Braces and control flow

Use braces for every `for` loop whose body spans multiple lines. This includes outer loops that contain another loop or another multi-line statement.

```cpp
for (std::uint64_t tileRow = 0; tileRow < tileRowCount; ++tileRow)
{
    for (std::uint64_t tileColumn = 0; tileColumn < tileColumnCount; ++tileColumn)
    {
        processTile(tileRow, tileColumn);
    }
}
```

Single-statement loops may remain unbraced when that matches the surrounding core code, but adding braces is preferred when the loop is likely to grow or when omission would make nesting ambiguous.

When an early validation or exception block is followed by a loop, leave an empty line between them.

```cpp
if (rowTileSize == 0 || columnTileSize == 0)
    throw std::invalid_argument("Invalid tiled point-data geometry");

for (std::uint64_t row = 0; row < rows; row += rowTileSize)
{
    processRowTile(row);
}
```

## Switch statements

Indent `case` labels inside the switch and separate non-trivial cases with an empty line.

```cpp
switch (typeIndex)
{
    case 0:
        return *reinterpret_cast<const float*>(bytes);

    case 1:
        return static_cast<float>(*reinterpret_cast<const biovault::bfloat16_t*>(bytes));

    case 2:
        return static_cast<float>(*reinterpret_cast<const std::int32_t*>(bytes));

    default:
        return 0.0f;
}
```

## Empty lines and grouping

Use empty lines to make logical structure visible:

- between setup, validation, processing, and return sections;
- between cases in a switch when the cases contain more than one simple statement;
- after an early exception or return before the main operation begins;
- inside longer callbacks between independent operations.

Avoid empty lines that split a short, cohesive statement sequence.

## Review checklist

Before submitting a change, check that:

- function signatures are kept on one line;
- straightforward calls are kept on one line;
- lambda calls keep the arguments and lambda introducer on the opening line;
- multi-line `for` bodies have braces, including outer nested loops;
- switch cases are indented and visually separated;
- related assignments are aligned where useful;
- validation and exception blocks are separated from following loops;
- the formatting matches nearby core code;
- no unrelated formatter configuration or broad reformatting was introduced.
