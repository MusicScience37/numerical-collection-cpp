"""Calculate true values for atan2(quad, quad) function."""

import mpmath
from num_collect_test_utils.multi_double.split_quad import split_quad

mpmath.mp.prec = 52 * 2 + 1  # Quad precision

# List of (x, y).
SAMPLES = [
    # cspell: disable
    # First quadrant.
    (
        (
            float.fromhex("0x1.6bc343cde7d78p+4"),
            float.fromhex("-0x1.6ec624ec8377cp-50"),
        ),
        (
            float.fromhex("0x1.bdc89c1b355f6p+5"),
            float.fromhex("0x1.4b1291c09af00p-53"),
        ),
    ),
    (
        (
            float.fromhex("0x1.3b807cf424316p+5"),
            float.fromhex("0x1.dbef1155dcc00p-53"),
        ),
        (
            float.fromhex("0x1.31de8194c625ep-3"),
            float.fromhex("-0x1.e355f96cf3440p-57"),
        ),
    ),
    # Second quadrant.
    (
        (
            float.fromhex("-0x1.ef79cb69682b3p+4"),
            float.fromhex("-0x1.baf6037aca920p-50"),
        ),
        (
            float.fromhex("0x1.2e413fa320922p-26"),
            float.fromhex("0x1.bc40a8e9b8dc0p-80"),
        ),
    ),
    (
        (
            float.fromhex("-0x1.f83d67f10f754p-9"),
            float.fromhex("-0x1.336e7c43305a0p-63"),
        ),
        (
            float.fromhex("0x1.41e44f511915ep+2"),
            float.fromhex("0x1.7ec6660534cf0p-52"),
        ),
    ),
    # Third quadrant.
    (
        (
            float.fromhex("-0x1.caabfff431e59p-1"),
            float.fromhex("-0x1.45d69a833ede8p-55"),
        ),
        (
            float.fromhex("-0x1.ceffedf90709bp-39"),
            float.fromhex("0x1.73d1b3e7f3b38p-93"),
        ),
    ),
    (
        (
            float.fromhex("-0x1.44c1e07279ba0p+8"),
            float.fromhex("0x1.edf61195839d0p-46"),
        ),
        (
            float.fromhex("-0x1.cd4129453a9eep+23"),
            float.fromhex("-0x1.a2e5334833370p-32"),
        ),
    ),
    # Fourth quadrant.
    (
        (
            float.fromhex("0x1.67dcf604801f0p-24"),
            float.fromhex("0x1.eb129515c9954p-78"),
        ),
        (
            float.fromhex("-0x1.613d97a2deb7ep-26"),
            float.fromhex("-0x1.782271675e068p-80"),
        ),
    ),
    (
        (
            float.fromhex("0x1.89c0e175ffb91p+6"),
            float.fromhex("-0x1.fd08ee8c9f4acp-48"),
        ),
        (
            float.fromhex("-0x1.9d56a4d62f911p+19"),
            float.fromhex("-0x1.6553801ca5aa0p-36"),
        ),
    ),
    # cspell: enable
]


def calculate_one(x: tuple[float, float], y: tuple[float, float]) -> None:
    """Calculate one pair of inputs.

    Args:
        x (tuple[float, float]): The x input value.
        y (tuple[float, float]): The y input value.
    """
    x_mpf = mpmath.mpf(x[0]) + mpmath.mpf(x[1])
    y_mpf = mpmath.mpf(y[0]) + mpmath.mpf(y[1])
    result_mpf = mpmath.atan2(y_mpf, x_mpf)
    upper, lower = split_quad(result_mpf)
    print(
        f"std::make_tuple(quad({x[0].hex()}, {x[1].hex()}), "
        f"quad({y[0].hex()}, {y[1].hex()}), "
        f"quad({upper.hex()}, {lower.hex()})),"
    )


def main() -> None:
    """Calculate all pairs of inputs."""
    for x, y in SAMPLES:
        calculate_one(x, y)


if __name__ == "__main__":
    main()
