"""Configure pytest.
"""

import pathlib

import approvaltests
import pytest
from approvaltests.reporters import ReporterThatAutomaticallyApproves


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
    return pathlib.Path(request.config.getoption("--build")).absolute()


@pytest.fixture
def writer_path(build_dir_path: pathlib.Path) -> pathlib.Path:
    return build_dir_path / "bin" / "num_collect_test_integ_logging_write_log_writer"


@pytest.fixture
def test_temp_dir_path(build_dir_path: pathlib.Path) -> pathlib.Path:
    return build_dir_path / "temp_test"


@pytest.fixture
def config_dir_path() -> pathlib.Path:
    return pathlib.Path(__file__).absolute().parent / "config"
