Module IrisBackendv3.data_standards.gsw_metadata_tools
======================================================
This module contains tools which serve as the standard bearer for how 
GSW-specific metadata is embedded in the FPrime XML.

@author: Connor W. Colombo (CMU)
@last-updated: 04/05/2021

Functions
---------

    
`build(metadata: str) ‑> str`
:   Build a GSW-specific metadata comment string out of the given encoded metadata string.

    
`extract_from_str(comment: str) ‑> str`
:   Extract the GSW-specific metadata stored in the given comment string.

    
`extract_from_xml(node: <cyfunction Element at 0x11083a260>) ‑> str`
:   Extract the GSW-specific metadata from the given XML node (eg. `<command>` or `<arg>`).

    
`insert_into_xml(node: <cyfunction Element at 0x11083a260>, metadata: str) ‑> None`
:   Format and insert the given metadata into the comment of the given XML node as CDATA.
    
    If the element doesn't have a comment yet, one will be added.