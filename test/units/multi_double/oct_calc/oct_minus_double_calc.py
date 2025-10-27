"""Calculate true values for tests of oct - double operation."""

import mpmath
from num_collect_test_utils.multi_double.split_oct import split_oct
from oct_plus_double_calc import SAMPLES

mpmath.mp.prec = 53 * 4 - 1  # Octuple precision


def calculate_one(a: tuple[float, float, float, float], b: float) -> None:
    """Calculate one pair of inputs.

    Args:
        a (tuple[float, float, float, float]): The first input value.
        b (float): The second input value.
    """
    a_mpf = mpmath.mpf(a[0]) + mpmath.mpf(a[1]) + mpmath.mpf(a[2]) + mpmath.mpf(a[3])
    b_mpf = mpmath.mpf(b)
    result = a_mpf - b_mpf
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
