"""Configuration of pytest."""

import typing

import pytest

from config_validator import ConfigValidator


@pytest.fixture(
    scope="session",
    params=[
        "https://json-schema.org/draft-04/schema",
        None,
    ],
)
def validator(request: pytest.FixtureRequest) -> ConfigValidator:
    """Get the validator."""
    return ConfigValidator(meta_schema_url=request.param)
