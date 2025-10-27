"""Test calculation of oct class."""

import logging
import math
import pathlib
import re
import subprocess
import typing
import urllib.parse

import click
import mpmath
import pandas
import plotly.express
from num_collect_test_utils.multi_double.split_oct import split_oct

OCT_PRECISION = 53 * 4 - 1
mpmath.mp.prec = OCT_PRECISION  # Oct precision

logging.basicConfig(
    level=logging.WARNING,
    format="[%(asctime)s.%(msecs)03d] [%(levelname)s] [%(name)s] %(message)s",
    datefmt="%Y-%m-%d %H:%M:%S",
)
LOGGER = logging.getLogger(__name__)
LOGGER.setLevel(logging.DEBUG)

THIS_DIR = pathlib.Path(__file__).absolute().parent
TEMP_DATA_DIR = THIS_DIR / "temp_data"

OPERATOR_NAME_COLUMN = "Operation"
ARG1_COLUMN = "Argument 1"
ARG2_COLUMN = "Argument 2"
RESULT_COLUMN = "Result"
REL_ERROR_COLUMN = "Relative Error"

# Operators. Each element is
# - operator name
# - function to perform calculation in mpmath
# - number of arguments
# - Minimum input.
# - Maximum input.
OPERATORS = [
    ("operator+", lambda x, y: x + y, 2, -1e10, 1e10),
    ("operator-", lambda x, y: x - y, 2, -1e10, 1e10),
    ("operator*", lambda x, y: x * y, 2, -1e10, 1e10),
    ("operator/", lambda x, y: x / y, 2, -1e10, 1e10),
]


def generate_inputs(
    *,
    num_args: int,
    min_input: float,
    max_input: float,
    num_inputs: int,
) -> list[list]:
    """Generate inputs.

    Args:
        num_args (int): Number of arguments.
        min_input (float): Minimum input.
        max_input (float): Maximum input.
        num_inputs (int): Number of inputs to be generated.

    Returns:
        list[list]: Inputs.
    """
    inputs: list[list] = []
    for _ in range(num_inputs):
        current_input = []
        for _ in range(num_args):
            random_value = mpmath.extraprec(20, True)(mpmath.rand)()
            if min_input < 0.0:
                if random_value < 0.5:
                    arg = min_input * mpmath.power(10, (random_value - 0.5) * 30)
                else:
                    arg = max_input * mpmath.power(10, (random_value - 1.0) * 30)
            else:
                arg = min_input * mpmath.power(max_input / min_input, random_value)
            current_input.append(arg)
        inputs.append(current_input)

    return inputs


def write_inputs_to_file(*, inputs: list[list], input_file_path: pathlib.Path) -> None:
    """Write inputs to a file.

    Args:
        inputs (list[list]): Inputs.
        input_file_path (pathlib.Path): Path to the input file.
    """
    with open(str(input_file_path), "w", encoding="utf-8") as file:
        for current_input in inputs:
            line = ""
            for arg in current_input:
                parts = split_oct(arg)
                # split_oct returns 4 double parts; write each as hex
                for p in parts:
                    line += f"{p.hex()},"
            line = line[:-1] + "\n"
            file.write(line)


def parse_inputs(*, input_file_path: pathlib.Path, num_args: int) -> list[list]:
    """Parse inputs.

    Args:
        input_file_path (pathlib.Path): Path to the input file.
        num_args (int): Number of arguments.

    Returns:
        list[list]: Parsed inputs.
    """
    with open(input_file_path, "r", encoding="utf-8") as file:
        inputs: list[list] = []
        for line in file:
            values = line.strip().split(",")
            if len(values) != 4 * num_args:
                raise ValueError("Invalid number of values.")
            current_input = []
            for i in range(num_args):
                comps = [float.fromhex(values[4 * i + j]) for j in range(4)]
                # reconstruct as mpmath mpf sum of parts
                current_input.append(sum(mpmath.mpf(c) for c in comps))
            inputs.append(current_input)
    return inputs


def run_calculator(
    *,
    executable_path: str,
    operator_name: str,
    input_file_path: pathlib.Path,
    output_file_path: pathlib.Path,
) -> float:
    """Run the calculator.

    Args:
        executable_path (str): Path to the executable.
        operator_name (str): Operator name.
        input_file_path (pathlib.Path): Path to the input file.
        output_file_path (pathlib.Path): Path to the output file.

    Returns:
        float: Computation time in seconds.
    """
    process_result = subprocess.run(
        [executable_path, operator_name, str(input_file_path), str(output_file_path)],
        stdout=subprocess.PIPE,
        encoding="utf-8",
        check=True,
    )
    match = re.search(r"Time: ([^\s]+) ms", process_result.stdout)
    if match is None:
        raise ValueError("Time not found.")
    time_ms = float(match.group(1))
    return 1e-3 * time_ms


def parse_outputs(*, output_file_path: pathlib.Path) -> list:
    """Parse outputs from a file.

    Args:
        output_file_path (pathlib.Path): Path to the output file.

    Returns:
        list: Outputs.
    """
    with open(output_file_path, "r", encoding="utf-8") as file:
        outputs: list = []
        for line in file:
            values = line.strip().split(",")
            if len(values) != 4:
                raise ValueError("Invalid number of values in output.")
            comps = [float.fromhex(v) for v in values]
            outputs.append(sum(mpmath.mpf(c) for c in comps))
    return outputs


def generate_results(  # pylint: disable=too-many-locals
    *,
    operator_name: str,
    operator_func: typing.Callable,
    inputs: list[list],
    outputs: list,
) -> pandas.DataFrame:
    """Generate data structure of results.

    Args:
        operator_name (str): Operator name.
        operator_func (typing.Callable): Operator function.
        inputs (list[list]): List of input values.
        outputs (list): List of output values.

    Returns:
        pandas.DataFrame: DataFrame containing the results.
    """
    arg1_values = []
    arg2_values = []
    results = []
    rel_errors = []
    for input_values, output_value in zip(inputs, outputs):
        formatted_inputs = []
        for val in input_values:
            parts = split_oct(val)
            formatted_inputs.append(
                f"oct({parts[0].hex()}, {parts[1].hex()}, {parts[2].hex()}, {parts[3].hex()})"
            )
        arg1_values.append(formatted_inputs[0])
        if len(input_values) >= 2:
            arg2_values.append(formatted_inputs[1])
        else:
            arg2_values.append("")
        parts = split_oct(output_value)
        formatted_output = (
            f"oct({parts[0].hex()}, {parts[1].hex()}, "
            f"{parts[2].hex()}, {parts[3].hex()})"
        )
        results.append(formatted_output)

        expected = mpmath.extraprec(20, True)(operator_func)(*input_values)
        rel_error = float(abs((output_value - expected) / expected))

        rel_errors.append(rel_error)

    return pandas.DataFrame(
        {
            OPERATOR_NAME_COLUMN: operator_name,
            ARG1_COLUMN: arg1_values,
            ARG2_COLUMN: arg2_values,
            RESULT_COLUMN: results,
            REL_ERROR_COLUMN: rel_errors,
        }
    )


def test_one_operator(  # pylint: disable=too-many-arguments
    *,
    operator_name: str,
    operator_func: typing.Callable,
    num_args: int,
    min_input: float,
    max_input: float,
    executable_path: str,
    num_inputs: int,
    reuse_inputs: bool,
) -> tuple[pandas.DataFrame, float]:
    """Test one operator.

    Args:
        operator_name (str): Operator name.
        num_args (int): Number of arguments.
        min_input (float): Minimum input.
        max_input (float): Maximum input.
        executable_path (str): Path to the executable.
        num_inputs (int): Number of inputs to be generated.
        reuse_inputs (bool): Whether to reuse inputs.

    Returns:
        tuple[pandas.DataFrame, float]: Results and
            the computation time in seconds.
    """
    TEMP_DATA_DIR.mkdir(exist_ok=True)
    operator_name_for_file_name = urllib.parse.quote(operator_name, safe="")
    input_file_path = TEMP_DATA_DIR / f"input_{operator_name_for_file_name}.csv"
    output_file_path = TEMP_DATA_DIR / f"output_{operator_name_for_file_name}.csv"
    LOGGER.debug("Input file path: %s", input_file_path)
    LOGGER.debug("Output file path: %s", output_file_path)

    if reuse_inputs and not input_file_path.exists():
        raise ValueError("Input file does not exist.")

    if reuse_inputs:
        inputs = parse_inputs(input_file_path=input_file_path, num_args=num_args)
    else:
        inputs = generate_inputs(
            num_args=num_args,
            min_input=min_input,
            max_input=max_input,
            num_inputs=num_inputs,
        )
        write_inputs_to_file(inputs=inputs, input_file_path=input_file_path)

    time_s = run_calculator(
        executable_path=executable_path,
        operator_name=operator_name,
        input_file_path=input_file_path,
        output_file_path=output_file_path,
    )

    outputs = parse_outputs(output_file_path=output_file_path)

    if len(outputs) != len(inputs):
        raise ValueError("Invalid number of outputs.")

    results = generate_results(
        operator_name=operator_name,
        operator_func=operator_func,
        inputs=inputs,
        outputs=outputs,
    )

    return (results, time_s)


def visualize_results(
    *, results: pandas.DataFrame, output_dir_path: pathlib.Path
) -> None:
    """Visualize results.

    Args:
        results (pandas.DataFrame): Results.
        output_dir_path (pathlib.Path): Path to the output directory.
    """
    LOGGER.info("Visualizing results.")

    # Zero values can cause problems in log scale.
    small_value = 2.0 ** (-OCT_PRECISION - 1)
    results[REL_ERROR_COLUMN] = results[REL_ERROR_COLUMN].apply(
        lambda x: max(x, small_value)
    )

    figure = plotly.express.box(
        results,
        x=OPERATOR_NAME_COLUMN,
        y=REL_ERROR_COLUMN,
        hover_data=[ARG1_COLUMN, ARG2_COLUMN, RESULT_COLUMN],
        log_y=True,
        title="Error of Oct Calculations",
    )
    figure.write_html(str(output_dir_path / "rel_error.html"))
    figure.write_image(str(output_dir_path / "rel_error.png"))


def show_stats(
    *, results: pandas.DataFrame, time_list: list[tuple[str, float]]
) -> None:
    """Show statistics of results.

    Args:
        results (pandas.DataFrame): Results.
        time_list (list[tuple[str, float]]): List of operator names and computation time.
    """
    LOGGER.info("Statistics:")
    LOGGER.info(
        "%15s %18s %22s %12s",
        "Operator Name",
        "Max Rel. Error",
        "Approx. Prec. (bits)",
        "Time (ms)",
    )
    for operator_name, time_s in time_list:
        max_rel_error = results[results[OPERATOR_NAME_COLUMN] == operator_name][
            REL_ERROR_COLUMN
        ].max()
        min_prec = math.floor(-math.log2(max_rel_error))
        LOGGER.info(
            "%15s %18.3e %22s %12.4f",
            operator_name,
            max_rel_error,
            min_prec,
            1e3 * time_s,
        )


@click.command()
@click.option("--build_dir", "-b", required=True, help="Build directory.")
@click.option("--num_inputs", "-n", default=1000, help="Number of inputs.")
@click.option("--reuse_inputs", is_flag=True, help="Reuse inputs.")
def test_oct_math(build_dir: str, num_inputs: int, reuse_inputs: bool) -> None:
    """Test oct calculation."""
    build_path = pathlib.Path(build_dir).absolute()
    executable_path = build_path / "bin" / "test_integ_multi_double_calculate_oct"
    results_dir_path = build_path / "temp_test" / "multi_double" / "oct_math"
    results_dir_path.mkdir(parents=True, exist_ok=True)

    total_results = pandas.DataFrame()
    time_list: list[tuple[str, float]] = []
    for operator_name, operator_func, num_args, min_input, max_input in OPERATORS:
        LOGGER.info("Testing operator %s", operator_name)
        results, time_s = test_one_operator(
            operator_name=operator_name,
            operator_func=operator_func,
            num_args=num_args,
            min_input=min_input,
            max_input=max_input,
            executable_path=str(executable_path),
            num_inputs=num_inputs,
            reuse_inputs=reuse_inputs,
        )
        total_results = pandas.concat([total_results, results], ignore_index=True)
        time_list.append((operator_name, time_s))
        LOGGER.info("Finished testing %s (Time: %.4f ms)", operator_name, 1e3 * time_s)

    visualize_results(results=total_results, output_dir_path=results_dir_path)

    show_stats(results=total_results, time_list=time_list)

    total_results.to_csv(str(results_dir_path / "results.csv"), index=False)

    LOGGER.info("Finished.")


if __name__ == "__main__":
    test_oct_math()  # pylint: disable=no-value-for-parameter
