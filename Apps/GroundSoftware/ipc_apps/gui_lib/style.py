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
    PURPLE_DARKER = "#4930AD"
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
    PRIMARY_DARKER = IrisColor.PURPLE_DARKER.value
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
        'background-color': IrisColor.GREY3.value,
        'min-width': '25ch'
    }
}

DROPDOWN_LABEL_STYLE_MIXIN: Final[Dict] = {
    'style': {
        'color': IrisColor.BLUE_ACCENT.value,
        'font-weight': 'bold'
    }
}


TABLE_STYLE_MIXIN: Final[Dict] = {
    'style_data': {
        'color': IrisDerivedColor.NEAR_WHITE.value,
        'backgroundColor': IrisColor.GREY5.value,
    },
    'style_data_conditional': [
        {
            'if': {'row_index': 'even'},
            'backgroundColor': IrisColor.GREY6.value,
        },
        {
            "if": {"state": "selected"},  # 'active' | 'selected'
            'color': IrisColor.BLUE_ACCENT.value,
            "backgroundColor": IrisColor.BLUE_ACCENT.value + "6A",  # + alpha
            "border": "1px solid " + IrisColor.BLUE_ACCENT.value + "6A",
        },
        {
            "if": {"state": "active"},  # 'active' | 'selected'
            'color': IrisColor.BLUE_ACCENT.value,
            "backgroundColor": IrisColor.BLUE_ACCENT.value + "6A",
            "border": "1px solid " + IrisColor.BLUE_ACCENT.value,
        }
    ],
    'style_header': {
        'color': IrisDerivedColor.NEAR_WHITE.value,
        'backgroundColor': IrisColor.GREY4.value,
        'fontWeight': 'bold'
    },
    # Style cells to default to smallest possible width:
    'style_cell': {
        'width': '2ch'
    },

    # Scroll if overflowing, keeping first row and column frozen:
    'fixed_columns': {'headers': True, 'data': 1},
    'style_table': {
        'minWidth': '100%',
        'maxWidth': '100%',
        'overflowX': 'scroll'
    },

    'style_filter': {  # only applies to the outer wrapper
        'backgroundColor': IrisColor.GREY6.value
    },
    # Custom CSS for specialized needs (e.g. styling filter):
    'css': [
        {
            'selector': '.dash-spreadsheet-container .dash-spreadsheet-inner input:not([type=radio]):not([type=checkbox])',
            'rule': (
                f'color: {IrisColor.BLUE_ACCENT.value} !important; '
                f'text-align: left !important; '
                f'padding-left: 0.5em; '
            )
        },
        {
            'selector': '.dash-spreadsheet-container .dash-spreadsheet-inner th.dash-filter input',
            'rule': (
                f'accent: {IrisColor.BLUE_ACCENT.value} !important; '
            )
        }
    ]
}

# Custom Formatted CSS. Think of this as `custom.css` but for any classes that need to pull data from the Python
# backend (like standardized colors, etc).
CUSTOM_FORMATTED_CSS: Final = (f"""
/** COLOR: **/
html {{
    color: {IrisColor.GREY6.value};
}}

body {{
    background-color: {IrisColor.GREY6.value};
}}

/** BUTTON: **/

.commandLine-send-button {{
    background-color: {IrisColor.GREY3.value}; 
    color: {IrisColor.BLUE_ACCENT.value}; 
    border: 1px solid {IrisColor.BLUE_ACCENT.value};
    border-radius: 4px;
    height: {(dcc_dropdown_height:='36px')}; /* Matches hardcoded value from DCC dropdown */
}}

.commandLine-send-button:hover {{
  background-color: {IrisColor.BLUE_ACCENT.value};
  color: {IrisDerivedColor.NEAR_WHITE.value};
}}


/** COMMAND-LINE SELECTORS: **/

input.commandLine-selector{{
    text-align: left !important;
    height: {dcc_dropdown_height}; /* Matches hardcoded value from DCC dropdown */
}}

/* Applied to selectors in `command_line` (dropdown) when they're not actively
driving what command is sent BUT were used in getting to the current
configuration (e.g. we selected premade-alias XYZ, then overrode its default
settings): */
.commandLine-selector-overridden .Select-value-label span {{
    /* important to override any dash defaults */
    text-decoration: line-through !important;
}}
""" + ''.join(
    f"""

    {x} {{
        /* important to override any dash defaults */
        color: {IrisColor.GREY5.value} !important;
    }}
    """
    for x in [
        # Selector for text inside DCC dropdowns:
        '.commandLine-selector-inactive .Select-value-label span',
        # Selector for text inside input fields:
        'input.commandLine-selector-inactive'
    ]
) + ''.join(
    f"""

    {x} {{
        /* important to override any dash defaults */
        color: {IrisColor.BLUE_ACCENT.value} !important;
    }}
    """
    for x in [
        # Selector for text inside DCC dropdowns:
        '.commandLine-selector-active .Select-value-label span',
        # Selector for text inside input fields:
        'input.commandLine-selector-active'
    ]
)
)
