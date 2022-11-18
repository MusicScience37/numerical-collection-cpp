"""Constants."""

import os

THIS_DIR = os.path.dirname(os.path.abspath(__file__))

SCHEMA_FILEPATH = os.path.join(os.path.dirname(THIS_DIR), "v1", "config_schema.json")

ROOT_DIR = os.path.dirname(os.path.dirname(THIS_DIR))

EXAMPLE_CONFIGS_DIR = os.path.join(ROOT_DIR, "examples", "logging", "configs")

FILE_ENCODING = "utf8"
