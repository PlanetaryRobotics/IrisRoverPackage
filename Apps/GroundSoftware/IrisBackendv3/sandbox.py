#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Nov 20 12:34:45 2020.

@author: connorcolombo
"""

import scapy.all as scp
import struct
import numpy as np
import matplotlib.pyplot as plt

if __name__ == "__main__":
    from utils import print_bytearray_hex as printraw

    ## /##
    # SETTINGS:
    ####
    # Data Transport:
    file = '../test-data/Iris_Image_Downlink_201201_Trial2.pcap'  # PCAP logs
    protocol = scp.UDP  # Protocol the SBC is using to send data
    port = 8080  # Port on the spacecraft the SBC is sending data to

    # Data Formatting Settings:
    packetgap = 1  # number of packets to ignore at beginning of pcap
    deadspace = 0  # number of bytes of deadspace at the beginning of the
    endianness_code = "<"  # < = little, > = big, ! = network

    # Image Formatting Settings:
    image_settings = {
        "width": 2592,
        "height": 1944
    }

    # Magics:
    file_magic = bytearray([0xda, 0xba, 0xd0, 0x00])
    if endianness_code == '<':
        file_magic = bytearray([0x00, 0xd0, 0xba, 0xda])  # Little Endian it.

    def build_image_from_ICP(packet_bytes, image, image_index, block_number_list):
        # Extract and Parse the Common Packet Header:
        # SeqNum (1B) + Length (2B) + Checksum (2B)
        CPH = packet_bytes[:5]
        seq_num, vlp_len, checksum = struct.unpack(
            endianness_code+' B H 2s', CPH)

        # Extract the Variable Length Payload
        VLP = packet_bytes[5:]

        # Parse VLP:
        object_count = 0
        block_count = float('inf')
        """
        while len(VLP) > 0:
            # Strip off the magic:
            magic, VLP = VLP[:4], VLP[4:]
            if magic != file_magic:
                raise Exception(
                    (
                    "Magic wrong for object {} in VLP. Expected only file blocks "
                    "(magic 0x{}) but found 0x{})"
                    ).format(object_count, file_magic.hex(), magic.hex())
                )
        
            object_count += 1
        """

        # Search for first file magic (0xDABAD0000):
        while len(VLP) > 0 and VLP[:4] != file_magic:
            VLP = VLP[1:]  # Strip off first bit

        while len(VLP) > 0 and image_index < image.size:
            # Once found, Strip off the magic:
            magic, VLP = VLP[:4], VLP[4:]
            # Then extract header:
            header, VLP = VLP[:5], VLP[5:]
            hashedId, totalBlocks, blockNumber, blockLen = struct.unpack(
                endianness_code+' H B B B', header)
            block_number_list.append(blockNumber)

            # Start building the file until end of packet, another file magic (0xDABAD0000) is encountered or image is full:
            while len(VLP) > 0 and VLP[:4] != file_magic and image_index < image.size:
                # Strip off first bit into image
                image[image_index], VLP = VLP[0], VLP[1:]
                image_index = image_index + 1

        return image_index, block_number_list

    def main():
        # Setup image:
        image = np.zeros(image_settings['width'] * image_settings['height'])

        # Read pcap File:
        pcap = scp.rdpcap(file)

        packets = list(filter(lambda x: x.dport == port,
                       pcap[protocol][packetgap:]))

        # Build image:
        image_index = 0
        block_number_list = []
        for idx, packet in enumerate(packets):
            # scp.hexdump(packet) # print entire packet as a hexdump
            # udp_contents = packet.getlayer(scp.UDP) # strip off everything before the UDP header
            content = scp.raw(packet.getlayer(scp.Raw))[deadspace:]
            printraw(content)
            image_index, block_number_list = build_image_from_ICP(
                content, image, image_index, block_number_list)
            print("{}/{} packets used.".format(idx+1, len(packets)))
            if image_index >= image.size:
                break

        # Display Image:
        print("{}/{} pixels filled".format(image_index+1, image.size))
        image = image / np.max(image)
        image_w = image.reshape(-1, image_settings['width'])
        plt.figure()
        plt.imshow(image_w, cmap=plt.cm.gray)
        plt.show()

        image_h = image.reshape(image_settings['height'], -1).T
        plt.figure()
        plt.imshow(image_h, cmap=plt.cm.gray)
        plt.show()

        bnl = np.asarray(block_number_list)
        plt.figure()
        plt.plot(np.arange(bnl.size), bnl)
        plt.show()

    if __name__ == "__main__":
        main()
