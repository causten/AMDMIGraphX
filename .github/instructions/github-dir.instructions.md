---
applyTo: ".github/**"
---

## Mandatory warning banner

When any file under `.github/` is changed, the **very first line** of the review summary MUST be a blockquote warning banner using this exact format:

> [!WARNING]
> This PR modifies CI/automation files under `.github/`. Changes here can break builds, disable security checks, or disrupt automation for every contributor. Review these files with extra caution.

This banner must appear before the pull request overview or any other content. Do not omit it, do not embed it inside a paragraph, and do not rephrase it as a casual note.

## Review checklist for `.github/` changes

- Verify that all GitHub Actions use pinned versions (e.g. `actions/checkout@v6`), never `@main` or `@master`.
- Check that secrets are not exposed or referenced insecurely.
- Confirm any new or changed `if` conditions in workflow steps are logically correct and fail-safe.
- Ensure new permissions or token scopes are justified and minimal.
