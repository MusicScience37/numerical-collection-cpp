"""Generate a random quad number."""

import mpmath
from num_collect_test_utils.multi_double.split_quad import split_quad

mpmath.mp.prec = 52 * 2 + 1  # Quad precision


def main() -> None:
    """Generate a random quad number."""
    random = mpmath.extraprec(20, True)(mpmath.rand)()
    number = mpmath.power(10, random * 30 - 15)
    upper, lower = split_quad(number)
    print(f'(float.fromhex("{upper.hex()}"), float.fromhex("{lower.hex()}"))')
    print(number)


if __name__ == "__main__":
    main()
