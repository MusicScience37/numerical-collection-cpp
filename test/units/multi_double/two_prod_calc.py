"""Calculate true values for tests of two_prod function."""

import mpmath
from quad_test_utils import split

SAMPLES = [
    (0.693381641899671e5, 0.11821175647226123e-2),
    (-0.3410913472042353e-4, 0.8060188004666143e3),
    (0.3667389932584115e3, -0.7226965248639126e-6),
]

mpmath.mp.prec = 52 * 2 + 1  # Quad precision


def calculate_one(a: float, b: float) -> None:
    """Calculate one pair of inputs.

    Args:
        a (float): The first input value.
        b (float): The second input value.
    """
    summed = mpmath.mpf(a) * mpmath.mpf(b)
    upper, lower = split(summed)
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
