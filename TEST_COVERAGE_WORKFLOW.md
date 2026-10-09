# Core Test Coverage Workflow

This file is the durable operating guide for the recurring core test coverage work. The scheduled task must read it before every run.

## Policy

- Make at most one small, meaningful test-coverage improvement per run.
- Work on one weekly branch and pull request.
- Never push directly to `master`.
- Never merge a pull request automatically.
- Preserve existing user changes and unrelated work.
- Leave changes reviewable and report the exact next action required from the human.
- Ask for explicit human approval before committing, pushing, creating a pull request, or merging.

## Branch naming

Use this naming convention:

```text
feature/core-test-coverage-YYYY-Www
```

For example:

```text
feature/core-test-coverage-2026-W41
```

## Current campaign

- Week: 2026-W41
- Branch: `feature/core-test-coverage-2026-W41-followup`
- Planned branch: none; current follow-up branch is active
- Pull request: not opened
- Status: preparing the next reviewable public-core coverage change after PR #1362 merged

## Daily work

1. Read this file and `TEST_COVERAGE_BACKLOG.md`.
2. Inspect the current branch and working tree.
3. If unreviewed changes from the previous run remain, do not start another implementation.
4. Add at most one focused test improvement.
5. Run the narrowest relevant tests and update the backlog.
6. Report the current branch, PR state, files changed, test results, and the next human action.

## Weekly checkpoint

At the end of the week:

1. Stop adding new test changes.
2. Run the relevant test suite.
3. Prepare a summary of all changes and coverage impact.
4. Ask for approval before committing, pushing, or opening a pull request.
5. Do not start the next weekly branch until the current pull request has been merged.

## After a pull request is merged

1. Confirm that the base branch is up to date.
2. Create the next weekly branch from the updated `master`.
3. Update the current campaign section in this file.
4. Continue using the existing backlog.

## Pause switch

To pause the campaign, add a top-level `PAUSED` marker to this file. While that marker exists, the scheduled task must not modify code or the backlog.
