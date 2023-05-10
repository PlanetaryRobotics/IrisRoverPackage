"""
Simple module for containing styling primitives in a central location.

Largely borrowed from `Apps/Frontend/src/styles/_colors.scss`, the `Cosmos
Design System` (from both `Colors and Typography` Page and the `Color styles`
pane) as of 05/10/2023.

Author = Connor W. Colombo (colombo@cmu.edu)
Last Updated = 05/10/2023
"""

from typing import Final, Dict
from enum import Enum

# Iris Colors:


class IrisColor(Enum):
    # Logo:
    IRIS_RED = "#FF3D00"
    # Main Palette Colors:
    PURPLE_LIGHT = "#B4A1FF"
    PURPLE = "#8A8DFF"
    PURPLE_DARK = "#6148C5"
    PRIMARY_DARKER = "#4930AD"
    GREEN_LIGHT = "#21DF9A"
    YELLOW = "#FFD039"
    RED_BRIGHT = "#FF2F46"
    # Additional Style Colors:
    BLUE_ACCENT = "#35BAF3"
    RED_MATTE = "#F35B6C"
    RED_LIGHT = "#FFBEC5"
    RED_DARK = "#E42C40"
    RED_DARKER = "#BF1A2C"
    GREEN_BASE = "#08AC5D"
    # Grayscale:
    GREY0 = "#FCFCFC"
    GREY1 = "#E9E9E9"
    GREY2 = "#9F9F9F"
    GREY3 = "#585858"
    GREY4 = "#424242"
    GREY5 = "#2A2A2A"
    GREY6 = "#222222"


class IrisDerivedColor(Enum):
    # Branding:
    BRAND = IrisColor.IRIS_RED.value

    # Palette:
    PRIMARY_LIGHT = IrisColor.PURPLE_LIGHT.value
    PRIMARY = IrisColor.PURPLE.value
    PRIMARY_DARK = IrisColor.PURPLE_DARK.value
    PRIMARY_DARKER = IrisColor.PRIMARY_DARKER.value
    NOMINAL = IrisColor.GREEN_LIGHT.value
    CAUTION = IrisColor.YELLOW.value
    DANGER = IrisColor.RED_BRIGHT.value

    # Greyscale Aliases:
    NEAR_WHITE = IrisColor.GREY0.value
    GREY_LIGHT = IrisColor.GREY1.value
    GREY = IrisColor.GREY2.value
    GREY_DARK = IrisColor.GREY3.value
    NEAR_BLACK = IrisColor.GREY6.value

    # Common Feature Colors:
    BACKGROUND = IrisColor.GREY6.value
    BACKGROUND_FIGMA = "#E5E5E5"
    MAP_ROUTE_LOCALIZED = IrisColor.GREEN_BASE.value
    MAP_ROUTE_ANTICIPATED = IrisColor.RED_MATTE.value
    MAP_ROUTE_PLANNED = IrisColor.YELLOW.value


# Mixin Dictionary to Expand into the properties dictionary for Dash Tables:
LABEL_STYLE_MIXIN: Final[Dict] = {
    'style': {
        'color': IrisDerivedColor.NEAR_WHITE.value,
        'fontWeight': 'bold'
    }
}

DROPDOWN_STYLE_MIXIN: Final[Dict] = {
    'style': {
        'color': IrisColor.BLUE_ACCENT.value,
        'background-color': IrisColor.GREY4.value,
        'min-width': '25ch',
        'max-height': '7em',
        'overflow-y': 'scroll'
    }
}

TABLE_STYLE_MIXIN: Final[Dict] = {
    'style_data': {
        'color': IrisDerivedColor.NEAR_WHITE.value,
        'backgroundColor': IrisColor.GREY5.value
    },
    'style_data_conditional': [
        {
            'if': {'row_index': 'even'},
            'backgroundColor': IrisColor.GREY6.value,
        }
    ],
    'style_header': {
        'backgroundColor': IrisColor.GREY4.value,
        'color': IrisDerivedColor.NEAR_WHITE.value,
        'fontWeight': 'bold'
    },
    # Style cells to default to smallest possible width:
    'style_cell': {
        'width': '5rem',
    },

    # Scroll if overflowing, keeping first row and column frozen:
    'fixed_columns': {'headers': True, 'data': 1},
    'style_table': {
        'minWidth': '100%',
        'overflowX': 'scroll'
    }
}
