"""round function for mpmath.mpf."""

import mpmath


def round(x):  # pylint: disable=redefined-builtin
    """Round the input value to the nearest integer.

    Args:
        x: Input value in mpmath.mpf.

    Returns:
        Rounded value in mpmath.mpf.
    """
    if x >= 0:
        return mpmath.floor(x + mpmath.mpf(0.5))
    return -mpmath.floor(-x + mpmath.mpf(0.5))
