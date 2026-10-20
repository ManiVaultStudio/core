---
name: group-functional-commits
description: Group current Git worktree changes into coherent functional commits when the user asks to commit changes separately by feature or purpose.
metadata:
  short-description: Group and commit worktree changes
---

# Group Functional Commits

Use this skill when the user asks to commit the current working tree in
separate functional parts.

## Workflow

1. Inspect the current branch, worktree status, diff statistics, changed
   files, and recent commits.
2. Read the relevant diffs before staging anything.
3. Group changes by functional purpose, not merely by file:
   - keep tightly coupled implementation and supporting UI changes together;
   - separate unrelated features;
   - keep formatting-only changes separate when practical;
   - preserve existing user changes.
4. Tell the user briefly which commit groups will be created.
5. Stage and commit one group at a time with concise, descriptive commit
   messages.
6. Do not push unless the user explicitly requests pushing.
7. If a file contains unrelated changes, use patch staging or carefully split
   the file rather than including everything in one commit.
8. Run `git diff --check` and inspect `git status` after committing.
9. Report each commit hash, message, and whether the worktree is clean.

## Safety

- Never use destructive commands such as `git reset --hard` or
  `git checkout --`.
- Do not discard, rewrite, or overwrite existing user changes.
- Do not amend existing commits unless explicitly requested.
- Do not create commits for untracked files whose purpose is unclear; ask the
  user first.
- If the repository prevents Git metadata changes, request permission for the
  staging and commit operation.
