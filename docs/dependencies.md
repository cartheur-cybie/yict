# Native Port Dependency Notes

## Reconstructed

- `native/include/legacy/yictdata202.h`
  - Source of truth: `../himage/src/yictdata.a_`
  - Cross-checked against usage in:
    - `source/cart.cpp`
    - `source/custom.cpp`
    - `../icaud/icaud.cpp`

## Missing Upstream Tables

- `unames.h`
- `unames2.h`

These were referenced by legacy VC6 project files but are absent from current
local repositories. Native code currently uses ROM-derived storage-number
mapping as a deterministic fallback for action IDs.

