"""Test of validating example configurations."""

import os

import pytest

from constants import EXAMPLE_CONFIGS_DIR
from config_validator import ConfigValidator


@pytest.mark.parametrize(
    "filename",
    [
        "use_default_log_sink.toml",
        "write_to_console_and_file.toml",
        "write_to_single_file.toml",
    ],
)
def test_example_configs(filename: str, validator: ConfigValidator):
    """Test of validating example configurations."""
    validator.validate_file(os.path.join(EXAMPLE_CONFIGS_DIR, filename))
