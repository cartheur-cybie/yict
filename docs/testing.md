# Testing

## Build

```bash
cmake -S native -B build/native
cmake --build build/native -j
```

## Run Tests

```bash
ctest --test-dir build/native --output-on-failure
```

## Manual Smoke Test

```bash
./build/native/yict_cli inspect-yict release/generic202
./build/native/yict_cli export-icb release/generic202 /tmp/generic202.icb
./build/native/yict_cli import-icb release/generic202 /tmp/generic202.icb /tmp/generic202_smoke
./build/native/yict_cli inspect-yict /tmp/generic202_smoke
```

Expected:
- first inspect succeeds (generation usually `1` for `generic202`)
- export succeeds
- import writes new `-l.bin` / `-h.bin`
- second inspect succeeds with generation incremented by `1`

## Related

- [Native Status](native-status.md)
- [Next Steps](next-steps.md)
