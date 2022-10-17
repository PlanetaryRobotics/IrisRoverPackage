import sys
import dpkt
import numpy as np
import matplotlib.pyplot as plt

image_width = 2592
image_height = 1944

image = []
with open(sys.argv[1], 'rb') as f:
    for ts, pkt in dpkt.pcap.Reader(f):
        frame = dpkt.ethernet.Ethernet(pkt)
        ip_dg = frame.data
        udp_dg = ip_dg.data
        image_block = udp_dg.data
        image_block = np.frombuffer(image_block, dtype=np.uint8)[4+9+3:]
        image.append(image_block)
image_buf = np.zeros(image_height * image_width)
image_data = np.array(image).flatten()
image_buf[:image_data.shape[0]] = image_data
image = image_buf.reshape((image_height, image_width))
# max_px_num = len(image_data) // image_width * image_width
# image = image_data[:max_px_num].reshape((-1, image_width))
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
