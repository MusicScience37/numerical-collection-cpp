# 5. Remove thread sanitizer

Date: 2022-02-25

## Status

Accepted

## Context

Thread sanitizer cannot work correctly with OpenMP unless OpenMP is built using Thread sanitizer
([Related problem](https://stackoverflow.com/questions/33004809/can-i-use-thread-sanitizer-for-openmp-programs)).

## Decision

I gave up use of thread sanitizer for now.

## Consequences

Check of thread safety cannot be done.
