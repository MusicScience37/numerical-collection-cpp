"""Split a mpmath.mpf number representing a oct number into four float numbers."""

import math

import mpmath


def split_oct(target) -> tuple[float, float, float, float]:
    """Split a mpmath.mpf number representing an oct number into four float numbers.

    Args:
        target: Target mpmath.mpf number.

    Returns:
        tuple[float, float, float, float]: A tuple containing the four float numbers.
    """
    part0 = float(target)
    remainder1 = target - mpmath.mpf(part0)
    part1 = float(remainder1)
    remainder2 = remainder1 - mpmath.mpf(part1)
    part2 = float(remainder2)
    remainder3 = remainder2 - mpmath.mpf(part2)
    part3 = float(remainder3)
    assert abs(part1) <= 0.5 * math.ulp(part0)
    assert abs(part2) <= 0.5 * math.ulp(part1)
    assert abs(part3) <= 0.5 * math.ulp(part2)
    return part0, part1, part2, part3
