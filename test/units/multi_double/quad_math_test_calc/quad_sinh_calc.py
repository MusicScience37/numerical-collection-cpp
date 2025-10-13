"""Calculate true values for tests of sinh(quad) function."""

import mpmath
from num_collect_test_utils.multi_double.split_quad import split_quad

mpmath.mp.prec = 52 * 2 + 1  # Quad precision

SAMPLES = [
    # cspell: disable
    (float.fromhex("-0x1.86b257fa21711p+8"), float.fromhex("-0x1.2a5a2e167baa8p-46")),
    (float.fromhex("-0x1.a96c8c1fe280ap-3"), float.fromhex("0x1.1deefd798cca0p-59")),
    (float.fromhex("-0x1.0a98bfb3fdaf8p-22"), float.fromhex("-0x1.6b4e354230ce8p-76")),
    (float.fromhex("-0x1.04824626c4105p-48"), float.fromhex("0x1.1e4507ff1e4b8p-103")),
    (float.fromhex("0x1.6e9219342dc4dp-49"), float.fromhex("-0x1.e01ec099a42d8p-103")),
    (float.fromhex("0x1.db931072e257dp-10"), float.fromhex("-0x1.cc1d6d3cdf740p-68")),
    (float.fromhex("0x1.0dd4be2b57836p-1"), float.fromhex("0x1.1326afb4ab2e0p-58")),
    (float.fromhex("0x1.8ff7def2ecb82p+7"), float.fromhex("0x1.5e17780a516e0p-47")),
    # cspell: enable
]


def calculate_one(inp: tuple[float, float]) -> None:
    """Calculate one input.

    Args:
        inp (tuple[float, float]): Input values.
    """
    input_mpf = mpmath.mpf(inp[0]) + mpmath.mpf(inp[1])
    result_mpf = mpmath.extraprec(20, True)(mpmath.sinh)(input_mpf)
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
