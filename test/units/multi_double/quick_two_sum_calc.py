"""Calculate true values for tests of quick_two_sum function."""

import mpmath

SAMPLES = [
    (0.6538735159764687, 0.5270818662876512e-6),
    (0.8265268781569042e5, -0.3078919110114601e-7),
    (-0.15789370800852565e-3, 0.8102649309423045e-16),
]

mpmath.mp.prec = 52 * 2 + 1  # Quad precision


def split(target) -> tuple[float, float]:
    """Split a mpmath.mpf number into two float numbers.

    Args:
        target: Target mpmath.mpf number.

    Returns:
        tuple[float, float]: A tuple containing the two float numbers.
    """
    upper = float(target)
    lower = float(target - mpmath.mpf(upper))
    return upper, lower


def calculate_one(a: float, b: float) -> None:
    """Calculate one pair of inputs.

    Args:
        a (float): The first input value.
        b (float): The second input value.
    """
    assert abs(a) > abs(b)
    summed = mpmath.mpf(a) + mpmath.mpf(b)
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
