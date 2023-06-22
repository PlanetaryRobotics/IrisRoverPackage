"""
Command Line Component.

Following pattern from: https://dash.plotly.com/all-in-one-components

Author: Connor W. Colombo (colombo@cmu.edu)
Last Updated: 06/19/2023
"""
from __future__ import annotations

from typing import Any, Final, Dict, Type, List, Tuple, cast
from collections import OrderedDict

from uuid import uuid4
import pandas as pd
import dash
from dash import (
    ctx, no_update,
    html, dcc,
    Output, Input, State,
    callback, MATCH, ALL
)
from dash.dependencies import _Wildcard
from dash.development.base_component import Component
import dash_bootstrap_components as dbc

import IrisBackendv3 as IB3

from IrisBackendv3.config import command_aliases
from IrisBackendv3.transceiver.xcvr_enum import TransceiverEnum
from IrisBackendv3.codec.magic import Magic
from IrisBackendv3.codec.metadata import DataPathway
from IrisBackendv3.codec.payload import CommandPayload
from IrisBackendv3.ipc.messages import (
    UplinkPayloadsRequestMessage, UplinkPayloadsRequestContent, UplinkPayloadsPacketSplit
)

from . import aio
from ..context import GuiContext
from .. import style
from ..style import (
    DROPDOWN_LABEL_STYLE_MIXIN,
    DROPDOWN_STYLE_MIXIN,
    LABEL_STYLE_MIXIN
)


class _CommandBuildingError(Exception):
    pass


def handle_cmd_error(context: GuiContext, err_msg: str) -> None:
    """Logs an error that occurs while making a command.
    Having a wrapper like this allows for generating a modal popup later if
    desired.
    """
    context.ipc_app.logger.error(err_msg)
    # Raise a special command error that will be caught at a higher level:
    raise _CommandBuildingError(err_msg)


def build_command(
    context: GuiContext,
    cmd_magic_str: str,
    cmd_full_name: str,
    args: List[Any],
    target_xcvr_str: str,
    target_pathway_str: str
) -> Tuple[CommandPayload | None, TransceiverEnum | None]:
    """Compiles the given command string in to a tuple containing a `CommandPayload` and a the `TransceiverEnum` specifying how it should be downlinked.
    Returns a `None, None` tuple if the command didn't validate.
    Command string is in the format from `alias_entry_to_cmd_string`.
    """
    standards = context.STANDARDS

    try:
        try:
            magic = Magic[cmd_magic_str.strip()]
        except:
            handle_cmd_error(
                context,
                f"Invalid Command `Magic`: `{cmd_magic_str}`.\n"
                f"Valid options are: {[e.name for e in Magic]}."
            )
            return None, None

        # Extract + Validate Command Name:
        cmd_full_name = cmd_full_name.strip()
        try:
            # Validate command name and get command specs:
            _, cmd_specs = standards.global_command_lookup(cmd_full_name)
        except:
            handle_cmd_error(
                context,
                f"Invalid command name: `{cmd_full_name}`."
            )
            return None, None

        # Extract + Validate Arguments:
        if len(args) != len(cmd_specs.args):
            handle_cmd_error(
                context,
                f"Command `{cmd_specs}` expects `{len(cmd_specs.args)}` "
                f"arguments but `{len(args)}` were given."
            )
            return None, None
        args = [v.strip() if isinstance(v, str) else v for v in args]
        args_dict: OrderedDict[str, Any] = OrderedDict(
            (arg_spec.name, arg_val)
            for arg_spec, arg_val in zip(cmd_specs.args, args)
        )
        # Adjust values as necessary:
        # Cast any non-strings to numbers, remove quotes from any strings:
        for k, v in args_dict.items():
            if (
                isinstance(v, str)
                and v[0] == v[-1]
                and v[0] in (r"'", r'"', r"`")
            ):
                # is a string surrounded by quotes. remove quotes
                args_dict[k] = v[1:-1]
            elif isinstance(v, str) and v.upper() in ('TRUE'):
                args_dict[k] = True
            elif isinstance(v, str) and v.upper() in ('FALSE'):
                args_dict[k] = False
            else:
                # See if this type can be coerced into a number:
                n: float | int
                float_worked: bool = False
                int_worked: bool = False
                try:
                    n = float(v)
                    float_worked = True
                    # make it an int if it can be an int:
                    if int(n) == n:
                        n = int(n)
                except ValueError:
                    float_worked = False

                if not float_worked:
                    # Try a non-base-10 integer:
                    try:
                        n = int(v, 0)
                        int_worked = True
                    except ValueError:
                        int_worked = False

                if float_worked or int_worked:
                    args_dict[k] = n

        # Extract and Validate the target XCVR:
        try:
            target_xcvr = TransceiverEnum[target_xcvr_str.strip()]
        except:
            handle_cmd_error(
                context,
                f"Invalid Transceiver: `{target_xcvr_str}`.\n"
                f"Valid options are: {[e.name for e in TransceiverEnum]}."
            )
            return None, None

        # Extract and Validate the pathway:
        try:
            pathway = DataPathway[target_pathway_str.strip()]
        except:
            handle_cmd_error(
                context,
                f"Invalid DataPathway: `{target_pathway_str}`.\n"
                f"Valid options are: {[e.name for e in DataPathway]}."
            )
            return None, None

        # Build command:
        try:
            cmd_payload: CommandPayload = command_aliases.build_command_from_alias(
                command_aliases.PreparedCommand(
                    name=cmd_full_name,
                    args=args_dict,
                    pathway=pathway,
                    magic=magic,
                    comment="(doesn't matter here since we're just building a payload)"
                )
            )
        except Exception as e:
            handle_cmd_error(
                context,
                f"Command failed to build because: `{e!s}`."
            )
            return None, None

        return cmd_payload, target_xcvr

    except Exception as e:
        handle_cmd_error(
            context,
            f"Processing command string failed for a non-validation reason.\n"
            f"Parameters were: "
            f"{cmd_magic_str=}, {cmd_full_name=}, {args=}, "
            f"{target_xcvr_str=}, {target_pathway_str=}. "
            f"\nFailed b/c:{e!s}."
        )
        return None, None


# Base Styling for all CLI Dropdowns:
CLI_DROPDOWN_BASE_STYLE: Final[Dict[str, Any]] = {
    **DROPDOWN_STYLE_MIXIN['style'],
    'width': '100%',
    'margin-bottom': '20px'
}


def _selector_css_classes(
    active: bool = True,
    extra_classes: str | List[str] | None = None
) -> str:
    """Which CSS classes to apply to a CommandLine selector, based on
    whether or not its active (driving which command is sent).

    Also will append any extra classes given in `extra_classes`
    (or the single extra class given if it's just a string).
    """
    classes = [
        'commandLine-selector',
        'dropUp'  # always make CLI "dropdowns" drop up
    ]
    if active:
        classes.append('commandLine-selector-active')
    else:
        classes.append('commandLine-selector-inactive')
    if extra_classes is not None:
        if not isinstance(extra_classes, list):
            extra_classes = [str(extra_classes)]
        classes = classes + [str(x) for x in extra_classes]

    # concat CSS classes together using a space
    # (`dash` seems to allow this...):
    return ' '.join(classes)


def create_option_dropdown(
    name: str,
    id: Any,
    options: List[str],
    labels: List[str] | None = None,
    default_value: str | None = None,
    extra_style: Dict[str, Any] | None = None
) -> dcc.Dropdown:
    """Standardized format for options dropdown in the CommandLine interface.
    All options in `options` will be presented.

    The `name` will be presented as a placeholder if the user has nothing
    selected.

    If `labels` is given, it must be the same length as `options` and the
    values in `options` will be used as the internal backing values and
    `labels` will be used as the user-facing values.

    Any `extra_style` is mixed in to the dropdown's `style` in an overriding
    fashion.
    """
    if labels is None:
        labels = options
    elif len(labels) != len(options):
        raise ValueError(
            f"Attempting to `create_option_dropdown` "
            f"but `{len(labels)=}` != `{len(options)=}`. "
            f"`{labels=}`, `{options=}`."
        )
    if extra_style is None:
        extra_style = dict()
    return dcc.Dropdown(
        [
            {
                "label": html.Span([label], **DROPDOWN_LABEL_STYLE_MIXIN),
                "value": value
            }
            for label, value in zip(labels, options)
        ],
        id=id,
        value=default_value,
        multi=False,
        searchable=True,
        placeholder=name,
        className=_selector_css_classes(active=True),
        optionHeight=40,
        **{
            **DROPDOWN_STYLE_MIXIN,
            'style': {
                **CLI_DROPDOWN_BASE_STYLE,
                **extra_style
            }
        }
    )


def create_cli_tooltip(
    msg: str | List,
    target: aio.AioSubId,
    width: str = '20vw'
) -> dbc.Tooltip:
    return dbc.Tooltip(
        msg,
        id=aio.id_generator(
            target['component'],
            target['subcomponent']+'_tip'
        )(target['aio_id']),
        target=target,
        placement='top',
        delay={'show': 0, 'hide': 100},
        autohide=True,
        style={
            'min-width': '125px',
            'width': width,
            'max-width': width,
            'height': 'auto',
            'white-space': 'normal'
        }
    )


class _CommandLineAIO(html.Div):
    # NOTE: All-in-One Components should be suffixed with 'AIO'

    # Inner components:
    alias_selector: dcc.Dropdown
    cmd_type_selector: dcc.Dropdown
    command_selector: dcc.Dropdown
    args_container: dbc.Row
    transceiver_selector: dcc.Dropdown
    pathway_selector: dcc.Dropdown
    send_button: html.Button
    command_send_toast: dbc.Toast

    # This comes from the Dash AIO pattern. Allows for global lookup of
    # subcomponent ids given a parent aio_id. Seems to be necessary for
    # MATCH-based pattern matching for callbacks inside the AIO component
    # instance(s).

    class ids:
        alias_selector = aio.id_generator('_CommandLineAIO', 'alias_selector')
        cmd_type_selector = aio.id_generator(
            '_CommandLineAIO', 'cmd_type_selector')
        command_selector = aio.id_generator(
            '_CommandLineAIO', 'command_selector')
        args_container = aio.id_generator('_CommandLineAIO', 'args_container')
        transceiver_selector = aio.id_generator(
            '_CommandLineAIO', 'transceiver_selector')
        pathway_selector = aio.id_generator(
            '_CommandLineAIO', 'pathway_selector')
        send_button = aio.id_generator('_CommandLineAIO', 'send_button')
        command_send_toast = aio.id_generator(
            '_CommandLineAIO', 'command_send_toast')

        # Base ID for index pattern matching of Arguments:

        @staticmethod
        def _argument_base(aio_id):
            # don't need aio_id yet (would if we ever have multiple component
            # instances), in which case we'd need to support Wildcards like
            # `MATCH`.
            return f"aio-cli-arg"

    def __init__(
        self,
        context: GuiContext,
        aliases_table: command_aliases.CommandAliasesTable,
        aio_id=None
    ) -> None:
        # Make a uuid if an override is not given:
        aio_id = str(uuid4()) if aio_id is None else aio_id

        # Pick the first entry as the default:
        # This will populate the alias selector and all other derived fields
        # will be auto-populated through callbacks.
        init_alias = next(iter(aliases_table._table.values()))

        # Build Components:

        self.alias_selector = create_option_dropdown(
            'Premade Alias', self.ids.alias_selector(aio_id),
            options=[n for n in aliases_table._table.keys()],
            default_value=init_alias.alias,
            extra_style={x: '45ch' for x in ['minWidth', 'width', 'maxWidth']}
        )

        self.cmd_type_selector = create_option_dropdown(
            'Command Magic', self.ids.cmd_type_selector(aio_id),
            options=[
                m.name for m in Magic
                if 'COMMAND' in m.name.upper() or 'CMD' in m.name.upper()
            ],
            labels=[
                m.name if m.name != 'COMMAND' else 'HERCULES_COMMAND'
                for m in Magic
                if 'COMMAND' in m.name.upper() or 'CMD' in m.name.upper()
            ],
            extra_style={x: '25ch' for x in ['minWidth', 'width', 'maxWidth']}
        )

        self.command_selector = create_option_dropdown(
            'Fully Qualified Command Name', self.ids.command_selector(aio_id),
            options=[
                n for n in context.STANDARDS.commands_chainmap_name.keys()
            ],
            extra_style={
                'minWidth': '50ch',
                'width': '100%',
                'maxWidth': '200ch'
            }
        )

        self.args_container = dbc.Row(
            self.args_container_from_command(aio_id, None),
            id=self.ids.args_container(aio_id),
            justify='start', align='left', className='g-1'
        )

        self.transceiver_selector = create_option_dropdown(
            'Transceiver', self.ids.transceiver_selector(aio_id),
            options=[dp.name for dp in TransceiverEnum],
            default_value=TransceiverEnum.ALL.name,
            extra_style={x: '18ch' for x in ['minWidth', 'width', 'maxWidth']}
        )
        self.pathway_selector = create_option_dropdown(
            'Pathway', self.ids.pathway_selector(aio_id),
            options=[dp.name for dp in DataPathway],
            extra_style={x: '18ch' for x in ['minWidth', 'width', 'maxWidth']}
        )

        self.send_button = html.Button(
            'Send',
            id=self.ids.send_button(aio_id),
            className='commandLine-send-button',
            n_clicks=0
        )

        self.command_send_toast = dbc.Toast(
            # Status message of command send will appear here
            "Nothing to report.",
            id=self.ids.command_send_toast(aio_id),
            header="Command Result",
            is_open=False,
            dismissable=True,
            duration=5000,  # ms
            icon='primary',
            # Fixed position, hovering over everything:
            style={"position": "fixed", "top": 75, "right": 10, "width": 500},
        )

        # Build layout:
        super().__init__([  # `html.Div([...])`
            dbc.Row([
                dbc.Col([
                    dbc.Row([
                        # Command Selection:
                        dbc.Col([
                            html.Label('Alias', **LABEL_STYLE_MIXIN),
                            self.alias_selector
                        ], width='auto', align='center'),
                        dbc.Col([
                            html.Label(
                                'Command Type (Processor)',
                                **LABEL_STYLE_MIXIN,
                                id=(cmd_type_label_id := aio.id_generator(
                                    '_CommandLineAIO', 'cmd_type_selector_label')(aio_id))
                            ),
                            self.cmd_type_selector,
                            create_cli_tooltip(
                                [
                                    "Which Magic to apply. ",
                                    html.Br(),
                                    "Who should PROCESS (interpret) this command FIRST. ",
                                    html.Br(),
                                    "For data processed FIRST by Watchdog: `WATCHDOG_COMMAND`. ",
                                    html.Br(),
                                    "For data processed FIRST by Radio: `RADIO_COMMAND`. ",
                                    html.Br(),
                                    "Otherwise, `HERCULES_COMMAND` (`COMMAND`) (data is PROCESSED by Hercules first)."
                                ],
                                target=cmd_type_label_id
                            )
                        ], width='auto', align='center'),
                        dbc.Col([
                            html.Label('Command Name', **LABEL_STYLE_MIXIN),
                            self.command_selector
                        ], width=True, align='center'),
                    ], justify='evenly', align='center', className="g-1"),

                    # Command Arguments:
                    self.args_container
                ], width=True, style={'background-color': style.IrisColor.GREY5.value}),

                # Uplink Settings:
                dbc.Col(
                    [html.Label(u'⮕', **LABEL_STYLE_MIXIN)],
                    width='auto', align='center'
                ),
                dbc.Col([
                    html.Label('Transceiver', **LABEL_STYLE_MIXIN),
                    self.transceiver_selector
                ], width='auto', align='center'),
                dbc.Col([
                    html.Label('Pathway', **LABEL_STYLE_MIXIN),
                    self.pathway_selector
                ], width='auto', align='center'),
                dbc.Col([self.send_button], width='auto', align='top')
            ], justify='between', align='center', className="g-1"),
            self.command_send_toast  # hovers at top. not actually here
        ], style={'background-color': style.IrisColor.GREY5.value})

    @staticmethod
    def _DASH_UNDEFINED_ARG() -> None:
        """Static classvar for the value to pass for arguments in dash
        functions that are undefined."""
        return None  # Component.UNDEFINED

    @classmethod
    def _argument_id_base(
        cls,
        aio_id: str | _Wildcard,
        arg_index: int | _Wildcard
    ) -> Dict[str, Any]:
        """Creates the base ID used to identify or query an argument input."""
        return {
            'aio_id': aio_id,
            'type': cls.ids._argument_base(aio_id),
            'index': arg_index
        }

    @classmethod
    def _argument_full_id(
        cls,
        aio_id: str | _Wildcard,
        arg_index: int | _Wildcard,
        uuid: str | _Wildcard | None = None
    ) -> Dict[str, Any]:
        """Creates the full ID used to identify or query an argument input,
        including the UUID, making a new one if not supplied."""
        if uuid is None:
            uuid = str(uuid4())
        return {
            **cls._argument_id_base(aio_id, arg_index),
            # UUID to guarantee the full arg-dict is unique so update happens:
            # (don't need this in the base since it isn't required to query)
            # or a Wildcard can be used here for selections.
            'arg_uuid': uuid
        }

    @classmethod
    def argument_to_input(
        cls,
        aio_id: str | _Wildcard,
        arg_index: int,
        arg_spec: IB3.data_standards.Argument,
        default_value: Any | None = None
    ) -> dcc.Dropdown | dcc.Input:
        """Creates an input field for specifying an argument based on the given
        argument specs.

        Optionally, a default (initial) value can be supplied for the argument.
        """
        FswDataTypeCategory = IB3.data_standards.fsw_data_type.Category
        # Create ID that will be used to address whatever field we build:
        arg_input_id = cls._argument_full_id(aio_id, arg_index)
        if arg_spec.is_enum:
            return cls._argument_to_enum_field(
                arg_input_id, arg_spec, default_value
            )
        elif arg_spec.datatype.category == FswDataTypeCategory.BOOLEAN:
            return cls._argument_to_boolean_field(
                arg_input_id, arg_spec, default_value
            )
        else:
            return cls._argument_to_input_field(
                arg_input_id, arg_spec, default_value
            )

    @classmethod
    def _argument_to_enum_field(
        cls,
        arg_input_id: Dict[str, Any],
        arg_spec: IB3.data_standards.Argument,
        default_value: Any | None = None
    ) -> dcc.Dropdown:
        """Creates an input field for an enum argument with fixed allowed
        options with the given `arg_spec` and `arg_input_id` that should have
        the given `default_value`."""
        UNDEFINED: Final = cls._DASH_UNDEFINED_ARG()
        if default_value is None:
            default_value = UNDEFINED
        # Check default value:
        if isinstance(default_value, (int, str)):
            try:
                default_value = arg_spec.get_enum_item(default_value)
                if default_value is not None:
                    default_value = default_value.name
                else:
                    default_value = UNDEFINED  # not valid
            except:
                default_value = UNDEFINED  # not valid
        else:
            default_value = UNDEFINED  # not valid

        return create_option_dropdown(
            arg_spec.name,
            id=arg_input_id,
            options=[
                e.name for e in arg_spec.enum
            ],
            labels=[
                f"{e.name} ({e.value})" for e in arg_spec.enum
            ],
            default_value=default_value,
            extra_style={
                'minWidth': '10ch',
                'width': '100%',
                'maxWidth': '60ch'
            }
        )

    @classmethod
    def _argument_to_boolean_field(
        cls,
        arg_input_id: Dict[str, Any],
        arg_spec: IB3.data_standards.Argument,
        default_value: Any | None = None
    ) -> dcc.Dropdown:
        """Creates an input field for a boolean argument with the given
        `arg_spec` and `arg_input_id` that should have the given
        `default_value`."""
        UNDEFINED: Final = cls._DASH_UNDEFINED_ARG()
        if default_value is None:
            default_value = UNDEFINED
        # Check default value:
        # Coerce other types to bool:
        if isinstance(default_value, str):
            if default_value.upper() in ('1', 'TRUE', '0XFF', 'FF'):
                default_value = True
            elif default_value.upper() in ('0', 'FALSE', '0X00', '00'):
                default_value = False
            else:
                default_value = UNDEFINED
        elif isinstance(default_value, (int, float)):
            try:
                default_value = bool(default_value)
            except:
                default_value = UNDEFINED

        # Convert bool to string (how dropdown uses it):
        if isinstance(default_value, bool):
            default_value = str(default_value)
        else:
            # Wasn't bool / couldn't be turned into bool, so ignore:
            default_value = UNDEFINED

        return create_option_dropdown(
            arg_spec.name,
            id=arg_input_id,
            default_value=default_value,
            options=['False', 'True'],
            extra_style={
                'minWidth': '10ch',
                'width': '100%',
                'maxWidth': '25ch'
            }
        )

    @classmethod
    def _argument_to_input_field(
        cls,
        arg_input_id: Dict[str, Any],
        arg_spec: IB3.data_standards.Argument,
        default_value: Any | None = None
    ) -> dcc.Input:
        """Creates a generic numerical or text input field for the argument
        with the given `arg_spec` and `arg_input_id` that should have the given
        `default_value`."""
        # Useful aliases:
        FswDataType = IB3.data_standards.FswDataType
        FswDataTypeCategory = IB3.data_standards.fsw_data_type.Category
        UNDEFINED: Final = cls._DASH_UNDEFINED_ARG()
        # Process default:
        if default_value is None:
            default_value = UNDEFINED
        # Map FswDataType Category to input type:
        input_type: str = {
            FswDataTypeCategory.NUMBER: 'number',
            FswDataTypeCategory.STRING: 'text',
            FswDataTypeCategory.VARSTRING: 'text',
            FswDataTypeCategory.IRISBYTESTRING: 'text',
        }.get(arg_spec.datatype.category, 'text')
        # Same thing for min and max values based on datatype where it matters:
        min_val = {
            FswDataType.U8: 0,
            FswDataType.U16: 0,
            FswDataType.U32: 0,
            FswDataType.I8: -128,
            FswDataType.I16: -32_768,
            FswDataType.I32: -2_147_483_648,
        }.get(arg_spec.datatype, UNDEFINED)
        max_val = {
            FswDataType.U8: 255,
            FswDataType.U16: 65_536,
            FswDataType.U32: 4_294_967_296,
            FswDataType.I8: 127,
            FswDataType.I16: 32_767,
            FswDataType.I32: 2_147_483_647,
        }.get(arg_spec.datatype, UNDEFINED)
        step_size: int | None
        if arg_spec.datatype in [
            FswDataType.U8,
            FswDataType.U16,
            FswDataType.U32,
            FswDataType.I8,
            FswDataType.I16,
            FswDataType.I32
        ]:
            step_size = 1
        else:
            step_size = UNDEFINED
        # Validate / coerce default value:
        # First pass, check the type:
        if arg_spec.datatype in [
            FswDataType.U8,
            FswDataType.U16,
            FswDataType.U32,
            FswDataType.I8,
            FswDataType.I16,
            FswDataType.I32
        ]:
            try:
                default_value = int(default_value)  # type: ignore
            except:
                default_value = UNDEFINED
        elif input_type == 'number':
            # then it can be treated as just a float:
            try:
                default_value = float(default_value)  # type: ignore
            except:
                default_value = UNDEFINED
        elif input_type == 'text':
            default_value = str(default_value)
        else:
            default_value = UNDEFINED

        # Second pass, just the bounds for numbers:
        if input_type == 'number' and (
            not isinstance(default_value, (int, float))
            or min_val != UNDEFINED and default_value < cast(float, min_val)
            or max_val != UNDEFINED and default_value > cast(float, max_val)
        ):
            # bounds exist and it fails them
            default_value = UNDEFINED

        # Build Input:
        return dcc.Input(
            value=default_value,
            id=arg_input_id,
            type=input_type,
            min=min_val,
            max=max_val,
            step=step_size,
            autoComplete=False,
            spellCheck=False,
            name=arg_spec.name,
            placeholder=f"({arg_spec.datatype.name})",
            className=_selector_css_classes(active=True)
        )

    @classmethod
    def command_argument_inputs(
        cls,
        aio_id: str | _Wildcard,
        cmd_spec: IB3.data_standards.Command,
        default_values: List[Any] | None = None
    ) -> List[Component]:
        """Creates a list of components (labels and input fields) for all of
        the arguments in the given command spec.
        Optionally, default arguments (`default_values`) can be supplied for
        each argument in order.
        """
        if default_values is None:
            default_values = [None] * len(cmd_spec.args)
        elif len(default_values) != len(cmd_spec.args):
            raise ValueError(
                "In `command_argument_inputs`, the number of `default_values` "
                "must match the number of args in the command spec. "
                f"Got: `{default_values=}` for `{cmd_spec=}`."
            )

        if len(cmd_spec.args) == 0:
            return [dbc.Col([
                html.Label(f'None.', **LABEL_STYLE_MIXIN)
            ])]

        return [
            dbc.Col([
                html.Label(f'{a.name} ({a.datatype.name}):',
                           **LABEL_STYLE_MIXIN),
                html.Br(),
                cls.argument_to_input(aio_id, i, a, default_value=dv)
            ])
            for i, (a, dv) in enumerate(zip(cmd_spec.args, default_values))
        ]

    @classmethod
    def args_container_from_command(
        cls,
        aio_id: str | _Wildcard,
        cmd_spec: IB3.data_standards.Command | None,
        default_values: List[Any] | None = None
    ) -> List[Component]:
        """Creates all components (labels, input fields, spacing, etc) for all
        of the arguments in the given command spec so the list can be used as
        the children for `args_container`.

        Same as `command_argument_inputs` but with accessory components
        (label for the entire row of inputs, spacing, etc):
        """
        base = [
            dbc.Col(
                [html.Label('Arguments:', **LABEL_STYLE_MIXIN)],
                width='auto', align='center'
            ),
            dbc.Col(
                [html.Div(style={'margin-left': '5ch'})],
                width='auto'
            )
        ]

        if cmd_spec is None:
            return base
        else:
            return [
                *base,
                *cls.command_argument_inputs(aio_id, cmd_spec, default_values)
            ]


def make_command_line_aio(context: GuiContext, *args, **kwargs) -> _CommandLineAIO:
    """Makes a subclass of `_CommandLineAIO` with implemented context-dependent
    callbacks."""

    # Load Aliases Table:
    # TODO: Should we make the file selectable (for procedures?) or just
    # always provide the maximal set?
    # "Name of the command_aliases file in the "
    # f"`{command_aliases.DEFAULT_COMMAND_ALIASES_DIR}` "
    # "directory which defines which commands are "
    # "available."
    COMMAND_ALIASES_FILE: Final = "standard_command_aliases"

    # Load aliases table for:
    aliases_table: Final = command_aliases.CommandAliasesTable.load_from_file(
        COMMAND_ALIASES_FILE
    )

    class _CommandLineAIO_w_Callbacks(_CommandLineAIO):
        ids = _CommandLineAIO.ids

        # Stateless pattern-matching callbacks for every component instance:

        @callback(
            output=dict(
                command_type=Output(ids.cmd_type_selector(MATCH), 'value'),
                command_name=Output(ids.command_selector(MATCH), 'value'),
                args=Output(ids.args_container(MATCH), 'children')
            ),
            inputs=dict(
                alias_name=Input(ids.alias_selector(MATCH), 'value'),
                cmd_name=Input(ids.command_selector(MATCH), 'value')
            )
        )
        def selector_to_selector_updater(
            alias_name: str | None,
            cmd_name: str | None
        ) -> Dict[str, Any]:
            """Updates selectors (dropdowns, etc) based on selections in other
            selectors.

            All have to happen from one multiplexed callback because to prevent
            chained callback issues (e.g. if the alias->command_name callback
            was different than the (alias or command_name)->args callback, then
            change(alias) -> update(args)
            but also change(alias)->change(cmd)->update(arg) - overriding the
            changes from the alias, thus deleting the default args provided by
            the alias selector)."""
            # Get alias entry from alias name:
            if alias_name is None:
                alias_entry = None
            else:
                alias_entry = aliases_table.get_entry(alias_name)

            # Determine new args list based on which inputs changed:
            args = _CommandLineAIO_w_Callbacks._argument_mux(
                ctx, alias_entry, cmd_name
            )

            if ctx.args_grouping['alias_name']['triggered'] and alias_entry is not None:
                return dict(
                    command_type=alias_entry.compiled_cmd.magic.name,
                    command_name=alias_entry.compiled_cmd.command.name,
                    args=args
                )
            else:
                return dict(
                    command_type=no_update,
                    command_name=no_update,
                    args=args
                )

        def _argument_mux(
            ctx: dash._callback_context.CallbackContext,
            alias_entry: command_aliases.CommandAliasesTable.Entry | None,
            cmd_name: str | None
        ) -> List[Component] | dash._callback.NoUpdate:
            """
            Returns list of args components based on which selector changed.

            Multiplexes multiple agents to change the args list because
            dash only allows each Output to be changed by, at most, 1
            callback (and we want both the alias and command name selection
            callbacks to be able change the args list).
            """

            # Since we'll be constructing new objects here, we need a concrete
            # aio_id, not just `MATCH`, so we need to grab the aio_id from
            # whatever triggered this:
            if (
                not (tid := ctx.triggered_id)
                or not isinstance(tid, dict)
                or 'aio_id' not in tid
            ):
                # not enough info to continue
                return no_update
            aio_id = tid['aio_id']

            if ctx.args_grouping['alias_name']['triggered']:
                if alias_entry is None:
                    return no_update
                # Create correctly ordered list of default argument values:
                # (pulls the command spec (`Command`) from the compiled
                # `CommandPayload` and uses the order of args from there):
                default_arg_values = [
                    alias_entry.compiled_cmd.args[arg_spec.name]
                    for arg_spec in alias_entry.compiled_cmd.command.args
                ]

                return _CommandLineAIO.args_container_from_command(
                    aio_id,
                    alias_entry.compiled_cmd.command,
                    default_arg_values
                )
            else:
                # This was triggered by a new command name being triggered:
                # Find that command spec in the datastandards:
                if cmd_name is None:
                    return no_update
                try:
                    _, cmd = context.STANDARDS.global_command_lookup(cmd_name)
                except KeyError as _:
                    context.ipc_app.logger.error(
                        "In `argument_multiplexer`: "
                        f"No command with `{cmd_name=}` in the DataStandards. "
                        "Maybe typo or you need to rebuild them "
                        "(with `make standards`)?"
                    )
                    return no_update

                return _CommandLineAIO.args_container_from_command(aio_id, cmd)

        @callback(
            output=dict(
                alias_classes=Output(ids.alias_selector(MATCH), 'className'),
                command_type_classes=Output(
                    ids.cmd_type_selector(MATCH), 'className'),
                command_name_classes=Output(
                    ids.command_selector(MATCH), 'className'),
                arg_classes=Output(
                    _CommandLineAIO._argument_full_id(
                        aio_id=MATCH, arg_index=ALL, uuid=ALL
                    ),
                    'className'
                )
            ),
            inputs=dict(
                alias_name=Input(ids.alias_selector(MATCH), 'value'),
                command_type=Input(ids.cmd_type_selector(MATCH), 'value'),
                command_name=Input(ids.command_selector(MATCH), 'value'),
                args=Input(
                    _CommandLineAIO._argument_full_id(
                        aio_id=MATCH, arg_index=ALL, uuid=ALL
                    ),
                    'value'
                )
            )
        )
        def selector_styling(
            alias_name: str,
            command_type: str,
            command_name: str,
            args: List[Any]
        ) -> Dict[str, str | List[str]]:
            """Changes the style of the selectorS based on what the user
            has selected most recently, so it's clear whether or not the
            current settings are from an alias.

            NOTE: Unfortunately, we can't just see if the `alias_name` was
            changed and triggered this because, after it triggers, the changing
            alias will trigger other values to be changed which can also
            trigger this (e.g. change(alias_name) -> change(args) -> this).
            **Instead**, we need to check what we really care about: do the
            settings here match the default for the alias.
            This has the added benefit that if we deviate from and then return
            to the alias' specifications (say by changing an arg value), the
            selector will light up (activate) again.
            """
            if alias_name is None:
                alias_active = False
            else:
                alias_entry = aliases_table.get_entry(alias_name)
                default_arg_values = [
                    alias_entry.compiled_cmd.args[arg_spec.name]
                    for arg_spec in alias_entry.compiled_cmd.command.args
                ]

                # Style the alias selector as active iff all the settings match
                # what this alias specifies:
                alias_active = (
                    command_type == alias_entry.compiled_cmd.magic.name
                    and command_name == alias_entry.compiled_cmd.command.name
                    and args == default_arg_values
                )

            if alias_active:
                alias_extra_classes = []
            else:
                alias_extra_classes = ['commandLine-selector-overridden']

            n_args = len(args)
            return dict(
                alias_classes=_selector_css_classes(
                    active=alias_active,
                    extra_classes=alias_extra_classes
                ),
                command_type_classes=_selector_css_classes(not alias_active),
                command_name_classes=_selector_css_classes(not alias_active),
                arg_classes=[_selector_css_classes(not alias_active)] * n_args
            )

        @callback(
            output=dict(
                tst_body=Output(ids.command_send_toast(MATCH), 'children'),
                tst_header=Output(ids.command_send_toast(MATCH), 'header'),
                tst_icon=Output(ids.command_send_toast(MATCH), 'icon'),
                tst_open=Output(ids.command_send_toast(MATCH), 'is_open')
            ),
            inputs=dict(
                n_clicks_send=Input(ids.send_button(MATCH), 'n_clicks')
            ),
            state=dict(
                command_type=State(ids.cmd_type_selector(MATCH), 'value'),
                command_name=State(ids.command_selector(MATCH), 'value'),
                args=State(
                    _CommandLineAIO._argument_full_id(
                        aio_id=MATCH, arg_index=ALL, uuid=ALL
                    ),
                    'value'
                ),
                xcvr_target_name=State(
                    ids.transceiver_selector(MATCH), 'value'),
                pathway_name=State(ids.pathway_selector(MATCH), 'value')
            )
        )
        def send_command(
            n_clicks_send: int,
            command_type: str,
            command_name: str,
            args: List[Any],
            xcvr_target_name: str,
            pathway_name: str

        ) -> Dict[str, Any]:
            """Builds and sends a command based on the current command settings.

            Displays status via a Toast.
            """
            msg_html: List[Component | str] = []
            success_so_far: bool = True

            if n_clicks_send == 0:
                # Page load not a click. Abort.
                raise dash.exceptions.PreventUpdate

            # Make sure command name is valid if given:
            # If so, extract command specs:
            cmd_spec: IB3.data_standards.Command | None = None
            if command_name is not None:
                try:
                    cmd_spec = context.STANDARDS.\
                        global_command_lookup(command_name)[1]
                except (TypeError, KeyError) as _:
                    success_so_far = False
                    msg_html += [
                        html.Br(), html.Br(),
                        f"`{command_name}` isn't a valid command."
                    ]

            # Make sure all args are present:
            for val, name in [
                (command_type, "Command Type"),
                (command_name, "Command Name"),
                (xcvr_target_name, "Transceiver Target"),
                (pathway_name, "Pathway")
            ]:
                if val is None:
                    success_so_far = False
                    msg_html += [
                        html.Br(), html.Br(),
                        f"{name} can't be empty."
                    ]

            # Make sure all args are present. Error and notify if not:
            if cmd_spec is not None:
                if len(args) != len(cmd_spec.args):
                    success_so_far = False
                    msg_html += [
                        html.Br(), html.Br(),
                        f"Bad number of args given for `{cmd_spec}`. "
                        f"Expected `{len(cmd_spec.args)}`, got `{len(args)}`."
                    ]
                else:
                    for arg_spec, arg_val in zip(cmd_spec.args, args):
                        if arg_val is None:
                            success_so_far = False
                            msg_html += [
                                html.Br(), html.Br(),
                                f"Argument `{arg_spec}` can't be `None`."
                            ]

            # Build command & validate args:
            if success_so_far:
                try:
                    cmd_payload, xcvr_target = build_command(
                        context,
                        cmd_magic_str=command_type,
                        cmd_full_name=command_name,
                        args=args,
                        target_xcvr_str=xcvr_target_name,
                        target_pathway_str=pathway_name
                    )
                    if cmd_payload is None or xcvr_target is None:
                        # We should never get here but we need to at least check.
                        success_so_far = False
                        msg_html = [
                            f"Failed to build command {command_name} with: ",
                            html.Br(), html.Br(),
                            f"{command_type=}, {args=} "
                            f"-> {xcvr_target_name=}, {pathway_name=}."
                        ]
                except _CommandBuildingError as cbe:
                    success_so_far = False
                    msg_html = [
                        f"Failed to build command {command_name} with: ",
                        html.Br(), html.Br(),
                        f"{command_type=}, {args=} "
                        f"-> {xcvr_target_name=}, {pathway_name=} ",
                        html.Br(), html.Br(),
                        f"because: {cbe!s}"
                    ]

            # Attempt to dispatch command to IPC:
            if success_so_far:
                # If we're here, we know the types of cmd and xcvr, so assert
                # their types:
                cmd_payload = cast(CommandPayload, cmd_payload)
                xcvr_target = cast(TransceiverEnum, xcvr_target)

                # Create IPC context, connect, send, and close:
                try:
                    with IB3.ipc.create_context() as ipc_ctx:
                        socket = IB3.ipc.create_socket(
                            ipc_ctx,
                            IB3.ipc.SocketType.PUBLISHER,
                            IB3.ipc.Port.TRANSCEIVER_PUB
                        )
                        ipc_msg = UplinkPayloadsRequestMessage(
                            UplinkPayloadsRequestContent(
                                payloads=[cmd_payload],
                                split=UplinkPayloadsPacketSplit.INDIVIDUAL,
                                packet_class=IB3.codec.packet.IrisCommonPacket,
                                target_xcvr=xcvr_target
                            )
                        )
                        IB3.ipc.send_to(
                            socket,
                            ipc_msg,
                            topic=IB3.ipc.Topic.UL_PAYLOADS,
                            subtopic=b'gui'
                        )
                        socket.close()
                except Exception as e:
                    success_so_far = False
                    msg_html += [
                        f"Failed to dispatch `{cmd_payload}` via IPC.",
                        html.Br(), html.Br(),
                        f"because: {e!s}"
                    ]

            if success_so_far:
                msg_html = [
                    f"{cmd_payload}",
                    html.Br(), html.Br(),
                    "Command dispatched to transceiver successfully.",
                    html.Br(), html.Br(),
                    "Check transceiver for send success."
                ]

            # Make sure we don't start with a newline:
            while isinstance(msg_html[0], html.Br) and len(msg_html) > 0:
                msg_html = msg_html[1:]

            # Report status via Toast:
            return dict(
                tst_body=msg_html,
                tst_header=html.Span([html.I(command_name), " Status:"]),
                tst_icon='success' if success_so_far else 'danger',
                tst_open=True  # open it back up if needed
            )

    return _CommandLineAIO_w_Callbacks(context, aliases_table, *args, **kwargs)
