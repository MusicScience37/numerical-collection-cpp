"""Calculate true values for pow_positive_int_impl(quad, int) function."""

import mpmath
from num_collect_test_utils.multi_double.split_quad import split_quad

mpmath.mp.prec = 52 * 2 + 1  # Quad precision

SAMPLES = [
    # cspell: disable
    (
        (
            float.fromhex("0x1.7e55eca788245p+5"),
            float.fromhex("0x1.d2c49ebca1298p-50"),
        ),
        0,
    ),
    (
        (
            float.fromhex("0x1.be580cb8d6672p+5"),
            float.fromhex("-0x1.7c1b0862537e4p-49"),
        ),
        1,
    ),
    (
        (
            float.fromhex("0x1.5a2b98fe9585cp-10"),
            float.fromhex("-0x1.0c36fa4e937a0p-66"),
        ),
        3,
    ),
    (
        (
            float.fromhex("0x1.abf6487b5305fp+5"),
            float.fromhex("0x1.924c526aeb870p-50"),
        ),
        10,
    ),
    (
        (
            float.fromhex("0x1.5df1c02a3661fp-1"),
            float.fromhex("0x1.98c0818650188p-57"),
        ),
        100,
    ),
    (
        (
            float.fromhex("0x1.17ce6ac42961cp+0"),
            float.fromhex("-0x1.070ccd508d778p-55"),
        ),
        1024,
    ),
    # cspell: enable
]


def calculate_one(a: tuple[float, float], b: int) -> None:
    """Calculate one pair of inputs.

    Args:
        a (tuple[float, float]): The first input value.
        b (int): The second input value.
    """
    a_mpf = mpmath.mpf(a[0]) + mpmath.mpf(a[1])
    summed = mpmath.power(a_mpf, b)
    upper, lower = split_quad(summed)
    print(
        f"std::make_tuple(quad({a[0].hex()}, {a[1].hex()}), "
        f"{b}U, "
        f"quad({upper.hex()}, {lower.hex()})),"
    )


def main() -> None:
    """Calculate all pairs of inputs."""
    for a, b in SAMPLES:
        calculate_one(a, b)


if __name__ == "__main__":
    main()
