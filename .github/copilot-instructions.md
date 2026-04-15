When a pull request modifies any file under the `.github/` directory (including subdirectories like `.github/workflows/`), you MUST start your review summary with a visible warning before any other content. Use a bold emoji-prefixed line like:

> **⚠️ CI/Automation Change:** This PR modifies files under `.github/`. A mistake here can break builds or disable security checks for every contributor. Review with extra caution.

This warning must appear before the "Pull request overview" heading. Never omit it for PRs that touch `.github/`.
