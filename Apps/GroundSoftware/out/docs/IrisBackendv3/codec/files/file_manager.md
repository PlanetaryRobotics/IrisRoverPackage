Module IrisBackendv3.codec.files.file_manager
=============================================

Classes
-------

`FileManager()`
:   

    ### Class variables

    `in_progress_files: Dict[int, IrisBackendv3.codec.files.file.File]`
    :

    ### Methods

    `process_packet(packet: IrisBackendv3.codec.packet_classes.packet.Packet) ‑> bool`
    :   Extracts any file blocks from the given packet and uses them to
        build or continue building a file.

    `save_all_files()`
    :