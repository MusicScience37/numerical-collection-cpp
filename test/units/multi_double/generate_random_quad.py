"""Generate a random quad number."""

import mpmath
from quad_test_utils import split

mpmath.mp.prec = 52 * 2 + 1  # Quad precision


def main() -> None:
    """Generate a random quad number."""
    random = mpmath.extraprec(20, True)(mpmath.rand)()
    number = mpmath.power(10, random * 30 - 15)
    upper, lower = split(number)
    print(f'(float.fromhex("{upper.hex()}"), float.fromhex("{lower.hex()}"))')


if __name__ == "__main__":
    main()
