"""trunc function for mpmath.mpf."""

import mpmath


def trunc(x):
    """Truncate the input value to an integer.

    Args:
        x: Input value in mpmath.mpf.

    Returns:
        Truncated value in mpmath.mpf.
    """
    if x >= 0:
        return mpmath.floor(x)
    return -mpmath.floor(-x)
