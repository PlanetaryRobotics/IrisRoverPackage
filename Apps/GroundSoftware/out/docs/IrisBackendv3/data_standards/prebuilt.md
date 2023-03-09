Module IrisBackendv3.data_standards.prebuilt
============================================
Special prebuilt data standards which exist outside of the FPrime XML (e.g. for
watchdog heartbeat)

@author: Connor W. Colombo (CMU)
@last-updated: 03/04/2023

Functions
---------

    
`add_to_standards(standards: IrisBackendv3.data_standards.data_standards.DataStandards, modules: Union[IrisBackendv3.data_standards.module.Module, List[IrisBackendv3.data_standards.module.Module]]) ‑> None`
:   Adds the given prebuilt module to the given data so long as it doesn't
    override anything.

    
`nid_nic_entry(t: Type[Any], ID: int, name: str, *args, **kwargs) ‑> Tuple[Tuple[int, str], ~T]`
:   Makes a tuple which can be used as a name-id-containing `NameIdDict` entry
    where the `name` and `ID` are also parameters of the entry value in addition
    to being the keys in the NameIdDict (e.g. in `TelemetryChannels` or
    `Commands`).
    This convenience function prevents the `name` and `ID` from needing to be
    entered twice when manually instantiating a NameIdDict with
    name-id-containing entries, preventing them from possibly becoming out of
    sync.