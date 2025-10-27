"""Calculate true values for tests of oct + double operation."""

import mpmath
from num_collect_test_utils.multi_double.split_oct import split_oct

SAMPLES = [
    # cspell: disable
    (
        (
            float.fromhex("0x1.831a1fd9362bfp+18"),
            float.fromhex("0x1.45e43dd2979ccp-38"),
            float.fromhex("0x1.8f2b9eb096b5ap-92"),
            float.fromhex("0x1.e045cf7aea680p-146"),
        ),
        float.fromhex("0x1.de60caa1ccaf4p+21"),
    ),
    (
        (
            float.fromhex("0x1.98f107a10da81p-42"),
            float.fromhex("-0x1.434beb493c597p-96"),
            float.fromhex("-0x1.0bb9c5b4a0147p-153"),
            float.fromhex("0x1.769dcaccd9a00p-209"),
        ),
        float.fromhex("-0x1.3b3dcd03b2853p+37"),
    ),
    (
        (
            float.fromhex("-0x1.54d3750b03757p+23"),
            float.fromhex("0x1.2493388414c18p-31"),
            float.fromhex("-0x1.14963b4238dfap-86"),
            float.fromhex("0x1.ec663e0118a00p-140"),
        ),
        float.fromhex("0x1.13e1e3e291a63p-22"),
    ),
    (
        (
            float.fromhex("-0x1.cf6d0948a67a1p-18"),
            float.fromhex("-0x1.e0e9f6aaefb02p-73"),
            float.fromhex("0x1.4c667f7e170e1p-127"),
            float.fromhex("-0x1.b603698a5a720p-181"),
        ),
        float.fromhex("-0x1.565a87f6ed41fp-31"),
    ),
]

mpmath.mp.prec = 53 * 4 - 1  # Octuple precision


def calculate_one(a: tuple[float, float, float, float], b: float) -> None:
    """Calculate one pair of inputs.

    Args:
        a (tuple[float, float, float, float]): The first input value.
        b (float): The second input value.
    """
    a_mpf = mpmath.mpf(a[0]) + mpmath.mpf(a[1]) + mpmath.mpf(a[2]) + mpmath.mpf(a[3])
    b_mpf = mpmath.mpf(b)
    result = a_mpf + b_mpf
    result0, result1, result2, result3 = split_oct(result)
    print(
        f"std::make_tuple("
        f"oct({a[0].hex()}, {a[1].hex()}, {a[2].hex()}, {a[3].hex()}), "
        f"{b.hex()}, "
        f"oct({result0.hex()}, {result1.hex()}, {result2.hex()}, {result3.hex()})),"
    )


def main() -> None:
    """Calculate all pairs of inputs."""
    for a, b in SAMPLES:
        calculate_one(a, b)


if __name__ == "__main__":
    main()
