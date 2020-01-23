import re

FIRST_CAP_REGEX = re.compile(r'(.)([A-Z][a-z]+)')
ALL_CAP_REGEX = re.compile(r'([a-z0-9])([A-Z])')


def pascal_to_snake_case(input_str: str) -> str:
    partial = FIRST_CAP_REGEX.sub(r'\1_\2', input_str)
    return ALL_CAP_REGEX.sub(r'\1_\2', partial).lower()


def pascal_to_screaming_snake_case(input_str: str) -> str:
    return pascal_to_snake_case(input_str).upper()


def pascal_to_camel_case(input_str: str) -> str:
    if len(input_str) > 1:
        return input_str[0].lower() + input_str[1:]
    elif input_str:
        return input_str[0].lower()
    else:
        return input_str


def pascal_to_pretty_case(input_str: str) -> str:
    """Here, "pretty" means spaces where there would be underscore in snake_case, with each word capitalized."""
    snake = pascal_to_snake_case(input_str)
    spaced = snake.replace('_', ' ')
    return spaced.title()
