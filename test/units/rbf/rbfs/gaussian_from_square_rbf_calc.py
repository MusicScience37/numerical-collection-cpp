"""Script to calculate true values for test of gaussian_from_square_rbf class."""

import mpmath


def main() -> None:
    """Main function."""
    mpmath.mp.dps = 100

    distance_rate_str_list = [
        "0.0",
        "1e-6",
        "0.099",
        "0.1",
        "0.101",
        "0.5",
        "0.9",
        "1.0",
        "1.5",
    ]
    for distance_rate_str in distance_rate_str_list:
        distance_rate = mpmath.mpf(distance_rate_str)
        value = mpmath.exp(-(distance_rate**2)) - mpmath.mpf(1) + distance_rate**2
        print(f"rbf({distance_rate_str}) = {mpmath.nstr(value, 16)}")

    for distance_rate_str in distance_rate_str_list:
        distance_rate = mpmath.mpf(distance_rate_str)
        value = mpmath.mpf(2) * mpmath.expm1(-(distance_rate**2))
        print(f"differentiated_rbf({distance_rate_str}) = {mpmath.nstr(value, 16)}")


if __name__ == "__main__":
    main()
