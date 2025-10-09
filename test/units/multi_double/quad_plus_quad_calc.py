"""Calculate true values for tests of quad + quad operation."""

import mpmath
from quad_test_utils import split

SAMPLES = [
    # cspell: disable
    (
        (
            float.fromhex("0x1.52b527b6c46a6p-17"),
            float.fromhex("0x1.fb1f8716820c8p-71"),
        ),
        (
            float.fromhex("0x1.3735a527aa3a0p-8"),
            float.fromhex("0x1.a73277b5f0aa0p-65"),
        ),
    ),
    (
        (
            float.fromhex("0x1.15c15c22faf99p+7"),
            float.fromhex("0x1.eaf4b47590204p-47"),
        ),
        (
            float.fromhex("-0x1.ae523e28a9262p-24"),
            float.fromhex("+0x1.2b1017f83acb8p-79"),
        ),
    ),
    (
        (
            float.fromhex("-0x1.1246ca6607d7ep-47"),
            float.fromhex("-0x1.42dce60279808p-102"),
        ),
        (
            float.fromhex("0x1.323833e7757d9p+30"),
            float.fromhex("0x1.5b3f99313c7a0p-26"),
        ),
    ),
    (
        (
            float.fromhex("-0x1.cdfd31e1eeaafp+14"),
            float.fromhex("0x1.6fa0de42b0780p-43"),
        ),
        (
            float.fromhex("-0x1.582805a6a9cfcp+5"),
            float.fromhex("0x1.1c37331fcfdacp-49"),
        ),
    ),
    # cspell: enable
]

mpmath.mp.prec = 52 * 2 + 1  # Quad precision


def calculate_one(a: tuple[float, float], b: tuple[float, float]) -> None:
    """Calculate one pair of inputs.

    Args:
        a (tuple[float, float]): The first input value.
        b (tuple[float, float]): The second input value.
    """
    a_mpf = mpmath.mpf(a[0]) + mpmath.mpf(a[1])
    b_mpf = mpmath.mpf(b[0]) + mpmath.mpf(b[1])
    summed = a_mpf + b_mpf
    upper, lower = split(summed)
    print(
        f"a=({a[0].hex()}, {a[1].hex()})\n"
        f"b=({b[0].hex()}, {b[1].hex()})\n"
        f"upper={upper.hex()}\n"
        f"lower={lower.hex()}\n"
        f"tuple=(quad({a[0].hex()}, {a[1].hex()}), quad({b[0].hex()}, {b[1].hex()}), "
        f"quad({upper.hex()}, {lower.hex()}))\n",
    )


def main() -> None:
    """Calculate all pairs of inputs."""
    for a, b in SAMPLES:
        calculate_one(a, b)


if __name__ == "__main__":
    main()
