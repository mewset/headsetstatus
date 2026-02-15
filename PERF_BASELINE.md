# Performance Baseline

Collected on 2026-02-15 in `/home/m0s/Projekt/headsetstatus`.

## Binary size

- Packed executable (`build/HeadsetStatus`): `38952` bytes (~39 KB)
- UPX report: `115912 -> 38952` (`33.60%`)

## After PR1+PR2(+low-risk size pass)

- Packed executable (`build-release/HeadsetStatus`): `39884` bytes
- UPX report: `120040 -> 39884` (`33.23%`)
- A/B with `-Oz`: same packed size as current release in this code state

## Build and tests

- Configure/build: success (`cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON`)
- Test suite: `2/2` passing (`ctest --test-dir build --output-on-failure`)

## Notes

- Runtime wakeups/CPU/event-rate baseline is environment dependent and should be measured on a real user session with active D-Bus traffic.
- Debug counters introduced in `main.cpp` can be used to compare event coalescing efficiency before and after additional optimizations.
