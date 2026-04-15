---
applyTo: ".github/**"
---

The very first thing in your review summary, before any heading or overview, MUST be this exact blockquote:

> [!WARNING]
> This PR modifies CI/automation files under `.github/`. Changes here can break builds, disable security checks, or disrupt automation for every contributor. Review these files with extra caution.

Do not skip this blockquote. Do not rephrase it. Do not embed it inside a paragraph.

## Review checklist

- Verify that all GitHub Actions use pinned versions (e.g. `actions/checkout@v6`), never `@main` or `@master`.
- Check that secrets are not exposed or referenced insecurely.
- Confirm any new or changed `if` conditions in workflow steps are logically correct and fail-safe.
- Ensure new permissions or token scopes are justified and minimal.
