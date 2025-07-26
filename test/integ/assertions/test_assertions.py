import pathlib
import subprocess


def test_integer_success(build_dir_path: pathlib.Path) -> None:
    """Test of a success of an assertion with integers."""
    binary_path = build_dir_path / "bin" / "test_integ_assert_integer_success"

    result = subprocess.run(
        [str(binary_path)],
        check=False,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        encoding="utf8",
    )

    assert result.returncode == 0
    assert "Assertion failed" not in result.stderr


def test_integer_failure(build_dir_path: pathlib.Path) -> None:
    """Test of a failure of an assertion with integers."""
    binary_path = build_dir_path / "bin" / "test_integ_assert_integer_failure"

    result = subprocess.run(
        [str(binary_path)],
        check=False,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        encoding="utf8",
    )

    assert result.returncode != 0
    assert "Assertion failed" in result.stderr
    assert "x == 3" in result.stderr
    assert "test_integer_failure.cpp" in result.stderr
    assert "main" in result.stderr
