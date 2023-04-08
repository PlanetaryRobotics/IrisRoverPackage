Module IrisBackendv3.utils.crc
==============================
A Collection of Basic CRC Utility Functions.

@author: Connor W. Colombo (CMU)
@last-updated: 11/12/2022

Functions
---------

    
`crc32_fsw(bufferData: bytes) ‑> int`
:   Exact clone of the CRC32 algorithm used in Iris FSW (specifically, code
    written by Iris - not FPrime or OS provided code. Namely, this is used in
    the NetworkManager at the time of writing.
    
     Computes the CRC32 of the given buffer.
     Runs in ~ 6 + 38*n (best of evaluated non-lookup-table-based methods).
     This is the only we'll use a CRC32 in the entire codebase, so it's not
     worth devoting precious RAM to it (if calculating on first call as in
     CRC32.hpp) and storing a giant static lookup table in flash is just
     asking for a cosmic bitflip to make this entire operation not work.
    
     NOTE: For BGAPI Passthrough, GSW should use this EXACT algorithm for
     CRC32 compute.
    
     NOTE: FSW-counterpart found in `fprime/CubeRover/NetworkManager/NetworkManager.cpp`.
    
     Derived from: https://web.archive.org/web/20190108202303/http://www.hackersdelight.org/hdcodetxt/crc.c.txt
     License: https://web.archive.org/web/20190716204559/http://www.hackersdelight.org/permissions.htm
    
    
    # Doctest examples confirmed to sync up with FSW results:
    >>> crc32_fsw(b'ImNetMan')
    2548205386
    >>> crc32_fsw(b'NetMan')
    2679180643
    >>> crc32_fsw(b'H')
    2852464175
    >>> crc32_fsw(b'HELLOmoonThisIsALongString')
    273146347
    
    # NOTE: \ are necessary below for the escape sequences to get into doctest:
    # (these are testing [0x48] and [0xDE, 0xAD, 0xBE, 0xEF] respectively)
    
    >>> crc32_fsw(b'\x48')
    2852464175
    
    >>> crc32_fsw(b'\xDE\xAD\xBE\xEF')
    2090640218

    
`uint32_t(x: int)`
: