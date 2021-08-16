# 3. Use unity build to speed up clang tidy

Date: 2021-07-31

## Status

Accepted

## Context

Static analysis using clang-tidy in CI took too long time.
I found following procedures to solve this problem:

- Use unity builds (create only one .cpp file to build a binary).
  - Reference
    - [How to speed up clang-tidy with unity builds | Method Park by UL](https://www.methodpark.de/blog/how-to-speed-up-clang-tidy-with-unity-builds/)
- Apply clang-tidy for changed files only.
  - Reference
    - [Speedup Clang Tidy By Running On Changed Files Only · Issue #1487 · ros-planning/moveit](https://github.com/ros-planning/moveit/issues/1487)

## Decision

I use unity build to speed up clang-tidy.

- Difficulty in apply clang-tidy for changed files only:
  - Applying clang-tidy for header files cannot work well with template classes,
    so source files including changed headers must be checked using clang-tidy.

## Consequences

Static analysis using clang-tidy takes much less time with unity build.
