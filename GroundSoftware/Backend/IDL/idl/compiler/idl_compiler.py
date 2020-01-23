import abc
import argparse
import pathlib
import typing

class IdlCompiler(abc.ABC):
    def __init__(self):
        pass

    @classmethod
    def add_compiler_subparser(cls, subparsers: argparse._SubParsersAction):
        compiler_parser = subparsers.add_parser(cls.get_compiler_name())
        cls.add_compiler_specific_options(compiler_parser)
        compiler_parser.set_defaults(func=cls.run)

    @classmethod
    @abc.abstractmethod
    def get_compiler_name(cls):
        raise NotImplementedError

    @classmethod
    @abc.abstractmethod
    def get_compiler_description(cls):
        raise NotImplementedError

    @classmethod
    @abc.abstractmethod
    def add_compiler_specific_options(cls, parser: argparse.ArgumentParser):
        raise NotImplementedError

    @classmethod
    @abc.abstractmethod
    def run(cls,
            input_dir_path: pathlib.Path,
            output_dir_path: pathlib.Path,
            force_overwrite: bool,
            args) -> getattr(typing, 'NoReturn', typing.Any):
        raise NotImplementedError

