# Security Hardening Record

## Purpose and scope

This document records targeted security hardening measures applied to ManiVault
Core, together with deliberate compatibility and release-risk decisions. It is
intended to make the security posture of the repository understandable to
maintainers, reviewers, and contributors.

This is an engineering record, not a security certification or a guarantee that
the software is free of vulnerabilities. New changes should be reviewed against
the controls and residual risks described here.

## Security posture

The current hardening work focuses on untrusted or semi-trusted content that can
be opened by the application: archive entries, tutorial metadata, learning
content, custom asset paths, external links, and Markdown-rendered content.

The controls below are intentionally narrow so that the release is protected
without imposing broad limits that could reject valid scientific projects.

## Implemented controls

| Area | Threat addressed | Control | Reference | Status |
| --- | --- | --- | --- | --- |
| Archive extraction | Archive entries escaping the destination directory through `..` paths or symbolic links | Archive paths are canonicalized and required to remain within the destination directory; relative traversal and archive symbolic links are rejected. | [PR #1364](https://github.com/ManiVaultStudio/core/pull/1364) | Implemented |
| Tutorial metadata | HTML injection through tutorial titles or tags | Tutorial metadata inserted into generated HTML is HTML-escaped. Tutorial body content and its required tutorial JavaScript remain supported. | [PR #1365](https://github.com/ManiVaultStudio/core/pull/1365) | Implemented |
| External links | Embedded content causing the application to open dangerous URL schemes | Clicked external links are restricted to valid HTTP(S) URLs with a host before being passed to the desktop URL handler. Schemes such as `file:`, `data:`, `javascript:`, `mailto:`, and custom schemes are rejected. | [PR #1367](https://github.com/ManiVaultStudio/core/pull/1367) | Implemented |
| Learning-center content | HTML injection through a video title | Video titles inserted into the learning-center HTML are HTML-escaped. | [PR #1368](https://github.com/ManiVaultStudio/core/pull/1368) | Implemented |
| Custom asset serving | Symlink or path edge cases escaping the configured asset root | The configured root and requested asset path must resolve to valid canonical paths, and the resolved asset must remain within the canonical root. | [PR #1369](https://github.com/ManiVaultStudio/core/pull/1369) | Implemented |
| Markdown viewer | Dependency drift and unnecessary script loading in rendered Markdown | CDN dependencies are version-pinned, the redundant script load was removed, and a restrictive Content Security Policy limits permitted resource types and origins. | [PR #1370](https://github.com/ManiVaultStudio/core/pull/1370) | Implemented |

## Compatibility and risk decisions

Some possible controls were deliberately not applied globally because they could
break legitimate workflows or are operational controls rather than application
code changes.

| Topic | Decision | Rationale / follow-up |
| --- | --- | --- |
| Raw HTML in Markdown | Retained | Plugins may use embedded HTML. The Markdown viewer was hardened with pinned dependencies and CSP controls without removing the supported Markdown feature. Any future sanitization proposal must first inventory plugin usage and provide a compatibility plan. |
| JavaScript in tutorial pages | Retained | Existing tutorials use JavaScript for expand/collapse behavior. Metadata is escaped while the required tutorial behavior remains available. |
| Serialization and decompression resource limits | Deferred | Some valid projects contain data files around 70 GB. Broad fixed limits could break supported workflows. Future safeguards should be operation-specific and configurable, with measurements from real projects before rollout. |
| CI credentials in pull-request builds | Mitigated operationally | Repository settings are used to restrict secret availability according to the project team's current configuration. Workflow and repository settings should be revalidated whenever CI permissions or publishing workflows change. |
| Third-party GitHub Action SHA pinning | Deferred | Dependabot-based SHA maintenance is not fully rolled out yet. Pinning should be revisited once that automation is available and stable. |

## Residual risk and follow-up

- The controls above reduce specific, reviewed attack paths; they do not replace
  dependency updates, code review, least-privilege repository settings, or
  secure release procedures.
- The CI credential model remains dependent on repository configuration and
  workflow review. Changes to publishing, deployment, SSH, Sentry, or other
  privileged integrations should be reviewed as security-sensitive changes.
- Action pinning and resource-limit designs remain follow-up items rather than
  completed controls.
- Any new feature that renders user-, plugin-, tutorial-, or project-controlled
  text as HTML should use context-appropriate escaping and receive a focused
  review.

## Verification

The hardening changes were verified proportionally to their scope during the
associated changes:

- Archive extraction tests were added and the standalone archive test passed.
- The affected Core and TutorialPlugin targets built successfully where
  applicable.
- The Markdown viewer changes were checked with the repository's build process.
- The full project CI suite remains the final integration check for the stacked
  pull requests.

## Change history

| Date | Change |
| --- | --- |
| 2026-10-09 | Added this security hardening record covering the current targeted security stack and deferred controls. |
