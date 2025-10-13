"""Calculate true values for pow(quad) function."""

import mpmath
from num_collect_test_utils.multi_double.split_quad import split_quad

mpmath.mp.prec = 52 * 2 + 1  # Quad precision

SAMPLES = [
    # cspell: disable
    (
        (
            float.fromhex("0x1.be580cb8d6672p+5"),
            float.fromhex("-0x1.7c1b0862537e4p-49"),
        ),
        (
            float.fromhex("0x1.7ff89514e4ba3p+2"),
            float.fromhex("0x1.d9043a8ea4840p-55"),
        ),
    ),
    (
        (
            float.fromhex("0x1.5a2b98fe9585cp-10"),
            float.fromhex("-0x1.0c36fa4e937a0p-66"),
        ),
        (
            float.fromhex("0x1.377eb204dfb45p+2"),
            float.fromhex("0x1.61c693b9f146cp-52"),
        ),
    ),
    (
        (
            float.fromhex("0x1.abf6487b5305fp+5"),
            float.fromhex("0x1.924c526aeb870p-50"),
        ),
        (
            float.fromhex("0x1.5b7d6d2e1b674p-2"),
            float.fromhex("0x1.79c41e9ec3e18p-56"),
        ),
    ),
    (
        (
            float.fromhex("0x1.5df1c02a3661fp-12"),
            float.fromhex("0x1.98c0818650188p-67"),
        ),
        (
            float.fromhex("-0x1.30a1b401d3ba5p+3"),
            float.fromhex("0x1.c85f223c97498p-52"),
        ),
    ),
    (
        (
            float.fromhex("0x1.17ce6ac42961cp-8"),
            float.fromhex("-0x1.070ccd508d778p-63"),
        ),
        (
            float.fromhex("-0x1.1f55dcd10d285p-3"),
            float.fromhex("-0x1.2c2ce0b511ca4p-57"),
        ),
    ),
    (
        (
            float.fromhex("0x1.6cda60be6b14fp-2"),
            float.fromhex("0x1.399372f983e40p-56"),
        ),
        (2.0, 0.0),
    ),
    (
        (
            float.fromhex("0x1.2c05b961881ddp+9"),
            float.fromhex("0x1.53c917c681b20p-45"),
        ),
        (100.0, 0.0),
    ),
    (
        (
            float.fromhex("0x1.d1b18f6ad52ffp-2"),
            float.fromhex("-0x1.4ec7fb6021200p-62"),
        ),
        (-300.0, 0.0),
    ),
    (
        (
            float.fromhex("0x1.dad80bdd56581p-10"),
            float.fromhex("-0x1.f97511af07900p-70"),
        ),
        (0.5, 0.0),
    ),
    (
        (
            float.fromhex("0x1.645eaa2f888ecp-7"),
            float.fromhex("0x1.11285a34d2f78p-62"),
        ),
        (0.0, 0.0),
    ),
    # cspell: enable
]


def calculate_one(a: tuple[float, float], b: tuple[float, float]) -> None:
    """Calculate one pair of inputs.

    Args:
        a (tuple[float, float]): The first input value.
        b (tuple[float, float]): The second input value.
    """
    a_mpf = mpmath.mpf(a[0]) + mpmath.mpf(a[1])
    b_mpf = mpmath.mpf(b[0]) + mpmath.mpf(b[1])
    summed = mpmath.power(a_mpf, b_mpf)
    upper, lower = split_quad(summed)
    print(
        f"std::make_tuple(quad({a[0].hex()}, {a[1].hex()}), "
        f"quad({b[0].hex()}, {b[1].hex()}), "
        f"quad({upper.hex()}, {lower.hex()})),"
    )


def main() -> None:
    """Calculate all pairs of inputs."""
    for a, b in SAMPLES:
        calculate_one(a, b)


if __name__ == "__main__":
    main()
