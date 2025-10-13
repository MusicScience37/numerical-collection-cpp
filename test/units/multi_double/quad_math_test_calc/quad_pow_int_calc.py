"""Calculate true values for pow(quad, int) function."""

import mpmath
from num_collect_test_utils.multi_double.split_quad import split_quad

mpmath.mp.prec = 52 * 2 + 1  # Quad precision

SAMPLES = [
    # cspell: disable
    (
        (
            float.fromhex("0x1.0c05b74da84a2p+0"),
            float.fromhex("-0x1.e8d06162c6b5cp-54"),
        ),
        -1025,
    ),
    (
        (
            float.fromhex("0x1.a9d0751a1b25ap-1"),
            float.fromhex("-0x1.7a911ad8d16f4p-55"),
        ),
        -1024,
    ),
    (
        (
            float.fromhex("0x1.582f99cdb6be9p-3"),
            float.fromhex("-0x1.f16591b5a67acp-57"),
        ),
        -90,
    ),
    (
        (
            float.fromhex("0x1.db8b052be4d4fp+24"),
            float.fromhex("-0x1.cfb2d37c6e56cp-30"),
        ),
        -4,
    ),
    (
        (
            float.fromhex("0x1.7aa3801ec43e4p-8"),
            float.fromhex("-0x1.f923144bdc7f4p-62"),
        ),
        -1,
    ),
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
    (
        (
            float.fromhex("0x1.144a41d817304p+0"),
            float.fromhex("0x1.f971a86a04718p-54"),
        ),
        1025,
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
        f"{b}, "
        f"quad({upper.hex()}, {lower.hex()})),"
    )


def main() -> None:
    """Calculate all pairs of inputs."""
    for a, b in SAMPLES:
        calculate_one(a, b)


if __name__ == "__main__":
    main()
