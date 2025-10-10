"""Calculate true values for test of quad + double operation."""

import mpmath
from num_collect_test_utils.multi_double.split_quad import split_quad

SAMPLES = [
    # cspell: disable
    (
        (
            float.fromhex("0x1.58645c21bbf0ep-16"),
            float.fromhex("-0x1.647a8b7ec08c8p-71"),
        ),
        float.fromhex("0x1.c17cdbf807483p+16"),
    ),
    (
        (
            float.fromhex("-0x1.7986766143f6ap+0"),
            float.fromhex("0x1.19615589dc270p-56"),
        ),
        float.fromhex("0x1.4e79b4fedfba5p-31"),
    ),
    (
        (
            float.fromhex("0x1.fd5d247a03357p-30"),
            float.fromhex("-0x1.09e10a59a3ca8p-85"),
        ),
        float.fromhex("-0x1.5a195eb27320ap+24"),
    ),
    (
        (
            float.fromhex("0x1.57b12ae1c238dp-25"),
            float.fromhex("-0x1.d8caf920c9978p-80"),
        ),
        float.fromhex("0x1.1a52be8fe7d1ap+24"),
    ),
    # cspell: enable
]

mpmath.mp.prec = 52 * 2 + 1  # Quad precision


def calculate_one(a: tuple[float, float], b: float) -> None:
    """Calculate one pair of inputs.

    Args:
        a (tuple[float, float]): The first input value.
        b (float): The second input value.
    """
    a_mpf = mpmath.mpf(a[0]) + mpmath.mpf(a[1])
    b_mpf = mpmath.mpf(b)
    summed = a_mpf + b_mpf
    upper, lower = split_quad(summed)
    print(
        f"a=({a[0].hex()}, {a[1].hex()})\n"
        f"b={b.hex()}\n"
        f"upper={upper.hex()}\n"
        f"lower={lower.hex()}\n"
        f"tuple=(quad({a[0].hex()}, {a[1].hex()}), {b.hex()}, "
        f"quad({upper.hex()}, {lower.hex()}))\n",
    )


def main() -> None:
    """Calculate all pairs of inputs."""
    for a, b in SAMPLES:
        calculate_one(a, b)


if __name__ == "__main__":
    main()
