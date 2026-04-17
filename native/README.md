# Native Prototype

This directory contains the Linux-native scaffold for the medium-term YICT port.

## Build

```bash
cmake -S native -B build/native
cmake --build build/native -j
```

## Run

```bash
./build/native/yict_cli inspect-yict release/generic202
./build/native/yict_cli export-icb release/generic202 /tmp/generic202.icb
./build/native/yict_cli import-icb release/generic202 /tmp/generic202.icb /tmp/generic202_out
```

## Test

```bash
ctest --test-dir build/native --output-on-failure
```

## Current Scope

- Load `*-l.bin` and `*-h.bin`
- Interleave into a 256 KiB ROM image in memory
- Validate YICT metadata tags (`YICT202` and `YICTEND`)
- Parse and rewrite condition/action tables
- Export/import `.icb` behavior files

The command currently acts as a smoke-test harness for `yict_core`.

## More Docs

- [`docs/native-status.md`](/home/cartheur/ame/aiventure/aiventure-github/i-cybie/yict/docs/native-status.md)
- [`docs/testing.md`](/home/cartheur/ame/aiventure/aiventure-github/i-cybie/yict/docs/testing.md)
