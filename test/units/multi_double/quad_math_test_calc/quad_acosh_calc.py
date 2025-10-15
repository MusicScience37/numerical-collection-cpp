"""Calculate true values for tests of acosh(quad) function."""

import mpmath
from num_collect_test_utils.multi_double.split_quad import split_quad

mpmath.mp.prec = 52 * 2 + 1  # Quad precision

SAMPLES = [
    # cspell: disable
    (float.fromhex("0x1.02252dc716765p+0"), float.fromhex("-0x1.1991ea087ceb0p-55")),
    (float.fromhex("0x1.d0327dcded31dp+10"), float.fromhex("-0x1.068f5953bba90p-44")),
    (float.fromhex("0x1.9ef917859d81fp+46"), float.fromhex("0x1.7e00e7b8a7110p-10")),
    # cspell: enable
]


def calculate_one(inp: tuple[float, float]) -> None:
    """Calculate one input.

    Args:
        inp (tuple[float, float]): Input values.
    """
    input_mpf = mpmath.mpf(inp[0]) + mpmath.mpf(inp[1])
    result_mpf = mpmath.extraprec(20, True)(mpmath.acosh)(input_mpf)
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
