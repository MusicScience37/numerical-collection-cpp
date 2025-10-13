"""Calculate true values for tests of sin(quad) function."""

import mpmath
from num_collect_test_utils.multi_double.split_quad import split_quad

mpmath.mp.prec = 52 * 2 + 1  # Quad precision

SAMPLES = [
    # cspell: disable
    split_quad(mpmath.pi / 8),
    split_quad(mpmath.pi * 3 / 8),
    split_quad(mpmath.pi * 5 / 8),
    split_quad(mpmath.pi * 7 / 8),
    split_quad(mpmath.pi * 9 / 8),
    split_quad(mpmath.pi * 11 / 8),
    split_quad(mpmath.pi * 13 / 8),
    split_quad(mpmath.pi * 15 / 8),
    split_quad(mpmath.pi * 17 / 8),
    (float.fromhex("0x1.284ed19fda3c1p-6"), float.fromhex("-0x1.323876d6af6e4p-60")),
    (float.fromhex("0x1.807eb74675238p+4"), float.fromhex("0x1.10f7197fc8ba4p-50")),
    (float.fromhex("0x1.7861bf3a3a90fp-6"), float.fromhex("-0x1.1a15c2c1ded18p-61")),
    (float.fromhex("-0x1.d63e4eda10237p-9"), float.fromhex("-0x1.3debc994dbed8p-64")),
    (float.fromhex("-0x1.6b4fee68b4baap+3"), float.fromhex("0x1.f3bba23239db0p-51")),
    (float.fromhex("-0x1.e76a585102824p-28"), float.fromhex("-0x1.3e79f43e85a60p-82")),
    # cspell: enable
]


def calculate_one(inp: tuple[float, float]) -> None:
    """Calculate one input.

    Args:
        inp (tuple[float, float]): Input values.
    """
    input_mpf = mpmath.mpf(inp[0]) + mpmath.mpf(inp[1])
    result_mpf = mpmath.extraprec(20, True)(mpmath.sin)(input_mpf)
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
