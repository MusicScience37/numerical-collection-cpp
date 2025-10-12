"""Calculate internal constants."""

import mpmath
from num_collect_test_utils.multi_double.split_quad import split_quad

mpmath.mp.prec = 52 * 2 + 1  # Quad precision

VALUES = [
    ("log2", mpmath.extraprec(20, True)(mpmath.log)(2)),
    ("half_log2", 0.5 * mpmath.extraprec(20, True)(mpmath.log)(2)),
    ("log2_inv", 1 / mpmath.extraprec(20, True)(mpmath.log)(2)),
    ("log10", mpmath.extraprec(20, True)(mpmath.log)(10)),
    ("log10_inv", 1 / mpmath.extraprec(20, True)(mpmath.log)(10)),
    ("exp_maclaurin_limit", mpmath.mpf("1.36e-3")),
    ("sqrt2", mpmath.extraprec(20, True)(mpmath.sqrt)(2)),
    ("sqrt2_inv", 1 / mpmath.extraprec(20, True)(mpmath.sqrt)(2)),
]


def calculate_one(name: str, value) -> None:
    """Calculate one constant.

    Args:
        name (str): The name of the constant.
        value: The value of the constant.
    """
    upper, lower = split_quad(value)
    print(
        f"{name}_quad = quad({upper.hex()}, {lower.hex()})",
    )


def main() -> None:
    """Calculate all constants."""
    for name, value in VALUES:
        calculate_one(name, value)


if __name__ == "__main__":
    main()
