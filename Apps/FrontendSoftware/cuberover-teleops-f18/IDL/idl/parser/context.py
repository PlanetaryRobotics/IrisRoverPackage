import typing


class Context:
    def __init__(self):
        self.types = {}  # type: typing.Dict[str, 'idl_type.IdlType']

    def add_type(self, new_type: 'idl_type.IdlType') -> None:
        # Don't raise an error on a repeat, but also don't re-add it to the types dictionary
        if new_type not in self.types:
            self.types[new_type.get_name()] = new_type

    def get_type(self, type_name: str) -> 'idl_type.IdlType':
        return self.types[type_name]

    def get_all_types(self) -> typing.Sequence['idl_type.IdlType']:
        return self.types.values()

    def is_known_type(self, type_name: str) -> bool:
        return type_name in self.types
