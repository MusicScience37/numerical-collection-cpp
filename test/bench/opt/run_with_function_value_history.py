"""Run a benchmark with function value history."""

import gzip
import pathlib
import subprocess

import click
import msgpack
import plotly.graph_objects


@click.command()
@click.argument(
    "binary_path",
    nargs=1,
    type=click.Path(exists=True, executable=True, resolve_path=True),
)
@click.option(
    "--reuse",
    is_flag=True,
    help="Reuse the same measurement data.",
)
def run_with_function_value_history(binary_path: str, reuse: bool):
    """Run a benchmark with function value history."""
    bench_dir = pathlib.Path(binary_path).absolute().parent.parent / "bench"
    binary_name = pathlib.Path(binary_path).stem

    if not reuse:
        click.echo(click.style(f"Run {binary_name}", bold=True))
        subprocess.run(
            [
                binary_path,
                "--plot",
                binary_name,
                "--json",
                f"{binary_name}/result.json",
                "--compressed-msgpack",
                f"{binary_name}/result.data",
                "--history",
                f"{binary_name}/history.data",
            ],
            cwd=str(bench_dir),
            check=True,
        )

    with gzip.open(bench_dir / binary_name / "history.data", mode="rb") as file:
        data = msgpack.unpack(file)

    figure = plotly.graph_objects.Figure()
    for measurement in data:
        if measurement["evaluations_upper"] and measurement["function_values_upper"]:
            # TODO Implement this case.
            pass
        else:
            figure.add_trace(
                plotly.graph_objects.Scatter(
                    x=measurement["evaluations"],
                    y=measurement["function_values"],
                    name=measurement["optimizer_name"],
                    mode="lines",
                )
            )

    figure.update_layout(
        title={"text": "Change of Function Values"},
        xaxis={"title": "Number of Function Evaluations", "type": "log"},
        yaxis={"title": "Function Value", "type": "log"},
    )

    figure.write_html(str(bench_dir / binary_name / "history.html"))
    figure.write_image(str(bench_dir / binary_name / "history.png"))


if __name__ == "__main__":
    run_with_function_value_history()  # pylint: disable=no-value-for-parameter
