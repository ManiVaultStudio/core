---
name: format-changes
description: Reformat changed C++ files in this repository to match the local ManiVault core style, especially one-line signatures, compact calls, aligned local groups, readable whitespace, indented switch cases, and braces around multi-line loops.
metadata:
  short-description: Format changed C++ files using the repository style
---

# Format Changes

Use this skill when the user asks to format, reformat, clean up, or apply the
repository style to current changes. It is intended for focused formatting of
changed files, not broad reformatting of the repository.

## Source of truth

Read `AGENTS.md` and `CODING_STYLE.md` at the repository root before editing.
Use nearby unchanged core files as the final reference when the written rules
do not answer a local layout question.

## Workflow

1. Inspect the current branch and worktree with `git status --short --branch`.
2. Identify the files and lines in scope from the user's request and the Git
   diff. Preserve unrelated user changes.
3. Read the relevant headers, sources, tests, and nearby core examples before
   changing layout.
4. Reformat only the requested or already changed code. Do not run a broad
   formatter, add an AStyle configuration, or create unrelated formatting
   churn.
5. Review the resulting diff and run `git diff --check`. Build or test the
   narrowest affected target when practical.

## Required local style

- Keep function declarations and definitions on one line, including long
  signatures.
- Keep ordinary calls on one line when they remain understandable. For a call
  with a lambda, keep the ordinary arguments and lambda introducer on the
  opening line; indent the lambda body normally.
- Align related declarations or assignments locally. Do not force alignment
  across unrelated statements or logical sections.
- Use empty lines to separate setup, validation, processing, and reporting.
  Leave an empty line after an early exception or validation guard before the
  main loop or operation.
- Do not add empty lines between every statement in a short cohesive group.
- Use braces for every multi-line `for` body, including an outer loop that
  contains a nested loop or another multi-line body.
- Indent `case` and `default` labels inside `switch` statements and separate
  non-trivial cases with empty lines.
- Keep four-space indentation and match the opening-brace layout of nearby
  core files.
- Keep action construction compact and readable; do not split every argument
  onto its own line.
- Preserve public-header boundaries and forward declarations. Do not introduce
  implementation-only includes into public headers merely to make formatting
  easier.

## Manual review checklist

Before handing off the change, inspect the changed lines for:

- one-line signatures;
- compact, understandable calls and lambda openings;
- aligned related assignments;
- useful empty lines between logical sections;
- braces around multi-line and nested `for` loops;
- indented and separated switch cases;
- no unrelated formatting churn;
- a clean `git diff --check` result.

Formatting is a manual, context-sensitive review in this repository. Do not
claim that the entire branch is formatted unless all relevant changed files
were actually inspected.
