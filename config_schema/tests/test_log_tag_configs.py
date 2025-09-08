"""Test of validating log tag configurations."""

import fastjsonschema
import pytest
from config_validator import ConfigValidator


class TestLogTagConfigs:
    """Test of validating log tag configurations."""

    def test_all_params(self, validator: ConfigValidator):
        """Test of configuration with all parameters."""
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
        """Test of configuration with only required parameters."""
        config = """
        [[num_collect.logging.tag_configs]]
        tag = "example_tag"
        """
        validator.validate_text(config)

    def test_no_params(self, validator: ConfigValidator):
        """Test of configuration without parameters."""
        config = """
        [[num_collect.logging.tag_configs]]
        """
        with pytest.raises(fastjsonschema.JsonSchemaValueException):
            validator.validate_text(config)

    def test_nonexisting_params(self, validator: ConfigValidator):
        """Test of configuration with non-existing parameter."""
        config = """
        [[num_collect.logging.tag_configs]]
        tag = "example_tag"
        ta = "tag"
        """
        with pytest.raises(fastjsonschema.JsonSchemaValueException):
            validator.validate_text(config)

    @pytest.mark.parametrize(
        "tag",
        [
            '""',
            '"abc"',
            '"09AZaz"',
            '"abc::def"',
            '"abc.def"',
            '"abc.def::hij"',
        ],
    )
    def test_valid_tags(self, validator: ConfigValidator, tag: str):
        """Test of configuration with valid 'tag'."""
        config = f"""
        [[num_collect.logging.tag_configs]]
        tag = {tag}
        """
        validator.validate_text(config)

    @pytest.mark.parametrize(
        "tag",
        [
            "[]",
            '"abc/def"',
            '"abc:def"',
            '"abc@def"',
            '"abc[def"',
            '"abc`def"',
            '"abc{def"',
            '"abc::"',
            '"::def"',
            '"abc."',
            '".def"',
        ],
    )
    def test_invalid_tags(self, validator: ConfigValidator, tag: str):
        """Test of configuration with invalid 'tag'."""
        config = f"""
        [[num_collect.logging.tag_configs]]
        tag = {tag}
        """
        with pytest.raises(fastjsonschema.JsonSchemaValueException):
            validator.validate_text(config)

    def test_invalid_sink(self, validator: ConfigValidator):
        """Test of configuration with invalid 'sink'."""
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
        """Test of configuration with valid 'output_log_level'."""
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
        """Test of configuration with invalid 'output_log_level'."""
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
        """Test of configuration with valid 'output_log_level_in_child_iterations'."""
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
        """Test of configuration with invalid 'output_log_level_in_child_iterations'."""
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
        """Test of configuration with valid 'iteration_output_period'."""
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
        """Test of configuration with invalid 'iteration_output_period'."""
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
        """Test of configuration with valid 'iteration_label_period'."""
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
        """Test of configuration with invalid 'iteration_label_period'."""
        config = f"""
        [[num_collect.logging.tag_configs]]
        tag = "example_tag"
        iteration_label_period = {period}
        """
        with pytest.raises(fastjsonschema.JsonSchemaValueException):
            validator.validate_text(config)
