"""
Methods for handling & archiving commands.
Processing of uplinked data.

@author: Colombo, CMU
@last-updated: 10/03/2024
"""

from typing import Any, Dict, List, Tuple, cast

from dataclasses import dataclass, field
from datetime import datetime

from yamcs.tmtc.model import CommandHistory  # type: ignore

from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection
from IrisBackendv3.codec.packet import IrisCommonPacket, parse_packet
from IrisBackendv3.codec.metadata import UplinkTimes, DataPathway
from IrisBackendv3.codec.payload import CommandPayload
from IrisBackendv3.data_standards.module import Command
from IrisBackendv3.config.command_aliases import CommandAliasesTable

from IrisBackendv3.storage.settings import settings
from IrisBackendv3.storage.logs import logger
from IrisBackendv3.storage.dataset import DataSet
from IrisBackendv3.storage.formatting import format_time, bytes_to_str

import numpy as np
import pandas as pd  # type: ignore

from rich.progress import track


@dataclass
class YamcsCommandMetadata:
    """Container for all Command metadata unique to YAMCS (not used in GDS):"""
    # YAMCS User who sent this command:
    user: str | None = None
    # YAMCS XTCE Name (from ICD):
    xtce_name: str | None = None
    # Approval Queue this got placed in in YAMCS:
    queue: str | None = None
    # Comment sent to YAMCS alongside this command:
    comment: str | None = None


def _build_alias_reverse_lookup_table() -> Dict[bytes, CommandAliasesTable.Entry]:
    """Build a reverse-lookup table for all aliases to find if and what alias
    could have been used to generate this command."""
    # Load default aliases table from default file:
    aliases = CommandAliasesTable.load_from_file()
    alias_reverse_table: Dict[bytes, CommandAliasesTable.Entry] = dict()
    for alias in aliases._table.values():
        packet = IrisCommonPacket(
            seq_num=0,  # doesn't matter, we'll strip this off next
            payloads=EnhancedPayloadCollection(
                CommandPayload=[alias.compiled_cmd]
            )
        )
        # Reference according to packet data, ignoring CommonPacketHeader
        # (since `seq_num` is variable):
        ref = packet.encode()
        ref = ref[IrisCommonPacket.CommonPacketHeader.CPH_LENGTH():]
        alias_reverse_table[ref] = alias
    return alias_reverse_table


def _get_yamcs_uplink_times(cmd: CommandHistory) -> UplinkTimes:
    """Extracts all relevant uplink times from a YAMCS command log."""
    ult = UplinkTimes()
    # Don't have: generated, pmcc_rx, pmcc_tx.
    # Technically could recover ack_rover but that would take a lot.
    if str(cmd.attributes.get('Acknowledge_Queued_Status')).upper() == 'OK':
        ult.amcc_rx = cmd.attributes.get('Acknowledge_Queued_Time')
    if str(cmd.attributes.get('Acknowledge_Released_Status')).upper() == 'OK':
        ult.amcc_ok = cmd.attributes.get('Acknowledge_Released_Time')
    if str(cmd.attributes.get('SLE_RADIATED_Status')).upper() == 'OK':
        ult.amcc_tx = cmd.attributes.get('SLE_RADIATED_Time')
    if str(cmd.attributes.get('Acknowledge_Sent_Status')).upper() == 'OK':
        ult.ack_lander = cmd.attributes.get('Acknowledge_Sent_Time')
    return ult


def extract_data_from_yamcs(
    yamcs_cmds: List[CommandHistory]
) -> List[Tuple[CommandPayload, YamcsCommandMetadata]]:
    """Extracts Command Data from a YAMCS CommandHistory:"""
    # Build table to reverse lookup any aliases used:
    alias_reverse_table = _build_alias_reverse_lookup_table()

    extracted: List[Tuple[CommandPayload, YamcsCommandMetadata]] = []
    for yamcs_cmd in track(yamcs_cmds, "Reconstructing Commands"):
        # Extract all the basic data:
        yamcs_data = YamcsCommandMetadata(
            user=yamcs_cmd.username,
            xtce_name=yamcs_cmd.name,
            queue=yamcs_cmd.attributes.get('queue'),
            comment=yamcs_cmd.comment
        )

        # Reverse Engineer the Command Packet:
        tc_binary_str = yamcs_cmd.source.split('tc-binary:')[-1][:-1].strip()
        packet_bytes = bytes.fromhex(tc_binary_str)
        ul_packet = parse_packet(packet_bytes)
        commands = [*ul_packet.payloads[CommandPayload]]

        # Generate metadata common to the packet:
        pathway_suffix = '_'+yamcs_cmd.name.split('_')[-1]
        pathway = {
            dp.yamcs_suffix: dp for dp in DataPathway
        }.get(pathway_suffix, DataPathway.NONE)

        # Perform reverse binary lookup to see if this was generated from an
        # alias:
        ref = packet_bytes[IrisCommonPacket.CommonPacketHeader.CPH_LENGTH():]
        alias = alias_reverse_table.get(ref, None)
        alias_name = None if alias is None else alias.alias

        for command in commands:
            command = cast(CommandPayload, command)
            # Add back in any relevant metadata:
            command.pathway = pathway
            command.amcc_ack = True  # inherently AMCC got this
            command.alias_name = alias_name
            if command.uplink_times is None:
                command.uplink_times = UplinkTimes()
            command.uplink_times.pull_from(_get_yamcs_uplink_times(yamcs_cmd))
            extracted.append((command, yamcs_data))

    return extracted


def generate_command_row(
    cmd: CommandPayload,
    yamcs_data: YamcsCommandMetadata
) -> pd.DataFrame:
    """Generates a row for the Archive table from relevant command data."""
    if cmd.uplink_times is None:
        # Something broke. We have nothing to return here.
        logger.warning(f"Failed to log {cmd=} without any `UplinkTimes`.")
        return pd.DataFrame()
    if cmd.uplink_times.amcc_rx is None:
        # Something broke. We have nothing to return here.
        logger.warning(
            f"Failed to log {cmd=} without an `amcc_rx` time for indexing.")
        return pd.DataFrame()
    df = pd.DataFrame(
        data=[{
            'Time1_queued': format_time(cmd.uplink_times.amcc_rx),
            'Time2_approved': format_time(cmd.uplink_times.amcc_ok),
            'Time3_radiated': format_time(cmd.uplink_times.amcc_tx),
            'Time4_onLander': format_time(cmd.uplink_times.ack_lander),
            'alias': cmd.alias_name,
            'command': repr(cmd),
            'command_type': cmd.__class__.__name__,
            'data': bytes_to_str(cmd.raw),
            'data_size': len(cmd.raw) if cmd.raw is not None else 0,
            'pathway': cmd.pathway.name if cmd.pathway is not None else np.nan,
            'source': cmd.source.name if cmd.source is not None else np.nan,
            'yamcs_User': yamcs_data.user,
            'yamcs_Name': yamcs_data.xtce_name,
            'yamcs_Queue': yamcs_data.queue,
            'yamcs_comment': yamcs_data.comment,
        }],
        index=format_time([cmd.uplink_times.amcc_rx])
    )
    df.index.name = settings['TIME_COL_NAME']
    return df


def yamcs_history_to_commands(yamcs_cmds: List[CommandHistory]) -> DataSet:
    """Produces a DataSet of all commands and relevant metadata logged in YAMCS.
    All data is sorted chronologically.
    """
    logger.debug(
        f"Reconstructing command data from {len(yamcs_cmds)} YAMCS entries . . . "
    )
    extracted = extract_data_from_yamcs(yamcs_cmds)

    # Extract all telem from each packet and add in any generated metafields:
    logger.debug(
        f"Formatting data of {len(extracted)} extracted commands . . . "
    )
    cmd_rows = [
        generate_command_row(cmd, yamcs_data)
        for cmd, yamcs_data in track(
            extracted, "[blue]Creating archive entries", total=len(extracted)
        )
    ]

    logger.debug("Joining command rows . . . ")
    data = pd.concat([*cmd_rows], axis=0, ignore_index=False)
    # Aggregate by time, joining multiple rows with the same timestamp:
    data.index.name = settings['TIME_COL_NAME']

    logger.debug(f"Building dataset from {data.shape} df . . . ")
    if len(cmd_rows) > 0:
        return DataSet(data)
    else:
        return DataSet(pd.DataFrame())
