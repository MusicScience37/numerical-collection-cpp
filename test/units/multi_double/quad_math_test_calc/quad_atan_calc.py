"""Calculate true values for tests of atan(quad) function."""

import mpmath
from num_collect_test_utils.multi_double.split_quad import split_quad

mpmath.mp.prec = 52 * 2 + 1  # Quad precision

SAMPLES = [
    # cspell: disable
    (float.fromhex("-0x1.81b6443eaab93p+35"), float.fromhex("0x1.a72f57922eef8p-20")),
    (float.fromhex("-0x1.669ed2612fc1cp+7"), float.fromhex("-0x1.ff9fcb47b9e00p-53")),
    (-1.0, 0.0),
    (float.fromhex("-0x1.5bc7759f2479ep-2"), float.fromhex("0x1.370ac47213f70p-57")),
    (float.fromhex("-0x1.c792b1acb54c8p-3"), float.fromhex("-0x1.de97dcdf1ce84p-57")),
    (float.fromhex("-0x1.d3f23b88b6ab2p-41"), float.fromhex("0x1.2e25012d428a0p-96")),
    (float.fromhex("0x1.592550521dba1p-42"), float.fromhex("0x1.b8ec35b72f7e8p-96")),
    (float.fromhex("0x1.076d4d74eca62p-1"), float.fromhex("-0x1.5065cd615be8cp-55")),
    (1.0, 0.0),
    (float.fromhex("0x1.5eda751c2c50fp+1"), float.fromhex("-0x1.5d54e671ea9d0p-53")),
    (float.fromhex("0x1.65c5033df841fp+19"), float.fromhex("-0x1.1c8698d601558p-36")),
    (float.fromhex("0x1.94ae79531ccc5p+42"), float.fromhex("0x1.3b24e86e7e6d8p-13")),
    # cspell: enable
]


def calculate_one(inp: tuple[float, float]) -> None:
    """Calculate one input.

    Args:
        inp (tuple[float, float]): Input values.
    """
    input_mpf = mpmath.mpf(inp[0]) + mpmath.mpf(inp[1])
    result_mpf = mpmath.extraprec(20, True)(mpmath.atan)(input_mpf)
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
