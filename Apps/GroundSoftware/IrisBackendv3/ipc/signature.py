"""
Tools for signing IPC messages and checking the integrity of signed messages.

@author: Connor W. Colombo (CMU)
@last-updated: 02/27/2022
"""
from typing import Final

import hmac
import hashlib

from .settings import settings
from .exceptions import MessageIntegrityError

# Delimited used between digest and message in a signed message:
DIGEST_DELIMITER: Final[bytes] = b' '


def compute_digest(msg: bytes) -> bytes:
    """ Computes the digest of the given message `msg`.
    """
    # use hexdigest (str) instead of digest (bytes) since only alphanumerics will
    # appear so the boundary between digest and message can easily be delimited
    # using a non-alphanumeric (b' ')
    digest = hmac.new(
        key=bytes(settings['SESSION_KEY'], 'utf8'),
        msg=msg,
        digestmod=hashlib.sha256
    ).hexdigest()
    return bytes(digest, 'utf8')


def add_digest(msg: bytes) -> bytes:
    """ Creates a signed message by adding a digest to a message to allow the 
    receiver to check the integrity of the message.

    NOTE: The `SESSION_KEY` IPC settings variable will have to be the same one 
    used when the message is ultimately received and checked.

    """
    return compute_digest(msg) + DIGEST_DELIMITER + msg


def extract_message_and_check_integrity(signed_msg: bytes) -> bytes:
    """ Separates the digest and message in a signed message.

    NOTE: The `SESSION_KEY` IPC settings variable will have to be the same one 
    used when the message was created.

    Args:
        signed_msg (bytes): A signed message consisting of a signature digest and 

    Raises:
        MessageIntegrityError: if integrity issues are present in the signed message

    Returns:
        bytes: The core message w/out the digest (if there are no integrity issues with the signed message)
    """
    received_digest, msg = signed_msg.split(DIGEST_DELIMITER, 1)
    msg_digest = compute_digest(msg)

    if msg_digest != received_digest:
        raise MessageIntegrityError(
            f"""
            Integrity check failed for signed message: `{signed_msg!r}`.
            The received digest was: `{received_digest!r}`.
            The computed digest was: `{msg_digest!r}`.
            """
        )

    return msg
