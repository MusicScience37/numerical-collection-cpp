"""Calculate true values for test of root_of_one_plus_square function."""

import mpmath
from num_collect_test_utils.multi_double.split_quad import split_quad

mpmath.mp.prec = 52 * 2 + 1  # Quad precision

SAMPLES = [
    # cspell: disable
    (float.fromhex("0x1.766868f7c0304p-25"), float.fromhex("-0x1.f8e07514ac7d8p-80")),
    (float.fromhex("0x1.05f644428982cp+4"), float.fromhex("-0x1.8e5f1b1bdb518p-51")),
    (float.fromhex("0x1.aead8b8a75c89p+10"), float.fromhex("0x1.a966b8ab353d0p-44")),
    (float.fromhex("0x1.4470bceaffb8dp+44"), float.fromhex("-0x1.33fdd08dd5560p-11")),
    # cspell: enable
]


def root_of_one_plus_square(x):
    """Calculate the root of one plus square.

    Args:
        x: Input value.

    Returns:
        The root of one plus square.
    """
    return mpmath.sqrt(1 + x * x)


def calculate_one(inp: tuple[float, float]) -> None:
    """Calculate one input.

    Args:
        inp (tuple[float, float]): Input values.
    """
    input_mpf = mpmath.mpf(inp[0]) + mpmath.mpf(inp[1])
    result_mpf = mpmath.extraprec(200, True)(root_of_one_plus_square)(input_mpf)
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
