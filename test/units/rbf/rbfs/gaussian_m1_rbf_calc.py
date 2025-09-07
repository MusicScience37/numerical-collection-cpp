"""Script to calculate true values for test of gaussian_m1_rbf class."""

import mpmath


def main() -> None:
    """Main function."""
    mpmath.mp.dps = 50

    distance_rate_str_list = [
        "0.0",
        "1e-6",
        "0.1",
        "0.5",
        "0.9",
        "1.0",
        "1.5",
    ]
    for distance_rate_str in distance_rate_str_list:
        value = mpmath.exp(-mpmath.mpf(distance_rate_str) ** 2) - mpmath.mpf(1)
        print(f"rbf({distance_rate_str}) = {mpmath.nstr(value, 16)}")


if __name__ == "__main__":
    main()
