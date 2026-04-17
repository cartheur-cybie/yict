# Native Port Status

## What Works Now

- Linux build with CMake (`native/`).
- CLI inspection of ROM halves and YICT metadata.
- `.icb` export from ROM.
- `.icb` import into ROM and save to new output halves.
- PROBACTION table parse + rewrite in pool3.
- General settings parse/import/export:
  - `STRETCH`
  - `WAGTAIL`
  - `MOODDATA`
  - `SSL_NEWDATA`
  - `SSL_SKIPCOUNT`
  - `DISABLE_CHARGER_INIT`
  - `MUTE_INIT`
  - `IDLE_DELAY`

## Commands

- `yict_cli inspect-yict <base-name>`
- `yict_cli export-icb <base-name> <out.icb>`
- `yict_cli import-icb <in-base-name> <in.icb> <out-base-name>`

## Test Coverage Today

- End-to-end roundtrip:
  - inspect input ROM
  - export `.icb`
  - import `.icb` to new ROM
  - inspect output ROM
  - verify generation bump
- Failure paths:
  - missing ROM files
  - malformed `.icb` line

## Known Gaps

- No native UI yet (CLI only).
- No canonical `unames.h` / `unames2.h` mapping tables recovered yet.
  - Current implementation uses deterministic ROM-derived storage indexing.
- Hardware transfer remains external.

