---
name: document-changes
description: Document changed ManiVault C++ APIs and classes using the surrounding core Doxygen style, with rationale, ownership and lifetime details, limits, and accurate parameter and failure documentation.
metadata:
  short-description: Document changed code in the ManiVault core style
---

# Document Changes

Use this skill when the user asks to document, improve documentation for, or
add rationale to changed C++ classes, APIs, storage backends, plugins, or
tests in this repository.

## Source of truth

Read `AGENTS.md` and `CODING_STYLE.md` at the repository root. Inspect nearby
unchanged core headers and source files before writing documentation. Follow
the surrounding file when it has a more specific convention.

The repository generally uses Doxygen comments with a concise `@brief`, a
short rationale-oriented description, relevant `@param` and `@return` tags,
and an author or maintainer tag. Existing core files commonly use `@author
Thomas Kroes`; preserve the established author format in the file being
edited.

## Documentation workflow

1. Inspect the branch and worktree, then identify the requested files and
   changed APIs.
2. Read the complete relevant class declarations and implementations, plus
   nearby core documentation examples.
3. Document public classes and APIs in headers whenever possible. Keep
   implementation-only details in source files unless they are necessary for
   a caller to use the API correctly.
4. Review the implementation before describing behavior. Document what the
   class is for, why it exists, the problem it solves, ownership and lifetime
   rules, important limits, and compatibility behavior.
5. Audit every data member in each changed class or struct, including private
   state, and document every member with a concise adjacent comment. Describe
   its role even when the name or type seems self-explanatory; add defaults,
   relationships, units, ownership, lifetime, serialization meaning, or
   invariants where relevant. Align trailing comments to one column within
   each related member group. Class-level documentation does not replace
   member-level comments.
6. Add parameter, return-value, lifetime, thread-safety, read-only, caching,
   serialization, and failure details when they are non-obvious or relevant.
7. Keep comments concise enough to remain useful. Do not restate the function
   name, obvious C++ syntax, or implementation line by line.
8. Check the diff for accuracy and run `git diff --check`. Build or test the
   narrowest affected target when documentation edits touch declarations or
   require compiler validation.

## Class documentation

Class documentation should answer these questions in plain language:

- What is this class or component?
- Why does it exist, and what problem does it solve?
- Who owns it, and what objects or resources does it keep alive?
- How is it expected to be used?
- What important limits or modes apply, such as read-only behavior, lazy
  loading, cache eviction, proxy behavior, or serialization compatibility?

For storage and access classes, explicitly describe the data layout, loading
granularity, cache or lease guarantees, mutation policy, and the relationship
to the public facade. For UI classes, explain the user-facing purpose and the
data or workflow it exposes without documenting incidental widget construction.

## API documentation

- Document public APIs in the public header.
- Document every struct/class data member in the header with a concise
  adjacent Doxygen comment or inline documentation. Explain what it represents
  even when that seems self-evident; add its default, units, ownership,
  lifetime, serialization meaning, and invariants where relevant.
- Describe non-obvious preconditions, bounds, empty-input behavior, and
  exceptions or error reporting.
- Explain whether returned pointers, views, references, callbacks, or leases
  require a lifetime token or remain valid only during a visitor call.
- Explain whether an operation materializes data, performs lazy access, uses a
  bounded cache, or changes storage representation.
- Document compatibility behavior when an API reads old serialized formats or
  preserves an eager legacy API.
- Use `@param` and `@return` when they add information beyond the declaration.
  Do not add boilerplate tags whose meaning is already obvious.

## Style and accuracy

- Match the surrounding core tone: direct, technical, and explanatory rather
  than promotional.
- Use `/** ... */` for Doxygen documentation and keep paragraphs separated by
  blank comment lines when the explanation has distinct ideas.
- Prefer `@brief` for the first summary sentence.
- Use the repository’s author or maintainer format, normally `@author Thomas
  Kroes` for newly documented project classes when no more specific ownership
  convention is present.
- Do not invent guarantees about thread safety, performance, persistence,
  ownership, or exception behavior. Verify them in the implementation.
- Do not expose implementation-only includes or storage details in public
  headers when a forward declaration and source-level documentation are
  sufficient.
- Keep documentation changes focused. Do not reformat unrelated code or add a
  standalone documentation system when a Doxygen comment is sufficient.

## Review checklist

Before handing off the change, verify:

- public classes and APIs in scope have useful header documentation;
- every changed class or struct has a concise comment for every data member,
  and related trailing comments align to a consistent column;
- class rationale, ownership/lifetime, and important limits are explained;
- non-obvious parameters, returns, failure behavior, and compatibility rules
  are documented;
- wording matches nearby core documentation and the author format is correct;
- comments describe actual behavior rather than intended future behavior;
- public and non-obvious structure/class members are documented and related
  member declarations are neatly aligned;
- no unrelated formatting churn was introduced;
- `git diff --check` passes.
