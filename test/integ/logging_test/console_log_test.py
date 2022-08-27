"""Test of logging.
"""

import pathlib
import subprocess
import typing

import approvaltests

from .create_timestamp_scrubber import create_timestamp_scrubber


def _execute_command(
    command: typing.List[str], test_temp_dir_path: pathlib.Path
) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        command,
        cwd=str(test_temp_dir_path),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        encoding="utf-8",
    )


def _verify_command_result(
    command: typing.List[str], test_temp_dir_path: pathlib.Path
) -> None:
    result = _execute_command(command, test_temp_dir_path)
    approvaltests.verify(
        f"""exit code: {result.returncode}
stdout:
{result.stdout}
stderr:
{result.stderr}
""",
        options=approvaltests.Options().with_scrubber(create_timestamp_scrubber()),
    )


def test_trace_log(
    writer_path: pathlib.Path,
    test_temp_dir_path: pathlib.Path,
    config_dir_path: pathlib.Path,
):
    _verify_command_result(
        command=[str(writer_path), str(config_dir_path / "console_log_trace.toml")],
        test_temp_dir_path=str(test_temp_dir_path),
    )
