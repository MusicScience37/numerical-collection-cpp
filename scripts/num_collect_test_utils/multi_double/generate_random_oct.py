"""Generate random oct number."""

import mpmath
from num_collect_test_utils.multi_double.split_oct import split_oct

mpmath.mp.prec = 53 * 4 - 1  # Octuple precision


def main() -> None:
    """Generate a random quad number."""
    random = mpmath.extraprec(20, True)(mpmath.rand)()
    number = mpmath.power(10, random * 30 - 15)
    term0, term1, term2, term3 = split_oct(number)
    print(
        f'(float.fromhex("{term0.hex()}"), float.fromhex("{term1.hex()}"), '
        f'float.fromhex("{term2.hex()}"), float.fromhex("{term3.hex()}"),)'
    )
    print(number)


if __name__ == "__main__":
    main()
