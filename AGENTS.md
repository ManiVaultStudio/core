# Repository development instructions

These instructions apply to the ManiVault core repository and supplement
`CODING_STYLE.md`, `.clang-format`, and the style used by nearby core source
files. When the formatter configuration and the established surrounding code
style disagree, preserve the surrounding core style unless the task
explicitly asks for a broad formatting change.

## Before editing

- Inspect the current branch and worktree before making changes.
- Read the relevant header, source, test, and build files before editing.
- Look at nearby core code for the local naming, layout, documentation, and
  action-system conventions.
- Keep changes focused. Do not reformat unrelated files or rewrite a whole
  source file merely to apply a formatter.
- Preserve existing user changes and avoid destructive Git operations.

## C++ layout

- Use the project’s existing SPDX header and copyright header conventions.
- Keep includes grouped and ordered consistently with the surrounding file.
- Use four spaces for indentation and do not use tabs for C++ indentation.
- Keep opening braces on the next line for classes, functions, control-flow
  statements, and initializer blocks when that is how the surrounding core
  file is written.
- Keep short, cohesive statements together. Use empty lines to separate
  setup, validation, processing, and reporting sections.
- Do not add empty lines between every statement in a short, related group.

## Function signatures

Keep function declarations and definitions on one line, including long
signatures. Do not split a parameter list merely to satisfy a preferred line
length.

```cpp
std::shared_ptr<PointDataTiledStorage> fromGenerator(std::uint64_t rows, std::uint64_t columns, std::size_t elementSize, std::size_t rowTileSize, std::size_t columnTileSize, TileGenerator generator);
```

This rule applies to constructors, member functions, free functions, lambdas
with explicit function syntax, and declarations in headers.

## Function calls and lambdas

- Keep ordinary function calls on one line when they remain understandable.
- Do not put every argument on a separate line by default.
- For calls with a lambda, keep the ordinary arguments and the lambda
  introducer on the opening line.
- Indent the lambda body normally and use empty lines to separate logical
  operations inside a longer callback.

```cpp
auto storage = std::make_shared<PointDataTiledStorage>(rows, columns, elementSize, rowTileSize, columnTileSize, [](const TileDescriptor& descriptor) {
    return mv::util::bytesFromBlobVariantMap(descriptor.blob);
});
```

Use a multi-line call only when the expression is genuinely too complex to
read as one line. Do not turn a simple action setup into a vertical list of
arguments.

```cpp
connect(_dataSynthesisMenu, &QMenu::aboutToShow, this, [this] {
    if (_dataSynthesisPopulated)
        return;

    developer::populateMenu(*_dataSynthesisMenu, parentWidget());
    _dataSynthesisPopulated = true;
});
```

## Assignments and declarations

Align related assignments and declarations locally when that improves
scanning. Alignment should be limited to one logical group and should not be
forced across unrelated statements.

```cpp
const auto totalTiles       = tileRowCount * tileColumnCount;
const auto progressInterval = std::max<std::size_t>(1, totalTiles / 100);
std::size_t tileIndex       = 0;
```

The same applies to related member initialization or local state setup. Do not
leave obviously related assignments ragged when a small amount of whitespace
would make their relationship clear.

## Control flow and braces

- Use braces for every `for` loop whose body spans multiple lines.
- An outer loop must also have braces when it contains a nested loop or any
  other multi-line body, even if its apparent body is only the nested loop.
- Braces are preferred for loops that are likely to grow or whose omission
  could make nesting ambiguous.
- A short single-statement loop may remain unbraced when that matches the
  surrounding core code.
- Keep `if`/`else` formatting consistent with nearby code; do not introduce
  braces or omit them inconsistently within one logical block.

```cpp
for (std::uint64_t tileRow = 0; tileRow < tileRowCount; ++tileRow)
{
    for (std::uint64_t tileColumn = 0; tileColumn < tileColumnCount; ++tileColumn)
    {
        processTile(tileRow, tileColumn);
    }
}
```

When an early validation or exception statement is followed by the main loop,
leave an empty line between them.

```cpp
if (rowTileSize == 0 || columnTileSize == 0)
    throw std::invalid_argument("Invalid tiled point-data geometry");

for (std::uint64_t row = 0; row < rows; row += rowTileSize)
{
    processRowTile(row);
}
```

## Switch statements

Indent `case` and `default` labels inside the switch. Separate non-trivial
cases with an empty line. Keep the statement belonging to a case indented one
level beyond the label.

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

Do not use the formatter’s default unindented case-label style for new or
modified code.

## Qt and ManiVault actions

- Use the existing `mv::gui` action classes for user-facing controls instead
  of creating ad-hoc Qt controls when an action equivalent exists.
- Keep action construction compact and readable; do not split every argument
  onto its own line.
- Use `HorizontalGroupAction` and `VerticalGroupAction` for grouped controls
  and use `ForceCollapsedInGroup` when a group belongs in a popup.
- Keep action enabled/read-only state synchronized in one clear update method
  when several controls depend on the same dataset or mode.
- Connect signals using the compact lambda form shown above.
- Prefer the project’s existing progress, notification, task, and selection
  APIs over introducing parallel mechanisms.

## Headers, classes, and documentation

- Keep public APIs documented in headers.
- Class documentation should explain what the class is for, why it exists,
  what problem it solves, its ownership/lifetime model, and important limits
  such as read-only behavior or cache semantics.
- Document non-obvious parameters, return values, lifetime requirements, and
  failure behavior.
- Follow the documentation tone and `@author` format used by the surrounding
  core files.
- Keep storage backends and implementation details out of public headers when
  a forward declaration and a source-level include are sufficient.
- Preserve compatibility when extending serialized or public APIs.

## Data access and memory-sensitive code

- Do not accidentally materialize an entire large dataset in a UI path when a
  range, tile, visitor, or bounded cache operation is available.
- Make bounds and empty-input behavior explicit before accessing point data.
- Keep active-view lifetime guarantees visible in the API and documentation.
- Do not invalidate cached data while a lease or view is active.
- Catch and report expected access/decoding failures at UI boundaries rather
  than allowing them to unwind through Qt event handling.

## Tests and verification

- Add or update focused tests for new behavior, especially boundary cases,
  invalid ranges, empty selections, old-format compatibility, and cache state.
- Build the narrowest affected target first, then run the most relevant tests.
- Run `git diff --check` before handing off a change.
- Report build or test limitations explicitly; do not claim success without
  running the relevant check.

## Formatting tools

Do not add an AStyle configuration or run a broad automatic formatter to encode
these rules. The project’s preferred layout includes deliberate choices that
generic formatter defaults may undo, such as one-line signatures, compact
lambda calls, aligned local assignments, indented switch cases, and braces on
multi-line outer loops. Apply the rules deliberately and preserve the style of
the surrounding core code.

Before handing off code, review the changed lines manually for:

- one-line signatures;
- compact, understandable calls;
- compact lambda openings;
- aligned related assignments;
- braces around multi-line and nested `for` loops;
- blank lines after validation/exception guards;
- indented, separated switch cases;
- consistent action grouping and enabled state;
- no unrelated formatting churn.
