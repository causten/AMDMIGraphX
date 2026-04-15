---
applyTo: ".github/**"
---

You are reviewing changes to CI/automation infrastructure. Start your review with a prominent warning.

Your review summary MUST begin with a GitHub alert blockquote as the very first output, before any heading. Use this format:

> **⚠️ CI/Automation Change:** This PR modifies files under `.github/`. These files control CI workflows, Actions configurations, and repository automation. A mistake here can break builds or disable security checks for every contributor. Reviewers should scrutinize these changes carefully.

After the warning, continue with your normal review summary.

- Verify that all GitHub Actions use pinned versions (e.g. `actions/checkout@v6`), never `@main` or `@master`.
- Check that secrets are not exposed or referenced insecurely.
- Confirm any new or changed `if` conditions in workflow steps are logically correct and fail-safe.
- Ensure new permissions or token scopes are justified and minimal.
- If any file in ./github/workflow/ is modified ensure there is a 2026 copyright 
