"""Test of validating log sink configurations."""

import pytest

import fastjsonschema

from config_validator import ConfigValidator


class TestLogSinkConfig:
    """Test of validating the common part of configurations of log sinks."""

    def test_no_name(self, validator: ConfigValidator):
        config = """
        [[num_collect.logging.sinks]]
        type = "console"
        """
        with pytest.raises(fastjsonschema.JsonSchemaValueException):
            validator.validate_text(config)

    def test_no_type(self, validator: ConfigValidator):
        config = """
        [[num_collect.logging.sinks]]
        name = "console sink"
        """
        with pytest.raises(fastjsonschema.JsonSchemaValueException):
            validator.validate_text(config)

    @pytest.mark.parametrize(
        "name",
        [
            '"any"',
        ],
    )
    def test_valid_name(self, validator: ConfigValidator, name: str):
        config = f"""
        [[num_collect.logging.sinks]]
        name = {name}
        type = "console"
        """
        validator.validate_text(config)

    @pytest.mark.parametrize(
        "name",
        [
            '"default"',
            "[]",
        ],
    )
    def test_invalid_name(self, validator: ConfigValidator, name: str):
        config = f"""
        [[num_collect.logging.sinks]]
        name = {name}
        type = "console"
        """
        with pytest.raises(fastjsonschema.JsonSchemaValueException):
            validator.validate_text(config)

    @pytest.mark.parametrize(
        "type_name",
        [
            '"any"',
            "[]",
        ],
    )
    def test_invalid_type(self, validator: ConfigValidator, type_name: str):
        config = f"""
        [[num_collect.logging.sinks]]
        name = "console sink"
        type = {type_name}
        """
        with pytest.raises(fastjsonschema.JsonSchemaValueException):
            validator.validate_text(config)


class TestConsoleLogSinkConfig:
    """Test of validating configurations of log sinks to write to console."""

    def test_all_params(self, validator: ConfigValidator):
        config = """
        [[num_collect.logging.sinks]]
        name = "console sink"
        type = "console"
        use_color = true
        """
        validator.validate_text(config)

    def test_no_use_color(self, validator: ConfigValidator):
        config = """
        [[num_collect.logging.sinks]]
        name = "console sink"
        type = "console"
        """
        validator.validate_text(config)

    def test_invalid_use_color(self, validator: ConfigValidator):
        config = """
        [[num_collect.logging.sinks]]
        name = "console sink"
        type = "console"
        use_color = "abc"
        """
        with pytest.raises(fastjsonschema.JsonSchemaValueException):
            validator.validate_text(config)

    def test_invalid_param(self, validator: ConfigValidator):
        config = """
        [[num_collect.logging.sinks]]
        name = "console sink"
        type = "console"
        use_colo = true
        """
        with pytest.raises(fastjsonschema.JsonSchemaValueException):
            validator.validate_text(config)


class TestSingleFileLogSinkConfig:
    """Test of validating configurations of a log sink to write to a single file."""

    def test_all_params(self, validator: ConfigValidator):
        config = """
        [[num_collect.logging.sinks]]
        name = "single file sink"
        type = "single_file"
        filepath = "example.log"
        """
        validator.validate_text(config)

    def test_no_filepath(self, validator: ConfigValidator):
        config = """
        [[num_collect.logging.sinks]]
        name = "single file sink"
        type = "single_file"
        """
        with pytest.raises(fastjsonschema.JsonSchemaValueException):
            validator.validate_text(config)


class TestAsyncLogSinkConfig:
    """Test of validating configurations of asynchronous log sinks."""

    def test_all_params(self, validator: ConfigValidator):
        config = """
        [[num_collect.logging.sinks]]
        name = "async sink"
        type = "async"
        inner_sink_name = "console sink"
        """
        validator.validate_text(config)

    def test_no_inner_sink_name(self, validator: ConfigValidator):
        config = """
        [[num_collect.logging.sinks]]
        name = "async sink"
        type = "async"
        """
        with pytest.raises(fastjsonschema.JsonSchemaValueException):
            validator.validate_text(config)


class TestCombinedLogSinkConfig:
    """Test of validating configurations of combined log sinks."""

    def test_all_params(self, validator: ConfigValidator):
        config = """
        [[num_collect.logging.sinks]]
        name = "combined sink"
        type = "combined"
        inner_sink_names = ["sink1", "sink2"]
        output_log_levels = ["trace", "debug"]
        """
        validator.validate_text(config)

    def test_no_inner_sink_names(self, validator: ConfigValidator):
        config = """
        [[num_collect.logging.sinks]]
        name = "combined sink"
        type = "combined"
        output_log_levels = ["trace", "debug"]
        """
        with pytest.raises(fastjsonschema.JsonSchemaValueException):
            validator.validate_text(config)

    def test_invalid_inner_sink_names(self, validator: ConfigValidator):
        config = """
        [[num_collect.logging.sinks]]
        name = "combined sink"
        type = "combined"
        inner_sink_names = [[]]
        output_log_levels = ["trace", "debug"]
        """
        with pytest.raises(fastjsonschema.JsonSchemaValueException):
            validator.validate_text(config)

    def test_no_output_log_levels(self, validator: ConfigValidator):
        config = """
        [[num_collect.logging.sinks]]
        name = "combined sink"
        type = "combined"
        inner_sink_names = ["sink1", "sink2"]
        """
        with pytest.raises(fastjsonschema.JsonSchemaValueException):
            validator.validate_text(config)

    def test_invalid_output_log_levels(self, validator: ConfigValidator):
        config = """
        [[num_collect.logging.sinks]]
        name = "combined sink"
        type = "combined"
        inner_sink_names = ["sink1", "sink2"]
        output_log_levels = [[]]
        """
        with pytest.raises(fastjsonschema.JsonSchemaValueException):
            validator.validate_text(config)
