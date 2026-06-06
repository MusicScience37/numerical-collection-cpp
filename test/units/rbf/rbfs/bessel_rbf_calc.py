"""Script to calculate true values of Bessel RBFs."""

import click
import mpmath


@click.group()
def cli() -> None:
    """Calculate true values of Bessel RBFs."""


DISTANCE_RATE_STR_LIST = [
    "0.0",
    "1e-6",
    "0.1",
    "0.4",
    "1.23",
    "4.56",
    "7.89",
]

mpmath.mp.dps = 100
SMALL_NUMBER = mpmath.mpf("1e-50")


def calculate_bessel_rbf(dimension: int, distance_rate: mpmath.mpf) -> mpmath.mpf:
    """Calculate value of Bessel RBF."""
    order = float(dimension) / 2 - 1
    if distance_rate < SMALL_NUMBER:
        return mpmath.besselj(order, SMALL_NUMBER) / mpmath.power(SMALL_NUMBER, order)
    return mpmath.besselj(order, distance_rate) / mpmath.power(distance_rate, order)


@cli.command()
@click.argument("dimension", type=int)
def rbf(dimension: int) -> None:
    """Calculate true values of Bessel RBFs."""
    for distance_rate_str in DISTANCE_RATE_STR_LIST:
        distance_rate = mpmath.mpf(distance_rate_str)
        value = calculate_bessel_rbf(dimension, distance_rate)
        print(f"std::make_tuple({distance_rate_str}, {mpmath.nstr(value, n=16)}),")


if __name__ == "__main__":
    cli()
