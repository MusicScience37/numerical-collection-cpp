"""Script to calculate true values of polyharmonic spline RBFs and its derivatives."""

import typing

import click
import mpmath


@click.group()
def cli() -> None:
    """Script to calculate true values of polyharmonic spline RBFs and its derivatives."""


DISTANCE_RATE_STR_LIST = [
    "0.0",
    "1e-6",
    "0.1",
    "0.9",
    "1.0",
    "1.23",
    "7.89",
]


mpmath.mp.dps = 100
SMALL_NUMBER = mpmath.mpf("1e-50")


def calculate_polyharmonic_spline_rbf(
    distance_rate: mpmath.mpf, degree: int
) -> mpmath.mpf:
    """Calculate value of polyharmonic spline RBF."""
    if degree % 2 == 1:
        # Odd degrees.
        return mpmath.mpf(distance_rate) ** mpmath.mpf(degree)

    # Even degrees.
    if distance_rate < SMALL_NUMBER:
        return mpmath.mpf(0)
    return mpmath.mpf(distance_rate) ** mpmath.mpf(degree) * mpmath.log(
        mpmath.mpf(distance_rate)
    )


@cli.command()
@click.argument("degree", type=int)
def rbf(degree: int) -> None:
    """Calculate true values of polyharmonic spline RBFs."""
    for distance_rate_str in DISTANCE_RATE_STR_LIST:
        distance_rate = mpmath.mpf(distance_rate_str)
        value = calculate_polyharmonic_spline_rbf(distance_rate, degree)
        print(f"std::make_tuple({distance_rate_str}, {mpmath.nstr(value, n=16)}),")


def approximate_derivative(
    function: typing.Callable[[mpmath.mpf], mpmath.mpf], point: mpmath.mpf
) -> mpmath.mpf:
    """Approximate the derivative of a function at a point."""
    step = mpmath.mpf("1e-15")
    return -(function(point + step) - function(point - step)) / (2 * step) / point


@cli.command()
@click.argument("degree", type=int)
def first(degree: int) -> None:
    """Calculate true values of the first derivative of polyharmonic spline RBFs."""
    for distance_rate_str in DISTANCE_RATE_STR_LIST:
        if distance_rate_str == "0.0":
            continue
        distance_rate = mpmath.mpf(distance_rate_str)
        value = approximate_derivative(
            lambda x: calculate_polyharmonic_spline_rbf(x, degree), distance_rate
        )
        print(f"std::make_tuple({distance_rate_str}, {mpmath.nstr(value, n=16)}),")


def approximate_second_derivative(
    function: typing.Callable[[mpmath.mpf], mpmath.mpf], point: mpmath.mpf
) -> mpmath.mpf:
    """Approximate the second derivative of a function at a point."""
    return approximate_derivative(lambda x: approximate_derivative(function, x), point)


@cli.command()
@click.argument("degree", type=int)
def second(degree: int) -> None:
    """Calculate true values of the second derivative of polyharmonic spline RBFs."""
    for distance_rate_str in DISTANCE_RATE_STR_LIST:
        if distance_rate_str == "0.0":
            continue
        distance_rate = mpmath.mpf(distance_rate_str)
        value = approximate_second_derivative(
            lambda x: calculate_polyharmonic_spline_rbf(x, degree), distance_rate
        )
        print(f"std::make_tuple({distance_rate_str}, {mpmath.nstr(value, n=16)}),")


def approximate_third_derivative(
    function: typing.Callable[[mpmath.mpf], mpmath.mpf], point: mpmath.mpf
) -> mpmath.mpf:
    """Approximate the third derivative of a function at a point."""
    return approximate_derivative(
        lambda x: approximate_second_derivative(function, x), point
    )


@cli.command()
@click.argument("degree", type=int)
def third(degree: int) -> None:
    """Calculate true values of the third derivative of polyharmonic spline RBFs."""
    for distance_rate_str in DISTANCE_RATE_STR_LIST:
        if distance_rate_str == "0.0":
            continue
        distance_rate = mpmath.mpf(distance_rate_str)
        value = approximate_third_derivative(
            lambda x: calculate_polyharmonic_spline_rbf(x, degree), distance_rate
        )
        print(f"std::make_tuple({distance_rate_str}, {mpmath.nstr(value, n=16)}),")


def approximate_fourth_derivative(
    function: typing.Callable[[mpmath.mpf], mpmath.mpf], point: mpmath.mpf
) -> mpmath.mpf:
    """Approximate the fourth derivative of a function at a point."""
    return approximate_derivative(
        lambda x: approximate_third_derivative(function, x), point
    )


@cli.command()
@click.argument("degree", type=int)
def fourth(degree: int) -> None:
    """Calculate true values of the fourth derivative of polyharmonic spline RBFs."""
    for distance_rate_str in DISTANCE_RATE_STR_LIST:
        if distance_rate_str == "0.0":
            continue
        distance_rate = mpmath.mpf(distance_rate_str)
        value = approximate_fourth_derivative(
            lambda x: calculate_polyharmonic_spline_rbf(x, degree), distance_rate
        )
        print(f"std::make_tuple({distance_rate_str}, {mpmath.nstr(value, n=16)}),")


if __name__ == "__main__":
    cli()
