"""Calculate true values for tests of exp(quad) function."""

import mpmath
from num_collect_test_utils.multi_double.split_quad import split_quad

mpmath.mp.prec = 52 * 2 + 1  # Quad precision

SAMPLES = [
    # cspell: disable
    (-707.0, 0.0),  # Almost minimum value
    (float.fromhex("-0x1.eecd4d2a8802bp+8"), float.fromhex("-0x1.0c0bd235ca6d4p-46")),
    (float.fromhex("-0x1.b93d3796001a0p+4"), float.fromhex("-0x1.ad3954413d8ccp-50")),
    (float.fromhex("-0x1.b684eabe415bbp+0"), float.fromhex("0x1.8394ecb303f2cp-54")),
    split_quad(mpmath.mpf("-1.35e-3")),  # Almost minimum value to use Maclaurin series.
    (float.fromhex("-0x1.fd71823e9ed31p-28"), float.fromhex("0x1.456b15bb2ec98p-83")),
    (float.fromhex("0x1.3e3e59d300f44p-40"), float.fromhex("0x1.c41897769fec4p-94")),
    split_quad(mpmath.mpf("1.35e-3")),  # Almost maximum value to use Maclaurin series.
    (float.fromhex("0x1.e37bed2c3aa0bp+0"), float.fromhex("-0x1.e2d5f1238d4c0p-56")),
    (float.fromhex("0x1.5832e2e5d966bp+5"), float.fromhex("0x1.8794e6d1f4d78p-49")),
    (float.fromhex("0x1.15d8c5d3e4004p+9"), float.fromhex("-0x1.01f96f5f98c28p-45")),
    (709.0, 0.0),  # Almost maximum value
    # cspell: enable
]


def calculate_one(inp: tuple[float, float]) -> None:
    """Calculate one input.

    Args:
        inp (tuple[float, float]): Input values.
    """
    input_mpf = mpmath.mpf(inp[0]) + mpmath.mpf(inp[1])
    result_mpf = mpmath.extraprec(20, True)(mpmath.exp)(input_mpf)
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
