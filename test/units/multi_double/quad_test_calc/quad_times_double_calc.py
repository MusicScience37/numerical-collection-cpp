"""Calculate true values for tests of quad * double operation."""

import mpmath
from num_collect_test_utils.multi_double.split_quad import split_quad
from quad_plus_double_calc import SAMPLES

mpmath.mp.prec = 52 * 2 + 1  # Quad precision


def calculate_one(a: tuple[float, float], b: float) -> None:
    """Calculate one pair of inputs.

    Args:
        a (tuple[float, float]): The first input value.
        b (float): The second input value.
    """
    a_mpf = mpmath.mpf(a[0]) + mpmath.mpf(a[1])
    b_mpf = mpmath.mpf(b)
    summed = a_mpf * b_mpf
    upper, lower = split_quad(summed)
    print(
        f"a=({a[0].hex()}, {a[1].hex()})\n"
        f"b={b.hex()}\n"
        f"upper={upper.hex()}\n"
        f"lower={lower.hex()}\n"
        f"tuple=(quad({a[0].hex()}, {a[1].hex()}), {b.hex()}, "
        f"quad({upper.hex()}, {lower.hex()}))\n",
    )


def main() -> None:
    """Calculate all pairs of inputs."""
    for a, b in SAMPLES:
        calculate_one(a, b)


if __name__ == "__main__":
    main()
