#!/usr/bin/env python3
"""Helper script to perform profiling"""

import os
import subprocess
from pathlib import Path

import click

THIS_DIR = Path(__file__).absolute().parent
PROF_RESULTS_DIR = THIS_DIR.parent / "profiling" / "results"


@click.command()
@click.argument(
    "binary_path",
    nargs=1,
    type=click.Path(exists=True, executable=True, resolve_path=True),
)
@click.option(
    "--freq",
    "frequency",
    default=100,
    type=int,
    show_default=True,
    help="Frequency of the profiling.",
)
@click.option(
    "--nodecount",
    "nodecount",
    default=100,
    type=int,
    show_default=True,
    help="Number of nodes in the result.",
)
@click.option(
    "--focus",
    "focus",
    default="",
    type=str,
    show_default=False,
    help="Function to focus on.",
)
@click.option("--reuse", is_flag=True, help="Reuse the same profiling data.")
def perform_profiling(
    binary_path: str, frequency: int, nodecount: int, focus: str, reuse: bool
):
    """Perform profiling of the given binary.

    The given binary is assumed to be linked with libprofiler,
    includes ProfilerStart / ProfilerStop calls,
    and write profiling results to the a file with a name like `<binary_name>.prof`.
    """

    name = str(Path(binary_path).stem)
    click.echo(
        click.style(f"Start profiling of {name}", bold=True) + f" (at {binary_path})"
    )

    os.makedirs(str(PROF_RESULTS_DIR), exist_ok=True)

    if not reuse:
        env = os.environ
        env["CPUPROFILE_FREQUENCY"] = str(frequency)
        process_result = subprocess.run(
            [binary_path], env=env, cwd=str(PROF_RESULTS_DIR), check=True
        )
        click.echo(
            click.style(
                f"{name} finished with exit code: {process_result.returncode}",
                bold=True,
            )
        )

    pprof_command = [
        "pprof",
        "-pdf",
        f"-nodecount={nodecount}",
    ]
    if focus != "":
        pprof_command.append(f"-focus={focus}")
    pprof_command.append(str(binary_path))
    pprof_command.append(str(PROF_RESULTS_DIR / f"{name}.prof"))
    process_result = subprocess.run(
        pprof_command,
        stdout=open(  # pylint: disable=consider-using-with
            str(PROF_RESULTS_DIR / f"{name}.pdf"), mode="wb"
        ),
        check=False,
    )
    click.echo(
        click.style(
            f"pprof finished with exit code: {process_result.returncode}", bold=True
        )
    )


if __name__ == "__main__":
    perform_profiling()  # pylint: disable=no-value-for-parameter
