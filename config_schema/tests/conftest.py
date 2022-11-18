"""Configuration of pytest."""

import typing

import pytest

from config_validator import ConfigValidator


@pytest.fixture(
    scope="session",
    params=[
        ("config_schema4.json", "https://json-schema.org/draft-04/schema"),
        ("config_schema4.json", None),
        ("config_schema7.json", None),
    ],
)
def validator(request: pytest.FixtureRequest) -> ConfigValidator:
    """Get the validator."""
    schema_filename, meta_schema_url = request.param
    return ConfigValidator(
        schema_filename=schema_filename, meta_schema_url=meta_schema_url
    )
