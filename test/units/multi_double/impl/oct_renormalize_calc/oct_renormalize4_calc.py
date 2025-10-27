"""Calculate true values for tests of oct_renormalize function with 4 arguments."""

import mpmath
from num_collect_test_utils.multi_double.split_oct import split_oct

SAMPLES = [
    (
        float.fromhex("0x1.a45f4d3ecd99ap+28"),
        float.fromhex("0x1.9a21dc10c2ea5p-22"),
        float.fromhex("0x1.a1bf18db488afp-72"),
        float.fromhex("0x1.c36b2a2df1c6fp-122"),
    ),
]

mpmath.mp.prec = 53 * 4 - 1  # Octuple precision


def calculate_one(a0: float, a1: float, a2: float, a3: float) -> None:
    """Calculate one set of inputs.

    Args:
        a0 (float): An input value.
        a1 (float): An input value.
        a2 (float): An input value.
        a3 (float): An input value.
    """
    result = mpmath.mpf(a0) + mpmath.mpf(a1) + mpmath.mpf(a2) + mpmath.mpf(a3)
    result0, result1, result2, result3 = split_oct(result)
    print(
        # pylint: disable=line-too-long
        f"std::make_tuple(std::to_array<double>({{{a0.hex()}, {a1.hex()}, {a2.hex()}, {a3.hex()}}}), "
        f"std::to_array<double>({{{result0.hex()}, {result1.hex()}, {result2.hex()}, {result3.hex()}}})),"
    )


def main() -> None:
    """Calculate all pairs of inputs."""
    for inp in SAMPLES:
        calculate_one(*inp)


if __name__ == "__main__":
    main()
