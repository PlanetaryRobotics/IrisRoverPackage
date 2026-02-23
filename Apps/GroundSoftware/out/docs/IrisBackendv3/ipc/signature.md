Module IrisBackendv3.ipc.signature
==================================
Tools for signing IPC messages and checking the integrity of signed messages.

@author: Connor W. Colombo (CMU)
@last-updated: 02/27/2022

Functions
---------

    
`add_digest(msg: bytes) ‑> bytes`
:   Creates a signed message by adding a digest to a message to allow the 
    receiver to check the integrity of the message.
    
    NOTE: The `SESSION_KEY` IPC settings variable will have to be the same one 
    used when the message is ultimately received and checked.

    
`compute_digest(msg: bytes) ‑> bytes`
:   Computes the digest of the given message `msg`.

    
`extract_message_and_check_integrity(signed_msg: bytes) ‑> bytes`
:   Separates the digest and message in a signed message.
    
    NOTE: The `SESSION_KEY` IPC settings variable will have to be the same one 
    used when the message was created.
    
    Args:
        signed_msg (bytes): A signed message consisting of a signature digest and 
    
    Raises:
        MessageIntegrityError: if integrity issues are present in the signed message
    
    Returns:
        bytes: The core message w/out the digest (if there are no integrity issues with the signed message)