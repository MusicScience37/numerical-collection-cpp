"""Scrubber of timestamps to use in approvaltests.
"""

import typing

import approvaltests.scrubbers


def create_timestamp_scrubber() -> typing.Callable[[str], str]:
    return approvaltests.scrubbers.create_regex_scrubber(
        r"\d\d\d\d-\d\d-\d\dT\d\d:\d\d:\d\d\.\d\d\d\d\d\d.\d\d\d\d",
        "<time-stamp>",
    )
