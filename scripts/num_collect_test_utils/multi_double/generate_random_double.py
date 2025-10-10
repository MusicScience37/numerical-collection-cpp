"""Generate a random double number."""

import mpmath

mpmath.mp.prec = 53  # Double precision


def main() -> None:
    """Generate a random double number."""
    random = mpmath.extraprec(20, True)(mpmath.rand)()
    number = mpmath.power(10, random * 30 - 15)
    print(f'float.fromhex("{float(number).hex()}")')


if __name__ == "__main__":
    main()
