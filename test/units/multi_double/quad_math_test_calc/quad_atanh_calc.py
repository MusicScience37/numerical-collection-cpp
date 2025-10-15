"""Calculate true values for tests of atanh(quad) function."""

import mpmath
from num_collect_test_utils.multi_double.split_quad import split_quad

mpmath.mp.prec = 52 * 2 + 1  # Quad precision

SAMPLES = [
    # cspell: disable
    (float.fromhex("-0x1.eecc475c72064p-1"), float.fromhex("0x1.52e3fa5837064p-55")),
    (float.fromhex("-0x1.63a7b3aa9fe62p-1"), float.fromhex("0x1.6a76ccde58c50p-56")),
    (float.fromhex("-0x1.19c4d7a962942p-7"), float.fromhex("-0x1.34c3b51ec3a60p-62")),
    (float.fromhex("-0x1.d7b5b86e430bep-33"), float.fromhex("0x1.c0648d22baa00p-91")),
    (float.fromhex("0x1.c9e645feeaceep-48"), float.fromhex("0x1.be9b281dbd348p-102")),
    (float.fromhex("0x1.6bfc419161f86p-26"), float.fromhex("-0x1.7e6aa79445bd0p-81")),
    (float.fromhex("0x1.5635d7afaf2cep-1"), float.fromhex("0x1.2e624f2d1fd50p-55")),
    (float.fromhex("0x1.b450f9a197e92p-1"), float.fromhex("0x1.0a9fbc263c338p-55")),
    # cspell: enable
]


def calculate_one(inp: tuple[float, float]) -> None:
    """Calculate one input.

    Args:
        inp (tuple[float, float]): Input values.
    """
    input_mpf = mpmath.mpf(inp[0]) + mpmath.mpf(inp[1])
    result_mpf = mpmath.extraprec(20, True)(mpmath.atanh)(input_mpf)
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
