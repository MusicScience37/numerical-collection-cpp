# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))

import pathlib

THIS_DIR = pathlib.Path(__file__).absolute().parent

# -- Project information -----------------------------------------------------

project = "numerical-collection-cpp"
release = "unknown"  # inserted in command line

copyright = "2021, MusicScience37 (Kenta Kabashima). "
author = "MusicScience37"

# -- General configuration ---------------------------------------------------

extensions = []

templates_path = ["_templates"]

exclude_patterns = []

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
html_theme_options = {
    "show_prev_next": False,
    "logo": {
        "text": html_title,
    },
    "pygment_light_style": "gruvbox-light",
    "pygment_dark_style": "native",
    "repository_url": "https://gitlab.com/MusicScience37Projects/numerical-analysis/numerical-collection-cpp",
    "use_repository_button": True,
}
