"""
Registry of pre-"built" (pre-specified) Transceivers that can be built given
just a name (useful for referring to a particular Transceiver build by name in
a CLI argument).
"""

from IrisBackendv3.transceiver.transceiver import Transceiver
from IrisBackendv3.transceiver.slip_transceiver import SlipTransceiver
from IrisBackendv3.transceiver.pcap_transceiver import PcapTransceiver, PcapParseOpts

from IrisBackendv3.transceiver.endec import UnityEndec


def build_xcvr_by_name(name: str, **kwargs) -> Transceiver:
    """Builds a pre-specified Transceiver instance based on the name given.
    Optionally, `kwargs` can be given to override certain default settings of
    the prebuilt transceiver.
    """
    match name.upper():
        case 'PCAP-18H':
            default_kwargs = dict(
                fixed_period_ms=1000,
                loop=False,
                endecs=[UnityEndec()]
            )
            # Combine `default_kwargs` with `kwargs`, overriding any
            # `default_kwargs` with given `kwargs`:
            kwargs = {**default_kwargs, **kwargs}
            return PcapTransceiver(
                PcapParseOpts(
                    pcap_file='./test-data/Iris_210503_18_hours_of_telem.pcap',
                    filter_port='any',
                    filter_protocol=None,
                    packetgap=kwargs.get('packetgap', 0),
                    deadspace=kwargs.get('deadspace', 0),
                    logging_level='INFO'
                ),
                **kwargs
            )

    # Nothing matched... raise an error:
    raise ValueError(
        f"Transceiver {name=} doesn't match any pre-specified Transceiver."
        f"Check `build_xcvr_by_name` for a list of all pre-specified "
        f"Transceivers you can use."
    )
