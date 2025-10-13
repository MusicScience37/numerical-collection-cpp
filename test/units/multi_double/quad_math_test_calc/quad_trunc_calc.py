"""Calculate true values for tests of trunc(quad) function."""

import mpmath
from num_collect_test_utils.multi_double.split_quad import split_quad
from num_collect_test_utils.multi_double.trunc import trunc

mpmath.mp.prec = 52 * 2 + 1  # Quad precision

SAMPLES = [
    # cspell: disable
    (float.fromhex("-0x1.1e7c7065eea5ep+112"), float.fromhex("0x1.6dcd37ee57648p+58")),
    (float.fromhex("-0x1.28f419c8f2c9dp+75"), float.fromhex("0x1.42d614c040accp+21")),
    (float.fromhex("-0x1.112fc50d218b7p+35"), float.fromhex("-0x1.5625eaa60b200p-23")),
    (float.fromhex("-0x1.1ac021183b471p-43"), float.fromhex("0x1.ccdb580b5519cp-97")),
    (float.fromhex("0x1.762890cd17ad9p-15"), float.fromhex("-0x1.fa19057de5c00p-71")),
    (float.fromhex("0x1.1ad0d72802619p+8"), float.fromhex("-0x1.697d2b1918d60p-46")),
    (float.fromhex("0x1.05fbaf0942101p+70"), float.fromhex("0x1.14570a7aa6020p+16")),
    (float.fromhex("0x1.661bd7be881a1p+115"), float.fromhex("0x1.15d90446d0dc0p+59")),
    # cspell: enable
]


def calculate_one(inp: tuple[float, float]) -> None:
    """Calculate one input.

    Args:
        inp (tuple[float, float]): Input values.
    """
    input_mpf = mpmath.mpf(inp[0]) + mpmath.mpf(inp[1])
    result_mpf = mpmath.extraprec(20, True)(trunc)(input_mpf)
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
