"""Calculate true values for tests of sqrt(quad) function."""

import mpmath
from num_collect_test_utils.multi_double.split_quad import split_quad

SAMPLES = [
    # cspell: disable
    (1.0, 0.0),
    (2.0, 0.0),
    (float.fromhex("0x1.1a651c97cd404p-22"), float.fromhex("-0x1.dfa323ed68808p-76")),
    ((float.fromhex("0x1.10c1e666a56e0p+0"), float.fromhex("-0x1.f42263d9ec5c0p-57"))),
    (float.fromhex("0x1.2c6adf012f63ap+27"), float.fromhex("0x1.7f94bdac919b8p-27")),
]

mpmath.mp.prec = 52 * 2 + 1  # Quad precision


def calculate_one(inp: tuple[float, float]) -> None:
    """Calculate one input.

    Args:
        inp (tuple[float, float]): Input values.
    """
    input_mpf = mpmath.mpf(inp[0]) + mpmath.mpf(inp[1])
    result_mpf = mpmath.extraprec(20, True)(mpmath.sqrt)(input_mpf)
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
