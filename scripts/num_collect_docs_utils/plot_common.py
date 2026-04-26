"""Common utilities for plotting in documentation."""

import IPython.display
import ms37_designs.plotly_templates
import plotly.graph_objects
import plotly.io


def set_common_configuration() -> None:
    """Set common configuration for plotting."""
    ms37_designs.plotly_templates.load_templates()
    plotly.io.templates.default = "ms37_white"


def show_figure(figure: plotly.graph_objects.Figure) -> IPython.display.HTML:
    """Show a figure.

    Args:
        figure (plotly.graph_objects.Figure): The figure to show.
    """
    return IPython.display.HTML(
        plotly.io.to_html(
            figure,
            full_html=False,
            include_plotlyjs="cdn",
            include_mathjax=False,
        )
    )
