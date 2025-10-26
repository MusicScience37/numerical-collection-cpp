"""Calculate true values for tests of oct + oct operation."""

import mpmath
from num_collect_test_utils.multi_double.split_oct import split_oct

SAMPLES = [
    # cspell: disable
    (
        (
            float.fromhex("0x1.96179cb334bc3p-8"),
            float.fromhex("0x1.410aea50a8609p-63"),
            float.fromhex("-0x1.dc61650752178p-119"),
            float.fromhex("-0x1.c877d80cd5a00p-173"),
        ),
        (
            float.fromhex("0x1.a80aa52d84e0fp-32"),
            float.fromhex("0x1.9b82745fe1ae1p-89"),
            float.fromhex("0x1.98ae81a4ae4a7p-143"),
            float.fromhex("-0x1.65a21f469be00p-197"),
        ),
    ),
    (
        (
            float.fromhex("0x1.ac9e3ae780628p-13"),
            float.fromhex("-0x1.774892a4c52fap-68"),
            float.fromhex("-0x1.b780d62ef948fp-122"),
            float.fromhex("0x1.3700688b8ca80p-176"),
        ),
        (
            float.fromhex("0x1.33cc1fd9d87abp-12"),
            float.fromhex("0x1.37be720f182efp-67"),
            float.fromhex("-0x1.3da5094b40d91p-121"),
            float.fromhex("0x1.e86a168b51980p-177"),
        ),
    ),
    (
        (
            float.fromhex("0x1.7ef5f07995547p+3"),
            float.fromhex("-0x1.fb22a325fa712p-51"),
            float.fromhex("0x1.573318b2e8163p-107"),
            float.fromhex("-0x1.5b06031d45400p-163"),
        ),
        (
            float.fromhex("-0x1.ad9874de18cdcp+37"),
            float.fromhex("-0x1.3e0eb7f68d4ffp-17"),
            float.fromhex("0x1.440e7cdebbc48p-71"),
            float.fromhex("-0x1.4361d93087ed0p-125"),
        ),
    ),
    # cspell: enable
]


mpmath.mp.prec = 53 * 4 - 1  # Octuple precision


def calculate_one(
    a: tuple[float, float, float, float], b: tuple[float, float, float, float]
) -> None:
    """Calculate one pair of inputs.

    Args:
        a (tuple[float, float, float, float]): The first input value.
        b (tuple[float, float, float, float]): The second input value.
    """
    a_mpf = mpmath.mpf(a[0]) + mpmath.mpf(a[1]) + mpmath.mpf(a[2]) + mpmath.mpf(a[3])
    b_mpf = mpmath.mpf(b[0]) + mpmath.mpf(b[1]) + mpmath.mpf(b[2]) + mpmath.mpf(b[3])
    result = a_mpf + b_mpf
    result0, result1, result2, result3 = split_oct(result)
    print(
        f"std::make_tuple("
        f"oct({a[0].hex()}, {a[1].hex()}, {a[2].hex()}, {a[3].hex()}), "
        f"oct({b[0].hex()}, {b[1].hex()}, {b[2].hex()}, {b[3].hex()}), "
        f"oct({result0.hex()}, {result1.hex()}, {result2.hex()}, {result3.hex()})),"
    )


def main() -> None:
    """Calculate all pairs of inputs."""
    for a, b in SAMPLES:
        calculate_one(a, b)


if __name__ == "__main__":
    main()
