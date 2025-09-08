"""Configure pytest."""

import pathlib

import approvaltests
import pytest
from approvaltests.reporters import ReporterThatAutomaticallyApproves

# Disable pylint warning about redefined-outer-name because
# pytest fixtures can have the same name as outer variables.
# pylint: disable=redefined-outer-name


def pytest_addoption(parser: pytest.Parser):
    """add option for pytest"""

    parser.addoption(
        "--build",
        action="store",
        help="path to build directory",
    )


@pytest.fixture(scope="session", autouse=True)
def configure_approvaltests():
    """Configure approvaltests library."""

    approvaltests.set_default_reporter(ReporterThatAutomaticallyApproves())


@pytest.fixture
def build_dir_path(request: pytest.FixtureRequest) -> pathlib.Path:
    """Get the path to the build directory.

    Args:
        request (pytest.FixtureRequest): Fixture request object.

    Returns:
        pathlib.Path: Path to the build directory.
    """
    return pathlib.Path(request.config.getoption("--build")).absolute()


@pytest.fixture
def writer_path(build_dir_path: pathlib.Path) -> pathlib.Path:
    """Get the path to the log writer binary.

    Args:
        build_dir_path (pathlib.Path): Path to the build directory.

    Returns:
        pathlib.Path: Path to the log writer binary.
    """
    return build_dir_path / "bin" / "test_integ_logging_write_log_writer"


@pytest.fixture
def test_temp_dir_path(build_dir_path: pathlib.Path) -> pathlib.Path:
    """Get the path to the temporary test directory.

    Args:
        build_dir_path (pathlib.Path): Path to the build directory.

    Returns:
        pathlib.Path: Path to the temporary test directory.
    """
    return build_dir_path / "temp_test"


@pytest.fixture
def config_dir_path() -> pathlib.Path:
    """Get the path to the config directory.

    Returns:
        pathlib.Path: Path to the config directory.
    """
    return pathlib.Path(__file__).absolute().parent / "config"
