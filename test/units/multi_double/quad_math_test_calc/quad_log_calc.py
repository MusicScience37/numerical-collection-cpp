"""Calculate true values for tests of log(quad) function."""

import mpmath
from num_collect_test_utils.multi_double.split_quad import split_quad

mpmath.mp.prec = 52 * 2 + 1  # Quad precision

SAMPLES = [
    # cspell: disable
    (float.fromhex("0x1.01c016cf3b315p-29"), float.fromhex("-0x1.ec067815a7138p-84")),
    (float.fromhex("0x1.197e3eb11c9f1p-11"), float.fromhex("0x1.600b1b3bfbf70p-65")),
    (float.fromhex("0x1.1d050b4ff7bbfp-2"), float.fromhex("-0x1.5cd3ea0636098p-57")),
    (float.fromhex("0x1.5957a572db657p+1"), float.fromhex("-0x1.ba4f2cdf77ab8p-53")),
    (float.fromhex("0x1.ec8b6be14f39fp+19"), float.fromhex("0x1.780a9e47ae320p-35")),
    (float.fromhex("0x1.ab2d7adaa1156p+33"), float.fromhex("-0x1.a5b71b73af190p-23")),
    # cspell: enable
]


def calculate_one(inp: tuple[float, float]) -> None:
    """Calculate one input.

    Args:
        inp (tuple[float, float]): Input values.
    """
    input_mpf = mpmath.mpf(inp[0]) + mpmath.mpf(inp[1])
    result_mpf = mpmath.extraprec(20, True)(mpmath.log)(input_mpf)
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
