"""
Command Line Component.

Following pattern from: https://dash.plotly.com/all-in-one-components

Author: Connor W. Colombo (colombo@cmu.edu)
Last Updated: 06/02/2023
"""
from __future__ import annotations

from typing import Any, Final, Dict, Type, List, Tuple
from collections import OrderedDict

import uuid
import pandas as pd
from dash import (
    no_update,
    html, dcc, dash_table,
    Output, Input, State,
    callback, MATCH
)
from dash.development.base_component import Component
import dash_bootstrap_components as dbc

import IrisBackendv3 as IB3

from IrisBackendv3.config import command_aliases
from IrisBackendv3.transceiver.xcvr_enum import TransceiverEnum
from IrisBackendv3.codec.magic import Magic
from IrisBackendv3.codec.metadata import DataPathway
from IrisBackendv3.codec.payload import CommandPayload

from . import aio
from ..context import GuiContext
from ..backend import DatabaseKey, db_ready
from .. import style
from ..style import (
    DROPDOWN_LABEL_STYLE_MIXIN,
    DROPDOWN_STYLE_MIXIN,
    LABEL_STYLE_MIXIN
)


def handle_cmd_error(context: GuiContext, err_msg: str) -> None:
    """Logs an error that occurs while making a command.
    Having a wrapper like this allows for generating a modal popup later if
    desired.
    """
    context.ipc_app.logger.error(err_msg)


def build_command(
    context: GuiContext,
    cmd_magic_str: str,
    cmd_full_name: str,
    args: List[Tuple[str, Any]],
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
            # Validate command name:
            standards.global_command_lookup(cmd_full_name)
        except:
            handle_cmd_error(
                context,
                f"Invalid command name: `{cmd_full_name}`."
            )
            return None, None

        # Extract + Validate Arguments:
        args_dict = OrderedDict(
            (arg[0].strip(), arg[1].strip())
            for arg in args
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


def create_option_dropdown(
    name: str,
    id: Any,
    options: List[str],
    labels: List[str] | None = None,
    default_value: str | Component._UNDEFINED = Component.UNDEFINED,
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
        className='dropUp',
        optionHeight=60,
        **{
            **DROPDOWN_STYLE_MIXIN,
            'style': {
                **DROPDOWN_STYLE_MIXIN['style'],
                'width': '100%',
                'margin-bottom': '20px',
                **extra_style
            }
        }
    )


class _CommandLineAIO(html.Div):
    # NOTE: All-in-One Components should be suffixed with 'AIO'

    # Internal data:
    _aliases_table: command_aliases.CommandAliasesTable

    # Inner components:
    alias_selector: dcc.Dropdown
    cmd_type_selector: dcc.Dropdown
    command_selector: dcc.Dropdown
    args_container: dbc.Row
    transceiver_selector: dcc.Dropdown
    pathway_selector: dcc.Dropdown
    send_button: html.Button

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
        # Base ID for index pattern matching of Arguments:
        @staticmethod
        def _argument_base(aio_id): return f"{aio_id}-cli-arg"

    def __init__(
        self,
        context: GuiContext,
        aio_id=None
    ) -> None:
        # Make a uuid if an override is not given:
        aio_id = str(uuid.uuid4()) if aio_id is None else aio_id

        # TODO: Should we make the file selectable (for procedures?) or just
        # always provide the maximal set?
        # "Name of the command_aliases file in the "
        # f"`{command_aliases.DEFAULT_COMMAND_ALIASES_DIR}` "
        # "directory which defines which commands are "
        # "available."
        COMMAND_ALIASES_FILE: Final = "standard_command_aliases"

        # Load aliases:
        self._aliases_table = command_aliases.CommandAliasesTable.load_from_file(
            COMMAND_ALIASES_FILE
        )

        # Pick the first entry as the default:
        init_alias = next(iter(self._aliases_table._table.values()))
        init_alias_name = init_alias.alias
        init_command = init_alias.compiled_cmd.command
        init_command_full_name = init_command.name
        # Create correctly ordered list of default argument values:
        init_command_args = [
            init_alias.compiled_cmd.args[arg_spec.name]
            for arg_spec in init_command.args
        ]

        # Build Components:

        def cli_tooltip(
            msg: str,
            target: aio.AioSubId,
            width: str = '20vw'
        ) -> dbc.Tooltip:
            return dbc.Tooltip(
                "Who should PROCESS (interpret) this command FIRST.    \n"
                "For data processed FIRST by Watchdog: `WATCHDOG_COMMAND`.    \n"
                "For data processed FIRST by Radio: `RADIO_COMMAND`.    \n"
                "Otherwise, `COMMAND` (data is PROCESSED by Hercules first).",
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

        self.alias_selector = create_option_dropdown(
            'Premade Alias', self.ids.alias_selector(aio_id),
            options=[n for n in self._aliases_table._table.keys()],
            default_value=init_alias_name,
            extra_style={
                'minWidth': '10ch',
                'width': '100%',
                'maxWidth': '40ch'
            }
        )

        self.cmd_type_selector = create_option_dropdown(
            'Command Type', self.ids.cmd_type_selector(aio_id),
            options=[
                m.name for m in Magic
                if 'COMMAND' in m.name.upper() or 'CMD' in m.name.upper()
            ],
            default_value=Magic.COMMAND.name,
            extra_style={x: '25ch' for x in ['minWidth', 'width', 'maxWidth']}
        )

        self.command_selector = create_option_dropdown(
            'Fully Qualified Command Name', self.ids.command_selector(aio_id),
            options=[
                n for n in context.STANDARDS.commands_chainmap_name.keys()
            ],
            default_value=init_command_full_name,
            extra_style={
                'minWidth': '50ch',
                'width': '100%',
                'maxWidth': '200ch'
            }
        )

        self.args_container = dbc.Row(
            [
                dbc.Col(
                    [html.Label('Arguments:', **LABEL_STYLE_MIXIN)],
                    width=1
                ),
                dbc.Col(
                    [html.Div(style={'margin-left': '15ch'})],
                    width=1
                ),
                *_CommandLineAIO.command_argument_inputs(
                    aio_id,
                    init_command,
                    default_values=init_command_args
                )
            ],
            justify='start', align='left'
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
            default_value=DataPathway.WIRELESS.name,
            extra_style={x: '18ch' for x in ['minWidth', 'width', 'maxWidth']}
        )

        self.send_button = html.Button(
            'Send',
            id=self.ids.send_button(aio_id),
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
                        ], align='center'),
                        dbc.Col([
                            html.Label(
                                'Command Type',
                                **LABEL_STYLE_MIXIN,
                                id=aio.id_generator(
                                    '_CommandLineAIO', 'cmd_type_selector_label')(aio_id)
                            ),
                            self.cmd_type_selector,
                            cli_tooltip(
                                "Who should PROCESS (interpret) this command FIRST. \n"
                                "For data processed FIRST by Watchdog: `WATCHDOG_COMMAND`. \n"
                                "For data processed FIRST by Radio: `RADIO_COMMAND`. \n"
                                "Otherwise, `COMMAND` (data is processed by Hercules first).",
                                target=aio.id_generator(
                                    '_CommandLineAIO', 'cmd_type_selector_label')(aio_id)
                            )
                        ], align='center'),
                        dbc.Col([
                            html.Label('Command Name', **LABEL_STYLE_MIXIN),
                            self.command_selector
                        ], align='center'),
                    ], justify='evenly', align='center'),

                    # Command Arguments:
                    self.args_container
                ], width=7),

                # Uplink Settings:
                dbc.Col([
                    html.Label('Transceiver', **LABEL_STYLE_MIXIN),
                    self.transceiver_selector
                ], width=2, align='center'),
                dbc.Col([
                    html.Label('Pathway', **LABEL_STYLE_MIXIN),
                    self.pathway_selector
                ], width=2, align='center'),
                dbc.Col([self.send_button], width=1, align='top')
            ], justify='between', align='center'),
        ])

    @classmethod
    def argument_to_input(
        cls,
        aio_id: str,
        arg_index: int,
        arg_spec: IB3.data_standards.Argument,
        default_value: Any | None = None
    ) -> dcc.Dropdown | dcc.Input:
        """Creates an input field for specifying an argument based on the given
        argument specs.

        Optionally, a default (initial) value can be supplied for the argument.
        """
        FswDataType = IB3.data_standards.FswDataType
        FswDataTypeCategory = IB3.data_standards.fsw_data_type.Category
        # Create ID that will be used to address whatever field we build:
        arg_input_id = {
            'type': cls.ids._argument_base(aio_id),
            'index': arg_index
        }

        if default_value is None:
            default_value = Component.UNDEFINED

        if arg_spec.is_enum:
            # Check default value:
            if isinstance(default_value, (int, str)):
                try:
                    default_value = arg_spec.get_enum_item(default_value)
                    if default_value is not None:
                        default_value = default_value.name
                    else:
                        default_value = Component.UNDEFINED  # not valid
                except:
                    default_value = Component.UNDEFINED  # not valid
            else:
                default_value = Component.UNDEFINED  # not valid

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
                    'maxWidth': '25ch'
                }
            )
        elif arg_spec.datatype.category == FswDataTypeCategory.BOOLEAN:
            # Check default value:
            # Coerce other types to bool:
            if isinstance(default_value, str):
                if default_value.upper() in ('1', 'TRUE', '0XFF', 'FF'):
                    default_value = True
                elif default_value.upper() in ('0', 'FALSE', '0X00', '00'):
                    default_value = False
                else:
                    default_value = Component.UNDEFINED
            elif isinstance(default_value, (int, float)):
                try:
                    default_value = bool(default_value)
                except:
                    default_value = Component.UNDEFINED

            # Convert bool to string (how dropdown uses it):
            if isinstance(default_value, bool):
                default_value = str(default_value)
            else:
                # Wasn't bool / couldn't be turned into bool, so ignore:
                default_value = Component.UNDEFINED

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
        else:
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
            }.get(arg_spec.datatype, Component.UNDEFINED)
            max_val = {
                FswDataType.U8: 255,
                FswDataType.U16: 65_536,
                FswDataType.U32: 4_294_967_296,
                FswDataType.I8: 127,
                FswDataType.I16: 32_767,
                FswDataType.I32: 2_147_483_647,
            }.get(arg_spec.datatype, Component.UNDEFINED)
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
                step_size = Component.UNDEFINED
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
                    default_value = int(default_value)
                except:
                    default_value = Component.UNDEFINED
            elif input_type == 'number':
                # then it can be treated as just a float:
                try:
                    default_value = float(default_value)
                except:
                    default_value = Component.UNDEFINED
            elif input_type == 'text':
                default_value = str(default_value)
            else:
                default_value = Component.UNDEFINED

            # Second pass, just the bounds for numbers:
            if input_type == 'number' and (
                not isinstance(default_value, (int, float))
                or min_val != Component.UNDEFINED and default_value < min_val
                or max_val != Component.UNDEFINED and default_value > max_val
            ):
                # bounds exist and it fails them
                default_value = Component.UNDEFINED

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
                placeholder=f"{arg_spec.name} ({arg_spec.datatype.name})"
            )

    @classmethod
    def command_argument_inputs(
        cls,
        aio_id: str,
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
                html.Label(f'{a.name}:', **LABEL_STYLE_MIXIN),
                cls.argument_to_input(aio_id, i, a, default_value=dv)
            ])
            for i, (a, dv) in enumerate(zip(cmd_spec.args, default_values))
        ]


def make_command_line_aio(context: GuiContext, *args, **kwargs) -> _CommandLineAIO:
    """Makes a subclass of `_CommandLineAIO` with implemented context-dependent
    callbacks."""
    class _CommandLineAIO_w_Callbacks(_CommandLineAIO):
        ids = _CommandLineAIO.ids

        # Stateless pattern-matching callbacks for every component instance:

        # @callback(
        #     Output(ids.telem_store(MATCH), 'data'),
        #     Input(ids.interval(MATCH), 'n_intervals')
        # )
        # def fetch_telem_data(n_intervals):
        #     # fetches updated telem data from the database
        #     if db_ready(context.inner_db):
        #         df = context.inner_db.read(DatabaseKey.TELEM_DF)
        #         if isinstance(df, pd.DataFrame):
        #             return _TelemTableAIO.telem_to_json(df)
        #     return no_update

    return _CommandLineAIO_w_Callbacks(context, *args, **kwargs)
