"""Utility functions for scripts in this directory."""

import math

import mpmath


def split(target) -> tuple[float, float]:
    """Split a mpmath.mpf number into two float numbers.

    Args:
        target: Target mpmath.mpf number.

    Returns:
        tuple[float, float]: A tuple containing the two float numbers.
    """
    upper = float(target)
    lower = float(target - mpmath.mpf(upper))
    assert abs(lower) <= 0.5 * math.ulp(upper)
    return upper, lower
