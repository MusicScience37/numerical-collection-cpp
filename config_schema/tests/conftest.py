"""Configuration of pytest."""

import pytest
from config_validator import ConfigValidator


@pytest.fixture(
    scope="session",
    params=["config_schema.json"],
)
def validator(request: pytest.FixtureRequest) -> ConfigValidator:
    """Get the validator."""
    schema_filename = request.param
    return ConfigValidator(schema_filename=schema_filename)
