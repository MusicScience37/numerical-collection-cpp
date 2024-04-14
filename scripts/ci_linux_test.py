#!/usr/bin/env python3
"""Script run in CI for tests in Linux."""

import os
import subprocess
import typing

import click

BUILD_TYPE_DICT = {
    "debug": "Debug",
    "release": "Release",
    "bench": "Release",
    "asan_ubsan": "Debug",
}


TEST_TYPE_VARIABLES = {
    "debug": {
        "NUM_COLLECT_TESTING": "ON",
        "NUM_COLLECT_ENABLE_UNIT_TESTS": "ON",
        "NUM_COLLECT_ENABLE_INTEG_TESTS": "ON",
        "NUM_COLLECT_ENABLE_BENCH": "OFF",
        "NUM_COLLECT_BUILD_EXAMPLES": "OFF",
        # "NUM_COLLECT_WRITE_JUNIT": "ON",
        "BUILD_SHARED_LIBS": "ON",
    },
    "release": {
        "NUM_COLLECT_TESTING": "ON",
        "NUM_COLLECT_ENABLE_UNIT_TESTS": "ON",
        "NUM_COLLECT_ENABLE_INTEG_TESTS": "ON",
        "NUM_COLLECT_ENABLE_BENCH": "OFF",
        "NUM_COLLECT_BUILD_EXAMPLES": "OFF",
        # "NUM_COLLECT_WRITE_JUNIT": "ON",
        "BUILD_SHARED_LIBS": "ON",
    },
    "bench": {
        "NUM_COLLECT_TESTING": "ON",
        "NUM_COLLECT_ENABLE_UNIT_TESTS": "OFF",
        "NUM_COLLECT_ENABLE_INTEG_TESTS": "OFF",
        "NUM_COLLECT_ENABLE_BENCH": "ON",
        "NUM_COLLECT_ENABLE_HEAVY_BENCH": "OFF",
        "NUM_COLLECT_TEST_BENCHMARKS": "ON",
        "NUM_COLLECT_BUILD_EXAMPLES": "ON",
        "NUM_COLLECT_TEST_EXAMPLES": "ON",
        # "NUM_COLLECT_WRITE_JUNIT": "ON",
        "BUILD_SHARED_LIBS": "OFF",
    },
    "asan_ubsan": {
        "NUM_COLLECT_TESTING": "ON",
        "NUM_COLLECT_ENABLE_UNIT_TESTS": "ON",
        "NUM_COLLECT_ENABLE_INTEG_TESTS": "ON",
        "NUM_COLLECT_ENABLE_BENCH": "OFF",
        "NUM_COLLECT_BUILD_EXAMPLES": "OFF",
        # "NUM_COLLECT_WRITE_JUNIT": "ON",
        "NUM_COLLECT_ENABLE_AUSAN": "ON",
    },
}


def execute_command(command: typing.List[str], cwd: str) -> None:
    click.echo(click.style(f">> {command}", bold=True, fg="green"))
    subprocess.run(command, check=True, cwd=cwd)


@click.command()
@click.option("--compiler_type", "compiler_type", required=True)
@click.option("--test_type", "test_type", required=True)
@click.option("--build_dir", "build_dir", required=True)
def check_tests_for_condition(
    compiler_type: str,
    test_type: str,
    build_dir: str,
) -> None:
    os.makedirs(build_dir, exist_ok=True)

    # Configure
    command = [
        "cmake",
        "..",
        "-G",
        "Ninja",
        "-DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake",
    ]
    build_type = BUILD_TYPE_DICT[test_type]
    command = command + [f"-DCMAKE_BUILD_TYPE={build_type}"]
    if compiler_type.startswith("clang"):
        command = command + [
            "-DVCPKG_TARGET_TRIPLET=x64-linux-llvm",
            "-DVCPKG_HOST_TRIPLET=x64-linux-llvm",
            "-DCMAKE_CXX_FLAGS=-stdlib=libc++",
        ]
    command = command + ["-DNUM_COLLECT_ENABLE_CCACHE:BOOL=ON"]
    for key, value in TEST_TYPE_VARIABLES[test_type].items():
        command = command + [f"-D{key}={value}"]
    execute_command(command, cwd=build_dir)

    # Build
    execute_command(["cmake", "--build", "."], cwd=build_dir)

    # Test
    execute_command(["ctest", "-V"], cwd=build_dir)


if __name__ == "__main__":
    check_tests_for_condition()
