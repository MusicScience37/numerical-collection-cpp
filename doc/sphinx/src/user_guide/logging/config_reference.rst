Reference of configurations
==================================

Configurations are made of following blocks:

Configuration of a log tag
-------------------------------

.. code-block:: toml

    [[num_collect.logging.tag_configs]]

    # Log tag. (Required.)
    # Empty string specifies the default configuration.
    tag = "example_tag"

    # Name of a log sink. (Optional.)
    # Log sink with this name must be configured in
    # num_collect.logging.sinks configuration (written below) in the same file,
    # or use "default" log sink in this library.
    sink = "default"

    # Minimum log level to output. (Optional.)
    # Choices: trace, debug, iteration, summary, info, warning, error, critical, off.
    output_log_level = "info"

    # Minimum log level to output in child iterations. (Optional.)
    # Choices are same as output_log_level.
    output_log_level_in_child_iterations = "info"

    # Period to write iteration logs. (Optional.)
    iteration_output_period = 10

    # Period to write labels of iteration logs. (Optional.)
    iteration_label_period = 20

Configuration of a log sink to write to console
---------------------------------------------------

.. code-block:: toml

    [[num_collect.logging.sinks]]

    # Name of the log sink. (Required.)
    name = "console sink"

    # Type of the log sink. (Required.)
    type = "console"

    # Whether to use color. (Optional.)
    use_color = true

Configuration of a log sink to write to a single file
-------------------------------------------------------

.. code-block:: toml

    [[num_collect.logging.sinks]]

    # Name of the log sink. (Required.)
    name = "single file sink"

    # Type of the log sink. (Required.)
    type = "single_file"

    # Filepath. (Required.)
    filepath = "num_collect_test_integ_logging.log"

Configuration of a log sink to write to a log sink asynchronously
-------------------------------------------------------------------------

.. code-block:: toml

    [[num_collect.logging.sinks]]

    # Name of the log sink. (Required.)
    name = "async sink"

    # Type of the log sink. (Required.)
    type = "async"

    # Log sink used to actual logging in asynchronous processing. (Required.)
    # A log sink with this name must be configured in the same file.
    inner_sink_name = "another log sink"

Configuration of a log sink to write to multiple log sinks
-----------------------------------------------------------------

.. code-block:: toml

    [[num_collect.logging.sinks]]

    # Name of the log sink. (Required.)
    name = "combined sink"

    # Type of the log sink. (Required.)
    type = "combined"

    # Log sinks used to actual logging. (Required.)
    # Log sink with these names must be configured in the same file.
    inner_sink_names = ["sink1", "sink2"]

    # Minimum log levels to output. (Required.)
    # Number of elements must be same with inner_sink_names.
    output_log_levels = ["trace", "debug"]
