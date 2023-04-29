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
                    packetgap=kwargs.get('packetgap', 37000),
                    deadspace=kwargs.get('deadspace', 0),
                    logging_level='INFO'
                ),
                **kwargs
            )
        case 'PCAP-RC9.5.3-2MIN-IMU':  # 2 Minutes of RC9.5.3 telem w/ IMU movement
            default_kwargs = dict(
                fixed_period_ms=588,  # avg. period (204 packets in 120s)
                loop=True,  # loop this to provide unlimited sample telem
                endecs=[UnityEndec()]
            )
            kwargs = {**default_kwargs, **kwargs}
            return PcapTransceiver(
                PcapParseOpts(
                    pcap_file='./test-data/telem_rc9_9.5.3_200packets_w_IMU.pcap',
                    filter_port='any',
                    filter_protocol=None,
                    packetgap=kwargs.get('packetgap', 0),
                    deadspace=kwargs.get('deadspace', 0),
                    logging_level='INFO'
                ),
                **kwargs
            )
        case 'IMG-GRID-500':
            default_kwargs = dict(
                fixed_period_ms=200,
                loop=False,
                endecs=[UnityEndec()]
            )
            # Combine `default_kwargs` with `kwargs`, overriding any
            # `default_kwargs` with given `kwargs`:
            kwargs = {**default_kwargs, **kwargs}
            return PcapTransceiver(
                PcapParseOpts(
                    # ___img_8_0_21.5__with_RDL_prints_at_each_state_load__no_crash.pcap',
                    # pcap_file='./test-data/___img_final_sm_w_ilock_loss_flush_handler.pcap',
                    # pcap_file='./test-data/___img_new_gi_dl_test__and_fail.pcap',
                    # pcap_file='./test-data/____real_img_via_flash_grid_test.pcap', # ! real flash grid
                    # pcap_file='./test-data/____real_img_via_flash_cam1.pcap', # !! real image
                    # pcap_file='./test-data/____real_img_via_flash_cam0.pcap',  # !! real image ( but shitty - maybe bad camera)
                    # pcap_file='./test-data/____real_img_via_flash_cam1.pcap', # !! real image
                    # pcap_file='./test-data/____real_img_via_flash_post_img_grid_test_to_test_clear.pcap',  # !! real image
                    # !! real image of real grid
                    pcap_file='./test-data/____real_img_of_real_grid_cam1_w_herc_mon_on.pcap',
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
