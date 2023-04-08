from IrisBackendv3.transceiver.endec import *

from typing import cast, ClassVar

import re
import scapy.all as scp  # type: ignore # no type hints


# class _SlipEndec(Endec):
#     """Known-good Implementation of SLIP (Serial Line Internet Protocol) pulled
#     from Iris Console used during FSW bring-up.

#     NOTE: Although this class is used internally in the `SlipTransceiver` class,
#     you **don't**  need to (and shouldn't) include `SlipEndec` in the `endecs`
#     list for SlipTransceiver."""
#     pass


class SlipEndec(Endec):
    """ Implementation of SLIP (Serial Line Internet Protocol) per RFC-1055,
    based on the implementation in `sliplib 0.6.2` by Ruud de Jong.

    NOTE: Although this class is used internally in the `SlipTransceiver` class,
    you **don't**  need to (and shouldn't) include `SlipEndec` in the `endecs`
    list.
    """
    # Special SLIP encoding characters:
    B_END: ClassVar[bytes] = b'\xc0'
    B_ESC: ClassVar[bytes] = b'\xdb'
    B_ESC_END: ClassVar[bytes] = b'\xdc'
    B_ESC_ESC: ClassVar[bytes] = b'\xdd'

    def is_valid(self, data: bytes) -> bool:
        """ Returns whether the packet's contents conform to the SLIP spec.
        A packet is valid if:
        * It contains no `B_END` bytes other than leading and/or trailing `B_END` bytes
        * Each `B_ESC` byte is followed by either an `B_ESC_END` or an `B_ESC_ESC` byte.
        """
        data = data.strip(self.B_END)
        return not (
            self.B_END in data
            or data.endswith(self.B_ESC)
            or re.search(self.B_ESC + b'[^' + self.B_ESC_END + self.B_ESC_ESC + b']', data)
        )

    def encode(self, data: bytes) -> bytes:
        """ Encodes data to send over a `Transceiver`.

        Wraps the given data in SLIP.
        """
        return (
            self.B_END
            + (data.replace(self.B_ESC, self.B_ESC + self.B_ESC_ESC)
               .replace(self.B_END, self.B_ESC + self.B_ESC_END))
            + self.B_END
        )

    def decode(self, data: bytes) -> bytes:
        """ Decodes data sent over a `Transceiver`.
        """
        if not self.is_valid(data):
            raise TransceiverDecodingException(data, "Invalid SLIP packet.")
        return (
            data.strip(self.B_END)
            .replace(self.B_ESC + self.B_ESC_END, self.B_END)
            .replace(self.B_ESC + self.B_ESC_ESC, self.B_ESC)
        )
