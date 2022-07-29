#!/usr/bin/env python3
"""Check source codes with include-what-you-use.
"""

import click
import pathlib
import trio
import tqdm

CLANG_INCLUDE_OPTION = ["-isystem", "/usr/lib/llvm-14/include/c++/v1/"]

# iwyu_tool.py -p . ../../test/units/constants/half_test.cpp ../../test/units/base/assert_test.cpp -- -isystem /usr/lib/llvm-14/include/c++/v1/ -Xiwyu --error


class IwyuProcessError(RuntimeError):
    pass


async def apply_iwyu_to_file(
    filepath: str,
    build_dir: str,
    tqdm_obj: tqdm.tqdm,
    limiter: trio.CapacityLimiter,
    stop_on_error: bool,
):

    async with limiter:
        result = await trio.run_process(
            ["iwyu_tool.py", "-p", ".", filepath, "--"]
            + CLANG_INCLUDE_OPTION
            + ["-Xiwyu", "--error"],
            capture_stdout=True,
            capture_stderr=True,
            check=False,
            cwd=build_dir,
        )
        if result.returncode == 0:
            tqdm_obj.write(f"> {filepath}: OK")
            tqdm_obj.update()
        else:
            tqdm_obj.write(
                f"""> {filepath}: NG
{result.stdout.decode("utf8")}
{result.stderr.decode("utf8")}"""
            )
            tqdm_obj.update()
            if stop_on_error:
                raise IwyuProcessError(f"Error in {filepath}.")


def filter_iwyu_process_error(exc):
    if isinstance(exc, IwyuProcessError):
        return None
    else:
        return exc


async def apply_iwyu_to_files(
    filepaths: list[str],
    build_dir: str,
    num_jobs: int,
    stop_on_error: bool,
):

    tqdm_obj = tqdm.tqdm(total=len(filepaths), unit="file")
    limiter = trio.CapacityLimiter(num_jobs)
    try:
        async with trio.open_nursery() as nursery:
            for filepath in filepaths:
                nursery.start_soon(
                    apply_iwyu_to_file,
                    filepath,
                    build_dir,
                    tqdm_obj,
                    limiter,
                    stop_on_error,
                )
    except IwyuProcessError:
        pass
    except trio.MultiError as e:
        e = trio.MultiError.filter(filter_iwyu_process_error, e)
        if e is not None:
            raise e
    finally:
        tqdm_obj.close()


def get_files_in(path: pathlib.Path) -> list[str]:

    filepaths: list[str] = []
    for child in path.iterdir():
        if child.is_file():
            filepaths.append(str(child))
        elif child.is_dir():
            filepaths = filepaths + get_files_in(child)

    return filepaths


@click.command()
@click.option("--build_dir", "-b", required=True, help="Build directory.")
@click.option("--num_jobs", "-j", default=1, help="Number of concurrent jobs.")
@click.argument("file_or_directory_paths", nargs=-1)
def check_iwyu(file_or_directory_paths: list[str], build_dir: str, num_jobs: int):
    """Check source codes with include-what-you-use."""

    filepaths: list[str] = []
    for file_or_directory_path in file_or_directory_paths:
        path = pathlib.Path(file_or_directory_path).absolute()
        if path.is_file():
            filepaths.append(str(path))
        elif path.is_dir():
            filepaths = filepaths + get_files_in(path)

    filepaths = [
        filepath
        for filepath in filepaths
        if filepath.endswith(".cpp") and not filepath.endswith("unity_source.cpp")
    ]

    # Currently, stop on error always.
    stop_on_error = True

    trio.run(apply_iwyu_to_files, filepaths, build_dir, num_jobs, stop_on_error)


if __name__ == "__main__":
    check_iwyu()
