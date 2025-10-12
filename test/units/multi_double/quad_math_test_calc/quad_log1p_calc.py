"""Calculate true values for tests of log1p(quad) function."""

import mpmath
from num_collect_test_utils.multi_double.split_quad import split_quad

mpmath.mp.prec = 52 * 2 + 1  # Quad precision

SAMPLES = [
    # cspell: disable
    (float.fromhex("-0x1.0502c4a06925ep-10"), float.fromhex("-0x1.958f20a5e1030p-64")),
    (float.fromhex("-0x1.eb9d5a6f5e582p-13"), float.fromhex("-0x1.3ce4a88bcd750p-69")),
    (float.fromhex("0x1.800465b45ad46p-48"), float.fromhex("0x1.a273de5b32fd8p-103")),
    (float.fromhex("0x1.11e0a04f502acp-9"), float.fromhex("0x1.f80d9d284506cp-63")),
    (float.fromhex("0x1.067ec9978934cp+15"), float.fromhex("-0x1.c28e47aebe040p-39")),
    # cspell: enable
]


def calculate_one(inp: tuple[float, float]) -> None:
    """Calculate one input.

    Args:
        inp (tuple[float, float]): Input values.
    """
    input_mpf = mpmath.mpf(inp[0]) + mpmath.mpf(inp[1])
    result_mpf = mpmath.extraprec(20, True)(mpmath.log1p)(input_mpf)
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
