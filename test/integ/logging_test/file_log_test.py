"""Test of file logs."""

import pathlib
import subprocess

import approvaltests

from .create_scrubber import create_scrubber


def _verify_command_result(
    writer_path: pathlib.Path,
    config_file_path: pathlib.Path,
    test_temp_dir_path: pathlib.Path,
    log_path: pathlib.Path,
) -> None:
    result = subprocess.run(
        args=[str(writer_path), str(config_file_path)],
        cwd=str(test_temp_dir_path),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        encoding="utf-8",
    )

    with open(str(log_path), mode="r", encoding="utf-8") as file:
        log_contents = file.read()

    approvaltests.verify(
        f"""exit code: {result.returncode}
stdout:
{result.stdout}
stderr:
{result.stderr}
{log_path.name}:
{log_contents}
""",
        options=approvaltests.Options().with_scrubber(
            create_scrubber(str(config_file_path))
        ),
    )


def test_trace_log(
    writer_path: pathlib.Path,
    test_temp_dir_path: pathlib.Path,
    config_dir_path: pathlib.Path,
):
    """Test with trace log level."""

    _verify_command_result(
        writer_path=writer_path,
        config_file_path=config_dir_path / "file_log_trace.toml",
        test_temp_dir_path=test_temp_dir_path,
        log_path=test_temp_dir_path / "test_integ_logging.log",
    )


def test_combined_file_console_log(
    writer_path: pathlib.Path,
    test_temp_dir_path: pathlib.Path,
    config_dir_path: pathlib.Path,
):
    """Test of combined file and console logs."""

    _verify_command_result(
        writer_path=writer_path,
        config_file_path=config_dir_path / "combined_file_console_log.toml",
        test_temp_dir_path=test_temp_dir_path,
        log_path=test_temp_dir_path / "test_integ_logging.log",
    )


def test_async_file_log(
    writer_path: pathlib.Path,
    test_temp_dir_path: pathlib.Path,
    config_dir_path: pathlib.Path,
):
    """Test of asynchronous file logs."""

    _verify_command_result(
        writer_path=writer_path,
        config_file_path=config_dir_path / "async_file_log.toml",
        test_temp_dir_path=test_temp_dir_path,
        log_path=test_temp_dir_path / "test_integ_logging.log",
    )


def test_async_file_console_log(
    writer_path: pathlib.Path,
    test_temp_dir_path: pathlib.Path,
    config_dir_path: pathlib.Path,
):
    """Test of asynchronous logs to a file and console."""

    _verify_command_result(
        writer_path=writer_path,
        config_file_path=config_dir_path / "async_file_console_log.toml",
        test_temp_dir_path=test_temp_dir_path,
        log_path=test_temp_dir_path / "test_integ_logging.log",
    )
