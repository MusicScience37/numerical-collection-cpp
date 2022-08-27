"""Test of invalid logging configuration files.
"""

import pathlib
import subprocess

import approvaltests

from .create_scrubber import create_scrubber


def _verify_command_result(
    writer_path: pathlib.Path,
    config_file_path: pathlib.Path,
    test_temp_dir_path: pathlib.Path,
) -> None:
    result = subprocess.run(
        args=[str(writer_path), str(config_file_path)],
        cwd=str(test_temp_dir_path),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        encoding="utf-8",
    )

    approvaltests.verify(
        f"""exit code: {result.returncode}
stdout:
{result.stdout}
stderr:
{result.stderr}
""",
        options=approvaltests.Options().with_scrubber(
            create_scrubber(str(config_file_path))
        ),
    )


def test_directory(
    writer_path: pathlib.Path,
    test_temp_dir_path: pathlib.Path,
    config_dir_path: pathlib.Path,
):
    """Test to pass directory paths."""

    _verify_command_result(
        writer_path=writer_path,
        config_file_path=config_dir_path,
        test_temp_dir_path=test_temp_dir_path,
    )


def test_nonexisting_file(
    writer_path: pathlib.Path,
    test_temp_dir_path: pathlib.Path,
    config_dir_path: pathlib.Path,
):
    """Test to pass non-existing paths."""

    _verify_command_result(
        writer_path=writer_path,
        config_file_path=config_dir_path / "non_existing_file.toml",
        test_temp_dir_path=test_temp_dir_path,
    )
