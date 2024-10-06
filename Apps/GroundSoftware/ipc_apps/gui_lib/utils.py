"""
Common utilities used across the GDS-GUI.

Author: Connor W. Colombo (colombo@cmu.edu)
Last Updated: 05/30/2023
"""
from typing import Final, List, Tuple, Dict

import pandas as pd  # type: ignore


def seconds_to_str(
    secs: float,
    time_delimiters: List[Tuple[int, str]] | None = None
) -> str:
    """Converts a the total number of seconds of a timedelta into a string
    representation using the given `time_delimiters`.
    The default time delimiters are days, hours, minutes, seconds.
    NOTE: Time delimiters should be ordered from largest to smallest time unit
    (e.g. days first, seconds last).

    # Examples:
    >>> seconds_to_str(0)
    '0s'
    >>> seconds_to_str(0.3)
    '0s'
    >>> seconds_to_str(0.9)
    '0s'
    >>> seconds_to_str(1.1)
    '1s'
    >>> seconds_to_str(12)
    '12s'
    >>> seconds_to_str(63)
    '1m3s'
    >>> seconds_to_str(3674)
    '1h1m14s'
    >>> seconds_to_str(93805)
    '1d2h3m25s'
    >>> seconds_to_str(86460)
    '1d1m'

    # Same exact behavior for negatives but with a negative sign:
    >>> seconds_to_str(-0.3)
    '0s'
    >>> seconds_to_str(-0.9)
    '0s'
    >>> seconds_to_str(-1.1)
    '-1s'
    >>> seconds_to_str(-12)
    '-12s'
    >>> seconds_to_str(-93805)
    '-1d2h3m25s'
    """
    # Strip off sign information (add back at end):
    sign = secs / abs(secs) if secs != 0 else 1
    secs = abs(secs)

    if time_delimiters is None:
        time_delimiters = [(86400, 'd'), (3600, 'h'), (60, 'm'), (1, 's')]

    parts: List[str] = []
    for coeff, unit in time_delimiters:
        num = secs // coeff
        if num > 0:
            parts.append(f"{int(num)}{unit}")
            secs -= num*coeff

    # If we didn't end up with anything, return 0 of the smallest unit:
    if len(parts) == 0:
        return "0" + str(time_delimiters[-1][1])

    return ('-' if sign < 0 else '') + ''.join(parts)


# Operators used in the filtering syntax for Dash's `DataTable`s
# (`dash_table`).
# Maps name for operator_type to all qualifying operator strings.
# (expanded from: https://dash.plotly.com/datatable/callbacks#backend-paging-with-filtering)
DASH_DATATABLE_FILTER_OPERATORS: Final[Dict[str, List[str]]] = dict(
    ge=['ge ', '>='],
    le=['le ', '<='],
    lt=['lt ', '<'],
    gt=['gt ', '>'],
    ne=['ne ', '!='],
    eq=['eq ', '='],
    contains=['contains ',],
    datestartswith=['datestartswith ']
)


def dash_table_split_filter_part(
    filter_expression: str
) -> Tuple[str | None, str | None, str | float | None]:
    """Splits out the part of a single filter expression based on operators
    for `dash_table` `DataTable`s.
    Returns a `(name, operator_name, value)` tuple where the `operator_name` is
    the key to the entry in `DASH_DATATABLE_FILTER_OPERATORS` where the given
    operator shows up.

    Expanded from the algorithm on:
    https://dash.plotly.com/datatable/callbacks#backend-paging-with-filtering
    """
    value: str | float
    for op_type_name, op_type in DASH_DATATABLE_FILTER_OPERATORS.items():
        for operator in op_type:
            if operator in filter_expression:
                name_part, value_part = filter_expression.split(operator, 1)
                name = name_part[name_part.find('{') + 1: name_part.rfind('}')]

                value_part = value_part.strip()
                v0 = value_part[0]
                if (v0 == value_part[-1] and v0 in ("'", '"', '`')):
                    value = value_part[1: -1].replace('\\' + v0, v0)
                else:
                    try:
                        value = float(value_part)
                    except ValueError:
                        value = value_part

                return name, op_type_name, value

    return (None, None, None)


DASH_DATATABLE_CONTAINS_SUBOPERATORS: Final[Dict[str, List[str]]] = dict(
    contains_any=['has', 'any'],
    contains_all=['all'],
    notcontains=['no', 'not', '!'],
    isin=['in', 'isin']
)


def dash_table_contains_sub_syntax_filter(
    df_col: pd.Series,
    filter_val: str | float | None
) -> pd.DataFrame:
    """Returns a boolean vector that can be used to filter DataFrame based on
    the given input column from the DataFrame and `filter_val` argument which
    was given to the `contains` operator.

    All sub-syntax operators follow the pattern:
    ```
    ${sub-operator} {sub-operand}
    ```

    Operand can optionally be surrounded by quotes if it's a single value:
    ```
    ${sub-operator} "{sub-operand}"
    ```

    Operand can also be a list, optionally surrounding its entries with quotes:
    ```
    ${sub-operator} [{sub-operand1}, {sub-operand2}, {sub-operand3}]
    ```
    or:
    ```
    ${sub-operator} ["{sub-operand1}", "{sub-operand2}", "{sub-operand3}"]
    ```
    NOTE: Both `[...]` and `(...)` may be used as list enclosures.

    If a list argument is given, a cell will be included if:
        - For `contains_any`: it contains ANY of the values in the list.
        - For `contains_all`: it contains ALL of the values in the list.
        - For `notcontains`: it contains NONE of the values in the list.
        - For `isin`: the cell is, in its entirety, contained in the list.

    Motivation:
    Dash DataTables don't directly pass the input to a filter input to as
    the filter query; instead it's first processed. So, any strings
    (non-syntactic tokens) that are in a filter input are instead treated as a
    value for the `contains` operator. So, if we want to layer on any
    additional syntax, we just have to handle it as a sub-syntax for arguments
    to the `contains` operators."""
    if not isinstance(filter_val, str) or len(filter_val) == 0 or filter_val[0] != '$':
        # Definitely no sub-syntax here, just perform a basic contains filter:
        # (this is the default operation performed by `dash`):
        return df_col.str.contains(filter_val)

    # Remove any quotes that might be wrapping the whole filter_val:
    v0 = filter_val[0]
    if v0 == filter_val[-1] and v0 in ("'", '"', '`'):
        filter_val = filter_val[1: -1].replace('\\' + v0, v0)

    operator, operand_expr = filter_val[1:].split(' ', 1)
    # Determine name of suboperator used (since several suboperators exist):
    for n, op_aliases in DASH_DATATABLE_CONTAINS_SUBOPERATORS.items():
        if operator in op_aliases:
            operator_name = n
            break
    else:
        # Didn't find a match. Just default to the first key:
        operator_name = next(iter(DASH_DATATABLE_CONTAINS_SUBOPERATORS.keys()))

    # Build operand list (or length-1 list if only one operand supplied):
    operand_expr = operand_expr.strip()
    operands_list: List[str]
    if (
        operand_expr[0] == '[' and operand_expr[-1] == ']' or
        operand_expr[0] == '(' and operand_expr[-1] == ')'
    ):
        operands_list = [opnd.strip()
                         for opnd in operand_expr[1:-1].split(',')]
    else:
        operands_list = [operand_expr]

    # Remove any quotes surrounding each operand:
    for i, opnd in enumerate(operands_list):
        v0 = opnd[0]
        if v0 == opnd[-1] and v0 in ("'", '"', '`'):
            operands_list[i] = opnd[1: -1].replace('\\' + v0, v0)

    res_filter = df_col == df_col  # start with unity filter

    if operator_name == 'notcontains':  # NONE of the values in the list:
        for opnd in operands_list:
            res_filter = res_filter & ~df_col.str.contains(opnd)
    elif operator_name == 'isin':  # is, in its entirety, in the list
        res_filter = res_filter & df_col.isin(operands_list)
    elif operator_name == 'contains_all':  # ALL of the values in the list
        for opnd in operands_list:
            res_filter = res_filter & df_col.str.contains(opnd)
    elif operator_name == 'contains_any':  # ANY of the values in the list
        # Just perform a basic contains operation:
        for opnd in operands_list:
            res_filter = res_filter | df_col.str.contains(opnd)

    return res_filter


def dash_table_default_filter_table(
    df: pd.DataFrame,
    filter_query: str
) -> pd.DataFrame:
    """Filters the given `DataFrame` based on the given `filter_query` using
    the base algorithm for `dash_table` `DataTable`s.

    Based on algorithm from:
    https://dash.plotly.com/datatable/callbacks#backend-paging-with-filtering
    """
    filtering_expressions = filter_query.split(' && ')
    dff = df
    for filter_expression in filtering_expressions:
        col, op, filter_val = dash_table_split_filter_part(filter_expression)

        if op in ('eq', 'ne', 'lt', 'le', 'gt', 'ge'):
            # these operators match pandas series operator method names
            dff = dff.loc[getattr(dff[col], op)(filter_val)]
        elif op == 'contains':
            dff = dff.loc[
                dash_table_contains_sub_syntax_filter(dff[col], filter_val)
            ]
        elif op == 'datestartswith':
            # this is a simplification of the `dash_table` `DataTable`
            # front-end filtering logic, only works with complete fields in
            # standard format:
            dff = dff.loc[dff[col].str.startswith(filter_val)]

    return dff


if __name__ == "__main__":
    # Run cheap doctests on built-in examples for any util functions:
    import doctest
    print(f"\n{doctest.testmod()}\n")
    print("Doctest Done!")
