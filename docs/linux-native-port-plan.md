# Linux Native Port Plan (Medium Term)

## Goal
Make YICT usable on Linux without Wine for the editor workflow:

- open `*-l.bin`/`*-h.bin`
- edit behavior/actions/options
- import/export `.icb`
- save updated ROM images

Out of scope for first native milestone:

- direct hardware transfer via legacy `WinIo`/`icburn.exe`

## Realistic Conclusion
A native Linux port is feasible if we split the project into:

1. `yict-core` (pure C++ ROM/behavior logic)
2. `yict-ui` (Qt/wxWidgets/GTK frontend)

The current code is tightly coupled to MFC/Win32, so this is a refactor + port, not a retarget.

## Current Blockers
The current repo cannot be rebuilt as-is from source because referenced shared headers are missing:

- `../yictdata202.h`
- `../unames.h`
- `../unames2.h`

These are required by [`source/cart.h`](/home/cartheur/ame/aiventure/aiventure-github/i-cybie/yict/source/cart.h) and [`source/custom.cpp`](/home/cartheur/ame/aiventure/aiventure-github/i-cybie/yict/source/custom.cpp).

## Recommended Target Stack
- Language: C++17
- Build: CMake + Ninja/Make
- UI: Qt 6 Widgets (best cross-platform tooling and model/view support)
- Tests: Catch2 or GoogleTest

## Phase Plan

### Phase 0: Reconstruct Build Inputs (1-3 days)
- Recover/create canonical `yictdata202.h` from `../himage/src/yictdata.a_` layout.
- Recover `unames.h` and `unames2.h` from upstream history/archives.
- Add `docs/dependencies.md` documenting provenance and checksums.

Exit criteria:
- headers are present in-repo and versioned.

### Phase 1: Extract Core Logic (3-6 days)
- Introduce `native/yict-core` library with no MFC dependency.
- Port these responsibilities from legacy code:
  - ROM half load/save
  - YICT header validation
  - condition/action table parsing
  - import/export `.icb`
- Replace `CString`, `CByteArray`, `AfxMessageBox` flows with:
  - `std::string`
  - `std::vector<uint8_t>`
  - explicit error returns/exceptions

Exit criteria:
- a CLI smoke tool can load, validate, and re-save ROM images.

### Phase 2: Behavioral Equivalence Tests (2-4 days)
- Add fixture corpus:
  - known good `generic202-*.bin`
  - representative `.icb` files
- Snapshot tests:
  - parse counts per condition
  - import/export roundtrip consistency
  - no-op save keeps ROM content stable (except expected generation fields)

Exit criteria:
- deterministic tests pass on Linux CI.

### Phase 3: Linux UI (4-8 days)
- Build Qt UI for:
  - condition tree
  - action list editing
  - general options
  - import/export/save
- Defer audio preview until core editor parity is stable.

Exit criteria:
- daily-driver parity for editing use cases.

### Phase 4: Hardware Transfer Story (Optional)
- Keep transfer external:
  - output files from native editor
  - program with existing Linux-friendly tools from sibling repos (`icburn`, `super`, `sdk`)

Exit criteria:
- documented Linux flashing workflow end-to-end.

## Architecture Boundary
Keep these strict interfaces:

- UI does not parse ROM internals directly.
- `yict-core` exposes domain objects and validates invariants.
- import/export stays in core so CLI and UI share behavior.

## Risk Register
- Missing canonical headers can drift behavior if reconstructed incorrectly.
- Legacy code relies on implicit assumptions (pointer casts, packed structures).
- Action-name maps (`unames*`) are critical for UX labeling and edit safety.

## Immediate Next Actions
1. Add missing headers (`yictdata202.h`, `unames.h`, `unames2.h`) with source provenance.
2. Move ROM load/save + YICT validation into `native/yict-core`.
3. Add first Linux CLI command: `yict-cli inspect <basename>`.
