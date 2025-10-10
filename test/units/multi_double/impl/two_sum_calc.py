"""Calculate true values for tests of two_sum function."""

import mpmath
from num_collect_test_utils.multi_double.split_quad import split_quad

SAMPLES = [
    (0.36442909686949376, 0.10112146788696608e-10),
    (-0.1805124821888754e-7, 0.33788841712672646e4),
    (0.7751347289611943e-2, -0.5452358218645957e3),
    (-0.14842670656838974e-4, 0.8840115830045514e-10),
]

mpmath.mp.prec = 52 * 2 + 1  # Quad precision


def calculate_one(a: float, b: float) -> None:
    """Calculate one pair of inputs.

    Args:
        a (float): The first input value.
        b (float): The second input value.
    """
    summed = mpmath.mpf(a) + mpmath.mpf(b)
    upper, lower = split_quad(summed)
    print(
        f"a={a.hex()}\n"
        f"b={b.hex()}\n"
        f"upper={upper.hex()}\n"
        f"lower={lower.hex()}\n"
        f"tuple=({a.hex()}, {b.hex()}, {upper.hex()}, {lower.hex()})\n",
    )


def main() -> None:
    """Calculate all pairs of inputs."""
    for a, b in SAMPLES:
        calculate_one(a, b)


if __name__ == "__main__":
    main()
