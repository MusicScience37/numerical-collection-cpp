"""Test of validating example configurations."""

import pytest

import fastjsonschema

from config_validator import ConfigValidator


class TestLogTagConfigs:
    """Test of validating example configurations."""

    def test_all_params(self, validator: ConfigValidator):
        config = """
        [[num_collect.logging.tag_configs]]
        tag = "example_tag"
        sink = "default"
        output_log_level = "info"
        output_log_level_in_child_iterations = "info"
        iteration_output_period = 10
        iteration_label_period = 20
        """
        validator.validate_text(config)

    def test_only_required_params(self, validator: ConfigValidator):
        config = """
        [[num_collect.logging.tag_configs]]
        tag = "example_tag"
        """
        validator.validate_text(config)

    def test_no_params(self, validator: ConfigValidator):
        config = """
        [[num_collect.logging.tag_configs]]
        """
        with pytest.raises(fastjsonschema.JsonSchemaValueException):
            validator.validate_text(config)

    def test_nonexisting_params(self, validator: ConfigValidator):
        config = """
        [[num_collect.logging.tag_configs]]
        tag = "example_tag"
        ta = "tag"
        """
        with pytest.raises(fastjsonschema.JsonSchemaValueException):
            validator.validate_text(config)

    def test_invalid_sink(self, validator: ConfigValidator):
        config = """
        [[num_collect.logging.tag_configs]]
        tag = "example_tag"
        sink = []
        """
        with pytest.raises(fastjsonschema.JsonSchemaValueException):
            validator.validate_text(config)

    @pytest.mark.parametrize(
        "log_level",
        [
            "trace",
            "debug",
            "iteration",
            "summary",
            "info",
            "warning",
            "error",
            "critical",
            "off",
        ],
    )
    def test_valid_output_log_levels(self, validator: ConfigValidator, log_level: str):
        config = f"""
        [[num_collect.logging.tag_configs]]
        tag = "example_tag"
        output_log_level = "{log_level}"
        """
        validator.validate_text(config)

    @pytest.mark.parametrize(
        "log_level",
        [
            '"iteration_label"',
            '"any"',
            "[]",
        ],
    )
    def test_invalid_output_log_levels(
        self, validator: ConfigValidator, log_level: str
    ):
        config = f"""
        [[num_collect.logging.tag_configs]]
        tag = "example_tag"
        output_log_level = {log_level}
        """
        with pytest.raises(fastjsonschema.JsonSchemaValueException):
            validator.validate_text(config)

    @pytest.mark.parametrize(
        "log_level",
        [
            "trace",
            "debug",
            "iteration",
            "summary",
            "info",
            "warning",
            "error",
            "critical",
            "off",
        ],
    )
    def test_valid_output_log_level_in_child_iterations(
        self, validator: ConfigValidator, log_level: str
    ):
        config = f"""
        [[num_collect.logging.tag_configs]]
        tag = "example_tag"
        output_log_level_in_child_iterations = "{log_level}"
        """
        validator.validate_text(config)

    @pytest.mark.parametrize(
        "log_level",
        [
            '"iteration_label"',
            '"any"',
            "[]",
        ],
    )
    def test_invalid_output_log_level_in_child_iterations(
        self, validator: ConfigValidator, log_level: str
    ):
        config = f"""
        [[num_collect.logging.tag_configs]]
        tag = "example_tag"
        output_log_level_in_child_iterations = {log_level}
        """
        with pytest.raises(fastjsonschema.JsonSchemaValueException):
            validator.validate_text(config)

    @pytest.mark.parametrize("period", [1, 2, 3])
    def test_valid_iteration_output_period(
        self, validator: ConfigValidator, period: int
    ):
        config = f"""
        [[num_collect.logging.tag_configs]]
        tag = "example_tag"
        iteration_output_period = {period}
        """
        validator.validate_text(config)

    @pytest.mark.parametrize("period", [0, -1, '"abc"'])
    def test_invalid_iteration_output_period(
        self, validator: ConfigValidator, period: int
    ):
        config = f"""
        [[num_collect.logging.tag_configs]]
        tag = "example_tag"
        iteration_output_period = {period}
        """
        with pytest.raises(fastjsonschema.JsonSchemaValueException):
            validator.validate_text(config)

    @pytest.mark.parametrize("period", [1, 2, 3])
    def test_valid_iteration_label_period(
        self, validator: ConfigValidator, period: int
    ):
        config = f"""
        [[num_collect.logging.tag_configs]]
        tag = "example_tag"
        iteration_label_period = {period}
        """
        validator.validate_text(config)

    @pytest.mark.parametrize("period", [0, -1, '"abc"'])
    def test_invalid_iteration_label_period(
        self, validator: ConfigValidator, period: int
    ):
        config = f"""
        [[num_collect.logging.tag_configs]]
        tag = "example_tag"
        iteration_label_period = {period}
        """
        with pytest.raises(fastjsonschema.JsonSchemaValueException):
            validator.validate_text(config)
