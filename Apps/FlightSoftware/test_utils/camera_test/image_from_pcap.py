import dpkt
import numpy as np
import matplotlib.pyplot as plt

image = []
with open('12-23_dataonly.pcap', 'rb') as f:
    for ts, pkt in dpkt.pcap.Reader(f):
        frame = dpkt.ethernet.Ethernet(pkt)
        ip_dg = frame.data
        udp_dg = ip_dg.data
        image_block = udp_dg.data
        image_block = np.frombuffer(image_block, dtype=np.uint8)[5+9+3:]
        image.append(image_block)
image = np.array(image)
image = image.reshape((1944, 2592))
plt.imshow(image, cmap='gray')
plt.show()



n_interleave = 8
im = []
for i in range(int(image.shape[0] / n_interleave)):
    for n in range(n_interleave):
        im.append(image[int(image.shape[0] / n_interleave * n  + i)])
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
