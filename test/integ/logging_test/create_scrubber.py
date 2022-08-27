"""Scrubber to use in approvaltests.
"""

import pathlib
import re
import typing

import approvaltests.scrubbers


def _create_timestamp_scrubber() -> typing.Callable[[str], str]:
    return approvaltests.scrubbers.create_regex_scrubber(
        r"\d\d\d\d-\d\d-\d\dT\d\d:\d\d:\d\d\.\d\d\d\d\d\d.\d\d\d\d",
        "<time-stamp>",
    )


def _scrub_exception_path(input: str) -> str:
    def replace_path(match: re.Match[str]) -> str:
        filename = pathlib.Path(match.group(1)).name
        line = match.group(2)
        col = match.group(3)

        return f"({filename}:{line}:{col})"

    return re.sub(r"\((.*):(\d*):(\d*)\)", replace_path, input)

def _create_config_filepath_scrubber(
    config_filepath: str,
) -> typing.Callable[[str], str]:
    def scrub(input: str) -> str:
        return input.replace(config_filepath, "<config-filepath>")

    return scrub


def create_scrubber(config_filepath: pathlib.Path) -> typing.Callable[[str], str]:
    return approvaltests.scrubbers.combine_scrubbers(
        _create_timestamp_scrubber(),
        _scrub_exception_path,
        _create_config_filepath_scrubber(config_filepath),
    )
