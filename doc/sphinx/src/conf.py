"""Configuration of Sphinx."""

# Disable all pylint warnings for configuration files.
# pylint: disable=all

import pathlib

THIS_DIR = pathlib.Path(__file__).absolute().parent

# -- Project information -----------------------------------------------------

project = "Numerical Collection C++"
release = "unknown"  # inserted in command line

copyright = "2021, MusicScience37 (Kenta Kabashima)"
author = "MusicScience37"

# -- General configuration ---------------------------------------------------

extensions = []

templates_path = ["_templates"]

exclude_patterns: list[str] = []

# settings of myst-parser
extensions += ["myst_parser"]
myst_enable_extensions = [
    "amsmath",
    "dollarmath",
]

# setting of MathJax
# Extension for MathJax is already enabled by myst_nb.
# MathJax URL working with Plotly was written in https://www.npmjs.com/package/plotly.js/v/2.16.4#mathjax.
mathjax_path = "https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-svg.js"
mathjax3_config = {
    "tex": {
        "macros": {
            "bm": ["{\\boldsymbol{#1}}", 1],
        },
    },
}

# setting of todo
extensions += ["sphinx.ext.todo"]
todo_include_todos = True

# setting of PlantUML
extensions += ["sphinxcontrib.plantuml"]
plantuml_output_format = "svg"
plantuml_syntax_error_image = True

# setting of bibtex
# https://sphinxcontrib-bibtex.readthedocs.io/
extensions += ["sphinxcontrib.bibtex"]
bibtex_bibfiles = [str(THIS_DIR.parent.parent / "articles.bib")]
bibtex_default_style = "plain"

# -- Options for HTML output -------------------------------------------------

html_theme = "sphinx_orange_book_theme"
html_static_path = ["_static"]
html_title = project
html_favicon = "../../icon_logo/icon.ico"
html_theme_options = {
    "show_prev_next": False,
    "logo": {
        "image_light": "../../icon_logo/logo.svg",
        "image_dark": "../../icon_logo/logo-dark.svg",
        "text": html_title,
    },
    "pygment_light_style": "gruvbox-light",
    "pygment_dark_style": "native",
    "repository_url": "https://gitlab.com/MusicScience37Projects/numerical-analysis/numerical-collection-cpp",
    "use_repository_button": True,
}
