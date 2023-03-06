from typing import TypeAlias, Dict, List

from IrisBackendv3.codec.files.file import File, HashedId
from IrisBackendv3.codec.packet_classes.packet import Packet


class FileManager:
    in_progress_files: Dict[HashedId, File]

    def process_packet(packet: Packet) -> bool:
        """ Extracts any file blocks from the given packet and uses them to
        build or continue building a file.
        """
        raise NotImplementedError()

    def save_all_files()
