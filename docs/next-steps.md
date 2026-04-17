# Next Steps

This is the practical execution plan from the current native baseline to a usable Linux-first YICT workflow.

## 1) Stabilize Core Behavior

- Recover canonical `unames.h` and `unames2.h`, then replace the ROM-derived fallback mapping.
- Add stronger guardrails around ROM parsing:
  - strict bounds checks for all table pointers
  - explicit error codes for each parse stage
- Add core APIs for scripted edits without going through `.icb` only:
  - set single `ACT(index)=storage`
  - set `PROBACT(index)=storage,...`
  - set general properties (`STRETCH`, `WAGTAIL`, etc.)

Exit criteria:
- deterministic storage/action mapping identical to legacy behavior for reference ROMs.

## 2) Expand Regression Tests

- Add fixture-based tests for:
  - known-good ROMs
  - malformed/truncated ROMs
  - malformed `.icb` inputs
- Add snapshot-style assertions:
  - exported `.icb` stability
  - no-op import/export idempotency
  - expected generation increments only on write
- Add CI workflow to run native build + tests on Linux.

Exit criteria:
- reproducible green test suite in CI for every commit.

## 3) Deliver a Power-User CLI

- Add explicit edit commands:
  - `set-general`
  - `set-act`
  - `set-probact`
  - `show-cond`
- Add `--dry-run` and `--json` output modes for automation.
- Preserve existing commands:
  - `inspect-yict`
  - `export-icb`
  - `import-icb`

Exit criteria:
- daily editing possible from CLI without touching legacy MFC app.

## 4) Build Native UI (Qt)

- Implement a thin Qt shell over `yict_core`:
  - open/save
  - condition tree
  - action assignment views
  - general options panel
- Keep all ROM logic in core; UI only orchestrates operations.

Exit criteria:
- native Linux GUI supports the common behavior-edit loop end-to-end.

## 5) Packaging and Release

- Add install/package targets (tarball at minimum).
- Write user-facing docs:
  - install/build
  - edit flow
  - safety/backup recommendations
- Produce a versioned changelog for native milestones.

Exit criteria:
- another user can clone, build, run, and edit without tribal knowledge.

## 6) Hardware Transfer Workflow

- Document and test Linux transfer path using sibling tooling (`icburn`, `super`, `sdk`) or a clean wrapper.
- Clearly separate editor output from flashing responsibilities.

Exit criteria:
- verified path from edited ROM output to hardware programming documentation.

