Module IrisBackendv3.config.command_aliases
===========================================
Tools for processing a Command Aliases file.

Last Update: 03/10/2023

Functions
---------

    
`CommandAliasesBuilder(func: CommandAliasesBuilderType) ‑> Callable[[IrisBackendv3.data_standards.data_standards.DataStandards], Dict[str, IrisBackendv3.config.command_aliases.PreparedCommand]]`
:   Decorator that ensures the type signature of a function that returns
    `CommandAliases`. Use case:
    ```py
    @CommandAliasesBuilder
    def ALIASES(standards: DataStandards) -> CommandAliases:
        return {
            'command-1': PreparedCommand(
                # ...
            )
            # etc
        }
    ```

    
`build_command_from_alias(prepared_cmd: PreparedCommand, alias_name: str | None = None) ‑> IrisBackendv3.codec.payload.CommandPayload`
:   Builds the given `PreparedCommand` (and inherently checks it for
    correctness).

    
`import_command_aliases_file(file_name: str | None = None, command_aliases_dir: str | None = None) ‑> module`
:   Imports the `command_aliases` file called `file_name` from
    `command_aliases_dir` (w.r.t. the `GroundSoftware` directory).
    
    NOTE: The only requirement for `command_aliases` files is that they are
    python modules containing a method called `ALIASES` with the signature
    `CommandAliasesBuilderType`.

Classes
-------

`CommandAliasesTable(table: Dict[str, CommandAliasesTable.Entry])`
:   

    ### Class variables

    `Entry`
    :

    ### Static methods

    `from_aliases(aliases: CommandAliases) ‑> IrisBackendv3.config.command_aliases.CommandAliasesTable`
    :

    `load_from_file(file_name: str | None = None, file_dir: str | None = None) ‑> IrisBackendv3.config.command_aliases.CommandAliasesTable`
    :

    ### Methods

    `as_legacy_prepared_commands(self) ‑> Dict[str, Tuple[IrisBackendv3.codec.metadata.DataPathway, IrisBackendv3.codec.magic.Magic, str, collections.OrderedDict[str, Any], IrisBackendv3.codec.metadata.DataPathway]]`
    :   Reformats the table as a legacy prepared commands table, as would
        be expected by the legacy `IrisConsole` functions in
        `IrisBackendv3.utils.console_display`.

    `get_entry(self, alias_name: str) ‑> IrisBackendv3.config.command_aliases.CommandAliasesTable.Entry`
    :   Returns the entry in the `CommandAliasesTable` corresponding with
        the given `alias_name`.

`Parameter(value, names=None, *, module=None, qualname=None, type=None, start=1)`
:   Enum used to indicate that a parameter should be pasted in the
    given argument slot.
    Used for legacy applications where variable arguments needed to be flagged.
    Used with `LegacyPreparedCommandType`.
    
    Better said with an example:
    If a command alias has kwargs:
        ```
        OrderedDict(
            'arg1': 42,
            'arg2': Parameter.PASTE,
            'arg3': 'something else'
            'arg4': PARAMETER.PASTE
            'arg5': 0xBEEF
        )
        ```
    and `get_command` is called with `params=[1, 2]` then the command will be built with:
        ```
        args={'arg1': 42,
        'arg2': 1,
        'arg3': 'something else'
        'arg4': 2
        'arg5': 0xBEEF}
        ```

    ### Ancestors (in MRO)

    * enum.Enum

    ### Class variables

    `PASTE`
    :

`PreparedCommand(name: str, args: OrderedDict, pathway: DataPathway, magic: Magic, comment: str = '')`
:   Pre-prepared command.

    ### Class variables

    `args: collections.OrderedDict`
    :

    `comment: str`
    :

    `magic: IrisBackendv3.codec.magic.Magic`
    :

    `name: str`
    :

    `pathway: IrisBackendv3.codec.metadata.DataPathway`
    :