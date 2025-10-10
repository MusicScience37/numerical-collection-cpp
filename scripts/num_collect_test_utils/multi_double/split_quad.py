"""Utility functions for tests of quad class."""

import math

import mpmath


def split_quad(target) -> tuple[float, float]:
    """Split a mpmath.mpf number representing a quad number into two float numbers.

    Args:
        target: Target mpmath.mpf number.

    Returns:
        tuple[float, float]: A tuple containing the two float numbers.
    """
    upper = float(target)
    lower = float(target - mpmath.mpf(upper))
    assert abs(lower) <= 0.5 * math.ulp(upper)
    return upper, lower
