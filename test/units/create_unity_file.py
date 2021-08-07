"""Helper script to create a source file for unit build.
"""

from os import write
from pathlib import Path
import subprocess
THIS_DIR = Path(__file__).parent.absolute()


def list_source_files(base_dir: Path) -> list[Path]:
    files = []
    for child in base_dir.iterdir():
        if child.is_dir():
            files = files + list_source_files(child)
        elif child.is_file() and str(child.suffix) == '.cpp':
            files = files + [child]
    return files


def write_unity_source():
    unity_source_file = THIS_DIR / 'unity_source.cpp'
    source_files = list_source_files(THIS_DIR)
    source_files = [
        source_file
        for source_file in source_files
        if str(source_file) != str(unity_source_file)
    ]
    with open(str(unity_source_file), mode='w', encoding='ascii') as file:
        for source_file in source_files:
            file.write(
                f'#include "{source_file.relative_to(THIS_DIR)}" // NOLINT(bugprone-suspicious-include)\n')

    subprocess.run(['clang-format', '-i', str(unity_source_file)], check=True)


if __name__ == '__main__':
    write_unity_source()
