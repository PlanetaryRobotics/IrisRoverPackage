COMMAND_OPTIONS_FILE_TEMPLATE = \
"""export default [
{command_options}
]
"""

COMMAND_OPTION_TEMPLATE = \
"""  new CommandOption({{
    name: "{class_name}",
    desc: "{class_description}",
    type: {command_type_id_value},
    commandID: {command_id_value},
    params: [
{command_parameters}
    ]
  }})"""

COMMAND_OPTION_PARAM_TEMPLATE = \
    "      {{ name: \"{name}\", units: \"{units}\", type: \"{type}\", defaultVal: {default_value} }}"
