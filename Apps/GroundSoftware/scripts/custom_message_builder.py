"""
Simple script to generate NoOp echo command binaries from messages.

@author: Connor W. Colombo (CMU)
@last-updated: 01/13/2024
"""
from typing import cast, Final, List, Dict, Type
from datetime import datetime, timedelta

import argparse
import time

from IrisBackendv3.logs import VALID_LOG_LEVELS

import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc
from IrisBackendv3.ipc.messages import (
    DownlinkedPayloadsMessage, DownlinkedPayloadsContent
)
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection
from IrisBackendv3.codec.payload import (
    TelemetryPayload, EventPayload, CommandPayload
)
from IrisBackendv3.codec.magic import Magic
from IrisBackendv3.codec.metadata import DataPathway, DataSource

from ipc_apps.dl_processor_lib.timestamping import RoverTimeEstimator

IB3.init_from_latest()

MESSAGES = [
    "PS: And believe me I an still alive",
    "PPS: I'm doing science and I'm still alive",
    "PPPS: I feel FANTASTIC and I'm still alive",
    "Hi Brooke & Ozzie! Love you! PS: Louie the Cat!",
    "Chief Scientist's Log Stardate 2024.1: The IRIS team works tirelessly to advance the mission",
    "to semi tim eden jordan ava nayrie emily hunar matt loveUtoTheMoonAndBack <3",
    "GMA & GD thanks for supporting me always! love you",
    "Congrats, Iris! Thanks Mom Dad Sam 1640",
    "Stay warm up there, little Iris! :) You've made CMU so proud with how far you've gone - Lance M.",
    "Mom, Rob, Rachel, Sandra, Luci, and Bruddas, I love you to lunar distance and back",
    "Your journey is a bit different than we wanted, but that's just payback for changing mine :) -ALD",
    "Hari\tMozhi, Kennedy, Gowthi. Of all in history, you three are the perfect family for me.",
    "Hello, friend",
    "Things worth doing are rarely easy - Cameron Harper",
    "I love u all(in strong American accent) ;)",
    "Let's believe in ourselves :)",
    "Das Ausserordentliche geschieht nicht auf gewoehnlichen Wegen (Goethe). Gruss aus Deutschland!",
    "Stella, Reach for the Stars,  Always attempt the impossible- Love you always.  Dad.",
    r"You miss 100% of the shots you don't take - Wayne Gretzky - Michael Scott - Anusha Maheshwari",
    "To Amber Huang, Joanna Cheung, Xia Feng, Weidong Huang, Ziyang Huang, my dear grandparents and my best friends, I wish you a happy and peaceful year in 2024.",
    "Peijia, enjoy this journey among the stars!",
    "'I don't know, give me ideas' - Misha Naga Gajula",
    "Ilovemoon.SHLeeYMLeeJHKimHLLee.Korea.TeamKKM",
    "Go Iris! -Sophia Zhao",
    "To my dad, I love you ",
    "Jake Tesler says Go forth, Live",
    "Long, and Prosper!",
    "Wishing the best to the Iris Lunar Rover and its future journey to where no man has been before and to its dedicated CMU student team from the Rao Family!",
    "To my dad, I would not be sending this message without you. To Ava, I love you.",
    "Peace should prevail in this world. Stop war.",
    "You made it Iris! Thanks for the most memorable 5 years with you and the team at CMU! Love, Divya Rao",
    "Looking for sublet, 1bed 1bath in Mtn View. send application at your earliest convenience. ",
    "Arjun Puri: friend to all, teacher to many, family to a fortunate few. This one's for you, Nana.",
    "Do not go gentle into that good night.",
    "'Rage, rage against the dying of the light.' Dylan Thomas",
    "H2P! Just as you exist in the stars, your name shall travel the stars too. I love you Deirdre, mom.",
    "Ad astra ~ Kyong, Peter, Isabel, Elizabeth, Victoria, Dan, Eric and Alessandra",
    "This is the first recorded message to space from a Singaporean. Majulah!",
    "Carnegie Mellon University is home to the most brilliant students in the universe.",
    "Hi Iris, have fun in space:) Thank you for all the memories! Ines and familia Rodriguez Hsu",
    "Bridging technology & humanity. May our pursuit inspire your journey.",
    "A haiku for a lunar rover \n\nProud of you Iris \nVisiting new horizons \nExploring the moon \n",
    "Congratulations Charlotte Michaluk, science talent search scholar!",
    "Shine on you crazy diamond! Love Aaron, Maggie and Windsor Aupperlee",
    "Be brave Iris more rovers will follow from your progenitors",
    "Congratulations to the Iris Team on their incredible mission adaptability!",
    "To the Talento and Geis families: Hello from Space! Love Carmyn <3",
    "A giant leap.",
    "LiYuZhen, ZhuAnKang, Greetings from the Moon!"
]

SENDERS = [
    "Iris",
    "Iris",
    "Iris",
    "Victoria Santiago",
    "Zachary Muraskin",
    "Arielle Min",
    "Shannon Case",
    "Siri Maley",
    "Lance Miller",
    "Alex Wittig",
    "Andrea Davis",
    "Hari Prasath John Kennedy",
    "Jeffery John",
    "Cameron Harper",
    "Perrin Tong",
    "Tejas Venkatesh",
    "Hendrik Dreger",
    "Tim Angert",
    "Anusha Maheshwari",
    "Ricky Huang",
    "Peiqi Liu",
    "Madhav Gajula",
    "Jimin Lee",
    "Sophia Zhao",
    "Nikolai Stefanov",
    "Jake Tesler",
    "Deepa Rao",
    "Nikolai Stefanov",
    "Leonard Vinnick",
    "Divya Rao",
    "Robert Rudolph",
    "Leela Cañuelas",
    "Joao Passos",
    "Stephanie Manasterski",
    "Alessandra Fleck",
    "Dhruva Reddy",
    "Farnam Jahanian",
    "Ines Rodriguez Hsu",
    "Dena Tsamitis - CMU INI",
    "Sonja Michaluk",
    "Sonja Michaluk ",
    "Aaron Aupperlee",
    "Kerry Quinn",
    "Bobby Gower",
    "Carmyn Talento",
    "Atharva Pusalkar",
    "Steve Wang"
]

# Setup:
app = ipc.IpcAppHelper("MessageBuilder")
manager = ipc.IpcAppManagerSync(socket_specs={
    'sub': ipc.SocketSpec(
        sock_type=ipc.SocketType.SUBSCRIBER,
        port=ipc.Port.TRANSCEIVER_SUB,
        topics=[ipc.Topic.DL_PAYLOADS]
    ),
})


parser = argparse.ArgumentParser(description=(
    'IRIS Lunar Rover — Send a Custom Message — CLI'
))


def get_opts(
    default_log_level: str = 'VERBOSE'
):
    parser.add_argument('-n', '--sent-by', type=str, default="Earth",
                        help="Name of person sending this message.")
    # parser.add_argument('-m', '--message', type=str, default="Hi, Iris!",
    #                     help="Custom message to be sent.")
    parser.add_argument('-d', '--roundtrip-delay-sec', type=float, default=0,
                        help="Amount to sleep to simulate roundtrip delay.")
    parser.add_argument('-f', '--file', type=str, default="./messages.xlsx",
                        help="Spreadsheet of custom message to load.")

    def str_to_log_level(s):
        if isinstance(s, str) and s.upper() in VALID_LOG_LEVELS:
            return s.upper()
        else:
            raise argparse.ArgumentTypeError(
                f'Valid log level expected. Log levels are: {VALID_LOG_LEVELS}'
            )

    parser.add_argument('--log-level', type=str_to_log_level, default=default_log_level,
                        help=('Logging level to be used (i.e. how annoying the logging '
                              'printouts should be). Only logs with importance greater '
                              'than or equal to the specified logging level are '
                              f'displayed. Valid logging levels are: {VALID_LOG_LEVELS}'))

    parsed_opts = parser.parse_known_args()
    opts = parsed_opts[0]
    opts.message = ' '.join(parsed_opts[1])
    return opts


def make_message(message: str) -> List[str]:
    COMMAND_NAME: Final[str] = "CommandDispatcher_Cmdnoopstring"
    MAX_MESSAGE_LEN: Final[int] = 39
    COUNTER_BUFFER_LEN: Final[int] = 4  # " X/Y"

    # Split into sub-messages as needed:
    sub_messages: List[str] = []
    if len(message) <= MAX_MESSAGE_LEN:
        sub_messages = [message]
    else:
        n_msgs: int = 0
        head: int = 0
        while head < len(message):
            sub_message_data = message[
                head:(head+MAX_MESSAGE_LEN-COUNTER_BUFFER_LEN)
            ]
            sub_message = sub_message_data + f" {n_msgs+1}/"
            sub_messages.append(sub_message)
            n_msgs += 1
            head += len(sub_message_data)
        sub_messages = [sm + str(n_msgs) for sm in sub_messages]

    standards = IB3.get_codec_standards()
    module, command = standards.global_command_lookup(COMMAND_NAME)
    command_payloads = [
        CommandPayload(
            pathway=DataPathway.WIRED,
            source=DataSource.GENERATED,
            magic=Magic.COMMAND,
            module_id=module.ID,
            command_id=command.ID,
            args=dict(arg_1=sm[:MAX_MESSAGE_LEN])
        )
        for sm in sub_messages
    ]

    # Build dummy uplink packet:
    packets = [
        IB3.codec.packet.IrisCommonPacket(
            seq_num=0,  # just using 0 for now
            payloads=IB3.codec.payload_collection.EnhancedPayloadCollection(
                CommandPayload=[command_payload]
            )
        )
        for command_payload in command_payloads
    ]

    return [
        sm + ": " + ''.join(f'{x:02x}' for x in packet.encode())
        for sm, packet in zip(sub_messages, packets)
    ]


def make_all_messages(opts):
    for sender, message in zip(SENDERS, MESSAGES):
        print(sender + ": ")
        for packet_str in make_message(message):
            print("\t " + packet_str)


def make_from_opts(opts):
    COMMAND_NAME: Final[str] = "CommandDispatcher_Cmdnoopstring"
    MAX_MESSAGE_LEN: Final[int] = 39
    COUNTER_BUFFER_LEN: Final[int] = 4  # " X/Y"

    sender = opts.sent_by
    message = opts.message
    app.logger.info(f"{sender}: '{message}'. Sending . . .")

    # Split into sub-messages as needed:
    sub_messages: List[str] = []
    if len(message) <= MAX_MESSAGE_LEN:
        sub_messages = [message]
    else:
        n_msgs: int = 0
        head: int = 0
        while head < len(message):
            sub_message_data = message[
                head:(head+MAX_MESSAGE_LEN-COUNTER_BUFFER_LEN)
            ]
            sub_message = sub_message_data + f" {n_msgs+1}/"
            sub_messages.append(sub_message)
            n_msgs += 1
            head += len(sub_message_data)
        sub_messages = [sm + str(n_msgs) for sm in sub_messages]

    standards = IB3.get_codec_standards()
    module, command = standards.global_command_lookup(COMMAND_NAME)
    command_payloads = [
        CommandPayload(
            pathway=DataPathway.WIRED,
            source=DataSource.GENERATED,
            magic=Magic.COMMAND,
            module_id=module.ID,
            command_id=command.ID,
            args=dict(arg_1=sm[:MAX_MESSAGE_LEN])
        )
        for sm in sub_messages
    ]

    # Build dummy uplink packet:
    packets = [
        IB3.codec.packet.IrisCommonPacket(
            seq_num=0,  # just using 0 for now
            payloads=IB3.codec.payload_collection.EnhancedPayloadCollection(
                CommandPayload=[command_payload]
            )
        )
        for command_payload in command_payloads
    ]

    for sub_message, command, packet in zip(sub_messages, command_payloads, packets):
        time.sleep(opts.roundtrip_delay_sec)
        pkt_data_str = ''.join(f'{x:02x}' for x in packet.encode())
        pathway_str = command.pathway.yamcs_suffix
        app.logger.success(
            f"\t > {sender}: \t '{sub_message}' \t !"
        )
        app.logger.verbose(
            f"\t\t -> \t {command}"
        )
        app.logger.verbose(
            f"\t\t -> {command.command.name}_{pathway_str}"
        )
        app.logger.verbose(
            f"\t\t -> 0x {pkt_data_str}"
        )
    app.logger.notice("MESSAGES COMPLETE.")


# Run:
if __name__ == "__main__":
    opts = get_opts()
    app.setLogLevel(opts.log_level)

    make_all_messages(opts)
    print('\n\n\n')
    make_from_opts(opts)
