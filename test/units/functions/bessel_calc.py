"""Script to calculate the true values for tests of Bessel functions."""

import click
import scipy.special


@click.group()
def cli() -> None:
    """Script to calculate the true values for tests of Bessel functions."""


@cli.command()
def cyl_bessel_j() -> None:
    """Generate tests for cyl_bessel_j."""
    inputs = [
        (0.0, 0.0),
        (0.0, 0.001),
        (0.0, 0.3),
        (0.0, 1.0),
        (0.0, 3.0),
        (0.0, 10.0),
        (1.0, 0.0),
        (1.0, 0.001),
        (1.0, 0.3),
        (1.0, 1.0),
        (1.0, 3.0),
        (1.0, 10.0),
        (7.0, 3.0),
        (1.2, 0.34),
    ]

    for nu, x in inputs:
        value = scipy.special.jv(nu, x)
        print(f"CHECK_BESSEL_J({nu}, {x}, {value});")


@cli.command()
def cyl_neumann() -> None:
    """Generate tests for cyl_neumann."""
    inputs = [
        (0.0, 0.001),
        (0.0, 0.3),
        (0.0, 1.0),
        (0.0, 3.0),
        (0.0, 10.0),
        (1.0, 0.001),
        (1.0, 0.3),
        (1.0, 1.0),
        (1.0, 3.0),
        (1.0, 10.0),
        (7.0, 3.0),
        (1.2, 0.34),
    ]

    for nu, x in inputs:
        value = scipy.special.yv(nu, x)
        print(f"CHECK_NEUMANN({nu}, {x}, {value});")


@cli.command()
def cyl_bessel_i() -> None:
    """Generate tests for cyl_bessel_i."""
    inputs = [
        (0.0, 0.0),
        (0.0, 0.001),
        (0.0, 0.3),
        (0.0, 1.0),
        (0.0, 3.0),
        (0.0, 10.0),
        (1.0, 0.0),
        (1.0, 0.001),
        (1.0, 0.3),
        (1.0, 1.0),
        (1.0, 3.0),
        (1.0, 10.0),
        (7.0, 3.0),
        (1.2, 0.34),
    ]

    for nu, x in inputs:
        value = scipy.special.iv(nu, x)
        print(f"CHECK_BESSEL_I({nu}, {x}, {value});")


@cli.command()
def cyl_bessel_k() -> None:
    """Generate tests for cyl_bessel_k."""
    inputs = [
        (0.0, 0.001),
        (0.0, 0.3),
        (0.0, 1.0),
        (0.0, 3.0),
        (0.0, 10.0),
        (1.0, 0.001),
        (1.0, 0.3),
        (1.0, 1.0),
        (1.0, 3.0),
        (1.0, 10.0),
        (7.0, 3.0),
        (1.2, 0.34),
    ]

    for nu, x in inputs:
        value = scipy.special.kv(nu, x)
        print(f"CHECK_BESSEL_K({nu}, {x}, {value});")


if __name__ == "__main__":
    cli()
