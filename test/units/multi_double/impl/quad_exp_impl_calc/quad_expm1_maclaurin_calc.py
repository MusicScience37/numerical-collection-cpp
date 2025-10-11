"""Calculate true values for tests of expm1(quad) function."""

import mpmath
from num_collect_test_utils.multi_double.split_quad import split_quad

mpmath.mp.prec = 52 * 2 + 1  # Quad precision

SAMPLES = [
    # cspell: disable
    split_quad(mpmath.mpf("-1.36e-3")),  # Minimum value to use Maclaurin series.
    (float.fromhex("-0x1.fd71823e9ed31p-28"), float.fromhex("0x1.456b15bb2ec98p-83")),
    (float.fromhex("-0x1.cdc648a9cfaeap-15"), float.fromhex("0x1.8963ce8d06dd0p-69")),
    (float.fromhex("0x1.3e3e59d300f44p-40"), float.fromhex("0x1.c41897769fec4p-94")),
    (float.fromhex("0x1.8779b52a9a07bp-18"), float.fromhex("0x1.c9fb9fa077a00p-76")),
    split_quad(mpmath.mpf("1.36e-3")),  # Maximum value to use Maclaurin series.
    # cspell: enable
]


def calculate_one(inp: tuple[float, float]) -> None:
    """Calculate one input.

    Args:
        inp (tuple[float, float]): Input values.
    """
    input_mpf = mpmath.mpf(inp[0]) + mpmath.mpf(inp[1])
    result_mpf = mpmath.extraprec(20, True)(mpmath.expm1)(input_mpf)
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
