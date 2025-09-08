"""Scrubber to use in approvaltests."""

import pathlib
import re
import typing

import approvaltests.scrubbers

# pylint: disable=redefined-builtin


def _create_timestamp_scrubber() -> typing.Callable[[str], str]:
    """Create a scrubber that replaces timestamps.

    Returns:
        typing.Callable[[str], str]: Scrubber function.
    """
    return approvaltests.scrubbers.create_regex_scrubber(
        r"\d\d\d\d-\d\d-\d\dT\d\d:\d\d:\d\d\.\d\d\d\d\d\d\d\d\d",
        "<time-stamp>",
    )


def _scrub_exception_path(input: str) -> str:
    """Scrub exception paths in the input string.

    Args:
        input (str): Input string to scrub.

    Returns:
        str: Scrubbed string.
    """

    def replace_path(match: re.Match[str]) -> str:
        """Replace a path.

        Args:
            match (re.Match[str]): Match object.

        Returns:
            str: Replaced string.
        """
        filename = pathlib.Path(match.group(1)).name

        # Line number differs between Clang and GCC.
        # Column is not support in GCC.
        return f"({filename}:<line>:<col>)"

    return re.sub(r"\((.*):(\d*):(\d*)\)", replace_path, input)


def _scrub_file_log_location(input: str) -> str:
    """Scrub file log locations in the input string.

    Args:
        input (str): Input string to scrub.

    Returns:
        str: Scrubbed string.
    """

    def replace_path(match: re.Match[str]) -> str:
        """Replace a path.

        Args:
            match (re.Match[str]): Match object.

        Returns:
            str: Replaced string.
        """
        filename = pathlib.Path(match.group(1)).name
        line = match.group(2)

        # Column is not support in GCC.
        # Function name can differ between different compilers.
        return f"({filename}:{line}:<col>, <function-name>)"

    return re.sub(r"\((.*):(\d*):(\d*), (.*)\)", replace_path, input)


def _create_config_filepath_scrubber(
    config_filepath: str,
) -> typing.Callable[[str], str]:
    """Create a scrubber that replaces the config file path.

    Args:
        config_filepath (str): Path to the config file.

    Returns:
        typing.Callable[[str], str]: Scrubber function.
    """

    def scrub(input: str) -> str:
        return input.replace(config_filepath, "<config-filepath>")

    return scrub


def create_scrubber(config_filepath: pathlib.Path | str) -> typing.Callable[[str], str]:
    """Create a scrubber for tests.

    Args:
        config_filepath (pathlib.Path | str): Path to the config file.

    Returns:
        typing.Callable[[str], str]: Scrubber function.
    """
    return approvaltests.scrubbers.combine_scrubbers(
        _create_timestamp_scrubber(),
        _scrub_exception_path,
        _scrub_file_log_location,
        _create_config_filepath_scrubber(str(config_filepath)),
    )
