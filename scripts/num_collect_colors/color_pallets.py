"""Data structure of color pallets."""

import dataclasses
import pathlib

import yaml

THIS_DIR = pathlib.Path(__file__).absolute().parent
COLOR_PALLETS_YAML_PATH = THIS_DIR / "color_pallets.yaml"


@dataclasses.dataclass
class ColorPallet:
    """Data structure of color pallet."""

    name: str
    colors: list[tuple[float, tuple[float, float, float]]]


def load_color_pallets() -> dict[str, ColorPallet]:
    """Load color pallets.

    Returns:
        A dictionary of color pallets.
    """
    with COLOR_PALLETS_YAML_PATH.open("r", encoding="utf-8") as file:
        raw_data = yaml.safe_load(file)

    color_pallets: dict[str, ColorPallet] = {}
    for raw_pallet in raw_data["color_pallets"]:
        name = raw_pallet["name"]
        colors: list[tuple[float, tuple[float, float, float]]] = []
        for position_str, lch_list in raw_pallet["colors"].items():
            position = float(position_str)
            assert len(lch_list) == 3
            lch: tuple[float, float, float] = tuple(
                float(value) for value in lch_list
            )  # type: ignore
            colors.append((position, lch))
        color_pallets[name] = ColorPallet(name=name, colors=colors)

    return color_pallets
