"""Helper script to create a source file for unit build.
"""

from os import write
from pathlib import Path
import subprocess


THIS_DIR = Path(__file__).parent.absolute()
UNITY_SOURCE_FILE = THIS_DIR / 'unity_source.cpp'
CMAKE_VAR_FILE_PATH = THIS_DIR / 'source_list.cmake'


def list_source_files(base_dir: Path) -> list[Path]:
    files = []
    for child in base_dir.iterdir():
        if child.is_dir():
            files = files + list_source_files(child)
        elif child.is_file() and str(child.suffix) == '.cpp':
            files = files + [child]
    return files


def list_source_file_paths() -> list[str]:
    source_files = list_source_files(THIS_DIR)
    return [
        str(source_file.relative_to(THIS_DIR))
        for source_file in source_files
        if str(source_file) != str(UNITY_SOURCE_FILE)
    ]


def write_unity_source(file_paths: list[str]):
    with open(str(UNITY_SOURCE_FILE), mode='w', encoding='ascii') as file:
        for source_file in file_paths:
            file.write(
                f'#include "{source_file}" // NOLINT(bugprone-suspicious-include)\n')

    subprocess.run(['clang-format', '-i', str(UNITY_SOURCE_FILE)], check=True)


def write_cmake_var_file(file_paths: list[str]):
    with open(str(CMAKE_VAR_FILE_PATH), mode='w', encoding='ascii') as file:
        file.write('set(SOURCE_FILES\n')
        for source_file in file_paths:
            file.write(f'    {source_file}\n')
        file.write(')')

    subprocess.run(
        ['cmake-format', '-i', str(CMAKE_VAR_FILE_PATH)], check=True)


def main():
    file_paths = list_source_file_paths()
    write_unity_source(file_paths)
    write_cmake_var_file(file_paths)


if __name__ == '__main__':
    main()
