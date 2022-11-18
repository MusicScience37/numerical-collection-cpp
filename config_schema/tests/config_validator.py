"""Validator of configurations."""

import json
import typing
import os

import fastjsonschema
import pytomlpp

from constants import SCHEMA_DIR, FILE_ENCODING


class ConfigValidator:
    """Validator of configurations."""

    def __init__(
        self, *, schema_filename: str, meta_schema_url: typing.Optional[str] = None
    ):
        """Constructor."""
        with open(
            os.path.join(SCHEMA_DIR, schema_filename), mode="r", encoding=FILE_ENCODING
        ) as file:
            schema = json.load(file)
        if meta_schema_url:
            schema["$schema"] = meta_schema_url
        self.validator = fastjsonschema.compile(schema)

    def validate_data(self, data: typing.Any) -> None:
        """Validate data.

        Args:
            data (typing.Any): Data.
        """
        self.validator(data)

    def validate_file(self, filepath: str) -> None:
        """Validate a file.

        Args:
            filepath (str): Filepath.
        """
        with open(filepath, mode="r", encoding=FILE_ENCODING) as file:
            data = pytomlpp.load(filepath)
        self.validate_data(data)

    def validate_text(self, text: str) -> None:
        """Validate a text.

        Args:
            text (str): Text.
        """
        data = pytomlpp.loads(text)
        self.validate_data(data)
