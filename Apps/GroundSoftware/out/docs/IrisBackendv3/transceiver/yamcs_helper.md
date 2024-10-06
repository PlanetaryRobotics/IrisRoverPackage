Module IrisBackendv3.transceiver.yamcs_helper
=============================================
Helper for interfacing with data from YAMCS.

Specifically, for inter-operating between Iris' data formatting and YAMCS's
formatting.

@author: Connor W. Colombo (CMU)
@last-updated: 09/30/2024

Functions
---------

    
`build_mock_yamcs_parameter(name: str, reception_time: datetime.datetime, generation_time: datetime.datetime, eng_value: Any, raw_value: Optional[Any] = None) ‑> yamcs.tmtc.model.ParameterValue`
:   Builds a mock YAMCS parameter with only the information we care about
    (for passing around internally to things that ingest YAMCS parameters.)

    
`get_peregrine_module() ‑> IrisBackendv3.data_standards.module.Module`
:   Gets the DataStandards Module for Peregrine telemetry from the live
    datastandards (which is why this can't just be evaluated as a constant).

    
`iris_telem_param_to_packet(param: yamcs.tmtc.model.ParameterValue, rx_is_generation_time: bool = False) ‑> IrisBackendv3.codec.packet_classes.packet.Packet`
:   Converts the given YAMCS param of Iris Telemetry into the Iris Packet it
    represents.

    
`peregrine_telem_params_to_packet(params: List[yamcs.tmtc.model.ParameterValue], rx_is_generation_time: bool = False) ‑> IrisBackendv3.codec.packet_classes.peregrine_dummy_packet.PeregrineDummyPacket`
:   Extract relevant data and metadata from the given YAMCS parameters of
    Peregrine telemetry and wraps the extracted data in a PeregrineDummyPacket.

Classes
-------

`YamcsInterface(server: str, port: int, instance: str, processor: str, username: str | None = None, password: str | None = None, tls: bool = True, auto_begin: bool = True, **_)`
:   Wrapper for common YAMCS interface tasks.

    ### Class variables

    `archive: Optional[yamcs.archive.client.ArchiveClient]`
    :

    `client: Optional[yamcs.client.YamcsClient]`
    :

    `mdb: Optional[yamcs.mdb.client.MDBClient]`
    :

    `processor: Optional[yamcs.tmtc.client.ProcessorClient]`
    :

    ### Methods

    `begin(self) ‑> None`
    :

    `get_param_datapoints(self, name: str, start: datetime.datetime | None = None, stop: datetime.datetime | None = None, descending: bool = False) ‑> List[Tuple[datetime.datetime, Any]]`
    :   Creates a list of (generation_time, eng_value) datapoint tuples for
        the parameter with the given name from the archive between the
        specified start and stop time (or all time if not specified).
        
        Values are returned in descending order (most recent first).
        
        :param name: parameter to query.
        :type name: str
        
        :param start: Datetime to start looking, defaults to `None`
            (beginning of time).
        
        :type start: Optional[datetime], optional
        :param stop: Datetime to stop looking, defaults to `None`
            (end of time).
        :type stop: Optional[datetime], optional
        :param descending: Most recent first if `True`, most recent last if
            `False`, defaults to `False`.
        :type descending: bool, optional
        :return: List of all parameter values matching the query.
        :rtype: List[ParameterValue]

    `get_param_history(self, name: str, start: datetime.datetime | None = None, stop: datetime.datetime | None = None, descending: bool = False) ‑> List[yamcs.tmtc.model.ParameterValue]`
    :   Gets a list of values for the parameter with the given name from
        the archive between the specified start and stop time (or all time if
        not specified).
        
        Values are returned in descending order (most recent first).
        
        :param name: parameter to query.
        :type name: str
        :param start: Datetime to start looking, defaults to `None`
            (beginning of time).
        :type start: Optional[datetime], optional
        :param stop: Datetime to stop looking, defaults to `None`
            (end of time).
        
        :type stop: Optional[datetime], optional
        :param descending: Most recent first if `True`, most recent last if
            `False`, defaults to `False`.
        
        :type descending: bool, optional
        :return: List of all parameter values matching the query.
        :rtype: List[ParameterValue]

    `get_param_names(self) ‑> List[str]`
    :   Helper function that returns a list of the qualified names of all
        parameters available in the MDB.