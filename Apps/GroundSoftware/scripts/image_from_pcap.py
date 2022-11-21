from typing import cast

import numpy as np
import matplotlib.pyplot as plt  # type: ignore

from termcolor import cprint

from IrisBackendv3.codec.payload import FileBlockPayload
from parse_pcap import parse_pcap, get_opts as get_pcap_opts

image_width = 2592
image_height = 1944

if __name__ == "__main__":
    image = []
    pcap_opts = get_pcap_opts(
        description='IRIS Lunar Rover — GSW Backend — PCAP Image Extraction Tool',
        default_port='any',
        default_cli_log=False,
        default_plot=False,
        default_log_level='CRITICAL'
    )

    # if debugging_mode := True:
    #     cprint("WARNING. ARGS ARE BEING MODIFIED BECAUSE DEBUG MODE IS ACTIVE", 'red')
    #     # pcap_opts.pcap_file = './test-data/Iris_Image_Downlink_201223-ImageDLTest_FixedLens.pcapng'
    #     pcap_opts.pcap_file = './test-data/CameraTesting-pcaps/0406_camera1_test.pcap'

    pcap_data = parse_pcap(pcap_opts)

    corrupted_fileblock_count = sum(
        1 for p in pcap_data['all_payloads'].FileBlockPayload if p.possible_corruption
    )
    if corrupted_fileblock_count > 0:
        cprint(
            f"WARNING: {corrupted_fileblock_count} of the {len(pcap_data['all_payloads'].FileBlockPayload)} "
            "file blocks show evidence of possible corruption but were still buildable, "
            "though image size and content may be affected. Make sure file block headers "
            "and metadata file blocks are being built correctly in the FSW.",
            'red')

    for fb in pcap_data['all_payloads'].FileBlockPayload:
        fb = cast(FileBlockPayload, fb)
        if fb.block_number > 0:
            # if not a metadata block
            image_block = np.frombuffer(fb.data, dtype=np.uint8)
            image.append(image_block)
    image_buf = np.zeros(image_height * image_width)
    image_data = np.array(image).flatten()
    image_buf[:image_data.shape[0]] = image_data
    image = image_buf.reshape((image_height, image_width))
    plt.imshow(image, cmap='gray')
    plt.show()

    n_interleave = 8
    im = []
    for i in range(int(image.shape[0] / n_interleave)):
        for n in range(n_interleave):
            im.append(image[int(image.shape[0] / n_interleave * n + i)])
    im = np.array(im)
    plt.imshow(im, cmap='gray')
    plt.show()

    # Rearrage the four quadrants
    splity, splitx = 1748, 1878
    br = im[:splity, :splitx]
    tr = im[splity:, :splitx]
    bl = im[:splity, splitx:]
    tl = im[splity:, splitx:]

    picture = np.ndarray(image.shape, dtype=np.uint8)
    picture[:tl.shape[0], :tl.shape[1]] = tl
    picture[:tr.shape[0], tl.shape[1]:] = tr
    picture[tr.shape[0]:, tl.shape[1]:] = br
    picture[tl.shape[0]:, :tl.shape[1]] = bl
    plt.imshow(picture, cmap='gray')
    plt.show()

    # Get rid of every 8th row starting from index 3 (all the white rows)
    pic = []
    i = -3
    for row in picture:
        if i % 8 in [0, 1]:
            pass
        else:
            pic.append(row)
        i += 1

    plt.imshow(pic, cmap='gray')
    plt.show()
