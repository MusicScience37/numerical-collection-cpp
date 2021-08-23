# 4. Stop using lambda functions in macros

Date: 2021-08-24

## Status

Accepted

## Context

When macros are implemented using lambda functions, variables (to be accurate, these aren't variable) in structured binding cannot be used with such macros.

Example:

```cpp
// current implementation of assertion
#define NUM_COLLECT_ASSERT(CONDITION)                                    \
    [&] {                                                                \
        if (!(CONDITION)) {                                              \
            throw ::num_collect::assertion_failure(                      \
                fmt::format("assertion failure: {} (at {})", #CONDITION, \
                    NUM_COLLECT_FUNCTION));                              \
        }                                                                \
    }()

// user code
void test() {
    const auto [var1, var2] = some_function();
    NUM_COLLECT_ASSERT(var1 == var2); // This is illegal.
}
```

## Decision

I will stop using lambda functions in macro definitions.

## Consequences

Re-implementation using ordinary function solved the problem.
