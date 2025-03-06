"""Configure pytest."""

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
