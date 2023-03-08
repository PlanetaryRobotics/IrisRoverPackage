from typing import TypeAlias, Dict, List, Final, ClassVar

from dataclasses import dataclass

from IrisBackendv3.codec.payload import FileMetadata, FileBlockPayload

HashedId: TypeAlias = int


class File:
    _FILE_EXT: ClassVar[str] = '.iris.hex'

    _metadata: FileMetadata

    _blocks: Dict[int, FileBlockPayload]

    @property
    def hashedId(self) -> HashedId:
        return self._metadata.hashedId

    def ingest_block(block: FileBlockPayload) -> None:
        pass

    def generate_file_contents() -> bytes:
        # TODO: Make a cached property?
        pass

    def save_file(self, path: str): pass
