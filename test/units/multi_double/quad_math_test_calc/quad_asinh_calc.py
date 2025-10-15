"""Calculate true values for tests of asinh(quad) function."""

import mpmath
from num_collect_test_utils.multi_double.split_quad import split_quad

mpmath.mp.prec = 52 * 2 + 1  # Quad precision

SAMPLES = [
    # cspell: disable
    (float.fromhex("-0x1.1ecf1dae30939p+39"), float.fromhex("-0x1.509a5c491b400p-20")),
    (float.fromhex("-0x1.13e2f13993d5ap+3"), float.fromhex("-0x1.183a1b317b3c0p-53")),
    (float.fromhex("-0x1.0759396169785p-11"), float.fromhex("0x1.f5ecdb3ae8adcp-65")),
    (float.fromhex("-0x1.68e5691a87c5ep-32"), float.fromhex("0x1.b27f0efafe080p-89")),
    (float.fromhex("0x1.44bcc8101fba9p-25"), float.fromhex("0x1.3a48d94efba60p-82")),
    (float.fromhex("0x1.dcd48122989b0p-9"), float.fromhex("-0x1.b596773838700p-68")),
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
    result_mpf = mpmath.extraprec(20, True)(mpmath.asinh)(input_mpf)
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
