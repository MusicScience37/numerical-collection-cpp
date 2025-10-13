"""Calculate true values for tests of asin(quad) function."""

import mpmath
from num_collect_test_utils.multi_double.split_quad import split_quad

mpmath.mp.prec = 52 * 2 + 1  # Quad precision

SAMPLES = [
    # cspell: disable
    (-1.0, 0.0),
    (float.fromhex("-0x1.f858f4b0b2a03p-1"), float.fromhex("0x1.c08ae3c4a76d4p-55")),
    (float.fromhex("-0x1.6e29015574577p-5"), float.fromhex("-0x1.42a63fd385670p-60")),
    (float.fromhex("0x1.a8c218be4484dp-32"), float.fromhex("0x1.a231ce8c92588p-87")),
    (float.fromhex("0x1.65528728f7924p-8"), float.fromhex("0x1.69b0488d1a5e0p-63")),
    (float.fromhex("0x1.9f63608ae2177p-1"), float.fromhex("-0x1.72f68eb27d260p-58")),
    (1.0, 0.0),
    # cspell: enable
]


def calculate_one(inp: tuple[float, float]) -> None:
    """Calculate one input.

    Args:
        inp (tuple[float, float]): Input values.
    """
    input_mpf = mpmath.mpf(inp[0]) + mpmath.mpf(inp[1])
    result_mpf = mpmath.extraprec(20, True)(mpmath.asin)(input_mpf)
    upper, lower = split_quad(result_mpf)
    print(
        f"std::make_tuple(quad({inp[0].hex()}, {inp[1].hex()}), "
        f"quad({upper.hex()}, {lower.hex()})),"
    )


def main() -> None:
    """Calculate all pairs of inputs."""
    for inp in SAMPLES:
        calculate_one(inp)


if __name__ == "__main__":
    main()
