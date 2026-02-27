# Release Flow

## Overview

Releases are fully automated via GitHub Actions. Pushing a version tag triggers the workflow.

## How to release

**1. Update version in three places:**

```bash
# CMakeLists.txt
project(HeadsetStatus VERSION 1.2.3 LANGUAGES CXX)

# PKGBUILD
pkgver=1.2.3

# CHANGELOG.md — add new section at the top:
## [1.2.3] - YYYY-MM-DD
### Added/Changed/Fixed
- ...
```

**2. Commit the version bump:**

```bash
git add CMakeLists.txt PKGBUILD CHANGELOG.md
git commit -m "bump version to 1.2.3"
```

**3. Push tag:**

```bash
git tag v1.2.3
git push origin main
git push origin v1.2.3
```

That's it. The rest is automatic.

---

## What the release workflow does

Triggered by any tag matching `v*.*.*`:

1. Installs `cmake`, `qt6-base-dev`, `libgl-dev`, `libdbus-1-dev`, `upx`
2. Builds binary with `cmake -DCMAKE_BUILD_TYPE=Release` (LTO, -Os, strip-all, UPX)
3. Downloads GitHub source tarball with retry (5 attempts, 10s between)
4. Computes sha256sum and patches `PKGBUILD` — exits with error if substitution fails
5. Extracts the `## [1.2.3]` section from `CHANGELOG.md` as release notes (fallback if missing)
6. Creates GitHub Release with:
   - `HeadsetStatus` — compiled binary
   - `PKGBUILD` — updated with correct sha256sum

---

## Workflows

| File | Trigger | Purpose |
|---|---|---|
| `.github/workflows/ci.yml` | push to `main`, PRs | Validates the build compiles |
| `.github/workflows/release.yml` | push tag `v*.*.*` | Creates GitHub Release |

Both use `actions/checkout@11bd71901bbe5b1630ceea73d27597364c9af683` (v4.2.2, SHA-pinned).

---

## Important notes

- **CHANGELOG.md must have the version section** before pushing the tag, otherwise release notes say `"No changelog entry found for X.Y.Z."`
- **sha256sums in PKGBUILD** is patched automatically by the workflow — do not update it manually before tagging
- **Remote must be SSH** (`git@github.com:mewset/headsetstatus.git`) — HTTPS OAuth lacks the `workflow` scope needed to push `.github/workflows/` files

To verify the remote is SSH:
```bash
git remote -v
# Should show: git@github.com:mewset/headsetstatus.git
```

To fix if it's HTTPS:
```bash
git remote set-url origin git@github.com:mewset/headsetstatus.git
```

---

## Deleting a release

```bash
gh release delete v1.2.3 --yes
git push origin --delete v1.2.3
git tag -d v1.2.3
```
