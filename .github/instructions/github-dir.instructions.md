---
applyTo: ".github/**"
---

You are reviewing changes to CI/automation infrastructure. Start your review with a prominent warning.

Your review summary MUST begin with a GitHub alert blockquote as the very first output, before any heading. Use this format:

> **⚠️ CI/Automation Change:** This PR modifies files under `.github/`. These files control CI workflows, Actions configurations, and repository automation. A mistake here can break builds or disable security checks for every contributor. Reviewers should scrutinize these changes carefully.

After the warning, continue with your normal review summary.

Additionally, check for:
- Actions not pinned to a specific version (e.g. using `@main` instead of `@v6`)
- Secrets exposed or referenced insecurely
- Incorrect or missing `if` conditions on workflow steps
- Unnecessary permission or token scope escalation
