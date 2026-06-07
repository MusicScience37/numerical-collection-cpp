"""Script to calculate true values of Matern RBFs."""

import click
import mpmath


@click.group()
def cli() -> None:
    """Calculate true values of Matern RBFs."""


DISTANCE_RATE_STR_LIST = [
    "0.0",
    "1e-6",
    "1e-5",
    "0.1",
    "0.4",
    "1.23",
    "4.56",
    "7.89",
]

mpmath.mp.dps = 100
SMALL_NUMBER = mpmath.mpf("1e-50")


def calculate_matern_rbf(order: float, distance_rate: mpmath.mpf) -> mpmath.mpf:
    """Calculate value of Matern RBF."""
    distance_rate = max(distance_rate, SMALL_NUMBER)
    return (
        (mpmath.power(2, 1 - order) / mpmath.gamma(order))
        * mpmath.power(distance_rate, order)
        * mpmath.besselk(order, distance_rate)
    )


@cli.command()
@click.argument("order", type=float)
def rbf(order: float) -> None:
    """Calculate true values of Matern RBFs."""
    for distance_rate_str in DISTANCE_RATE_STR_LIST:
        distance_rate = mpmath.mpf(distance_rate_str)
        value = calculate_matern_rbf(order, distance_rate)
        print(f"std::make_tuple({distance_rate_str}, {mpmath.nstr(value, n=16)}),")


if __name__ == "__main__":
    cli()
