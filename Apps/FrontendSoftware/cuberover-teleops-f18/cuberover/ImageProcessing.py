"""
Author: Jay Shaker
"""

import numpy as np
import time

"""
Image Processing Class
Would be a single function but it needs to maintain the state of all images.
Reads and stores images and returns them when it receives an end sequence

Inputs: Number of image bits per bytestring
"""
class ImageProccessing(object):

	def __init__(self, bits_received):
		self.image_dict = {} # Stores all images in play
		self.out_of_order = {} # Stores bytestring if recieved out of order of a start code
		self.bits_received = bits_received # Number of image bits recieved per image opcode
		self.timer = {}

	def end_image(self, picture):
		"""
		Completes the image

		Inputs: The image dictionary for an image
		Outputs: Numpy array of bit values for image
		"""
		pic = "".join(picture["im"])
		pic = " ".join(pic)
		#Each entry in the image dict is a string. The first join puts them together. 
		#The second one splits them because otherwise numpy would read it as just one huge number
		#of ones and zeros
		pic = np.fromstring(pic, sep=" ")
		pic = pic.reshape((picture["height"], picture["width"]))

		del self.image_dict[name]
		return pic

	def read_byte_string(self, byte_string):
		"""
		Reads, parses, and preforms actions based on the different bytes strings it receives. 
		Right now it does not deal with receiving extra data beyond what it handles. 

		Inputs: byte_string with format guide shown below

		Format:

			*constant* 
			[op_code](2 char) + [name](8 char)

			*start* (op_code = "01") 
			*constant* + [image_height](16 char) + [image_width](16 char)

			*image_packet* (op_code = "10")
			*constant* + [order](16 char) + [image_bits](self.bits_received char)

			*end* (op_code = "11")
			*constant*

		Outputs: Image if op_code = "11" and image has been completed
		"""

		keys = self.timer.keys():
		if keys:
			new_time = time.time()
			for key in keys:
				if new_time - self.timer[key] > 120:
					im = self.image_dict[name]["im"]
					im = ["1111111111111111" if x==None for x in im]
					self.image_dict[name]["im"] = im
					picture = self.image_dict[name]
					picture = self.end_image(picture)
					return picture
				else:
					continue



		total_bytes = byte_string
		op_code, name, byte_string = byte_string[0:2], byte_string[2:10], byte_string[10:]
		print(op_code, name)
		#START#
		if op_code == "01":
			total_height, total_width, byte_string = byte_string[:16], byte_string[16:32], byte_string[32:]
			total_height = int(total_height, 2)
			total_width = int(total_width, 2)

			total_bits = total_height * total_width

			np_size = int(total_bits / self.bits_received)

			self.image_dict[name] = {}
			self.image_dict[name]["im"] = [None] * np_size
			self.image_dict[name]["height"] = total_height
			self.image_dict[name]["width"] = total_width
			print(self.image_dict[name])

			# If image packets were received before the start, add them to the image now
			if name in self.out_of_order:
				for string in self.out_of_order[name]:
					self.read_byte_string(string)
				del self.out_of_order[name]

		#IMAGE#
		elif op_code == "10":
			order, byte_string = byte_string[:16], byte_string[16:24]
			order = int(order, 2)
			if name in self.image_dict:
				self.image_dict[name]["im"][order] = byte_string

			# If there is not started image, add the bytestring to out_of_order
			elif name in self.out_of_order:
				self.out_of_order[name] = [total_bytes] + self.out_of_order[name]
			else:
				self.out_of_order[name] = [total_bytes]

		#STOP#
		elif op_code == "11":
			if name in self.image_dict:
				picture = self.image_dict[name]
				if None in picture:
					self.timer[name] = time.time()
				else:
					pic = self.end_image(picture)
					return pic
			else:
				raise ValueError("No image to complete")
		else:
			raise ValueError("Invalid image opcode")

if __name__ == "__main__":

	improc = ImageProccessing(8)

	small_pic = "01" + "00000000" + "0000000000000100" + "0000000000000100"
	# Bits are (start) + (image_name) + (image_height = 4) + (image_width = 4)
	improc.read_byte_string(small_pic)

	first = "10" + "00000000" + "0000000000000000" + "1111111111111111"
	# continue image (00000000) with bits at index 0
	improc.read_byte_string(first)

	out_of_order = "11" + "00000001" + "0000000000000001" + "1111111111111111"
	# end image 00000001 with a bunch of random bits at the end
	improc.read_byte_string(out_of_order)

	second = "10" + "00000000" + "0000000000000001" + "1111111111111111"
	# continue image (00000000) with bits at index 1
	improc.read_byte_string(second)

	out_of_order = "10" + "00000001" + "0000000000000001" + "1111111111111111"
	# continue image (00000001) with bits at index 1
	improc.read_byte_string(out_of_order)

	end = "11" + "00000000"
	# end image (00000000)
	test_im = improc.read_byte_string(end)
	
	assert(np.array_equal(test_im, np.ones((4, 4))))
	assert(len(improc.out_of_order) == 1)
	assert(len(improc.out_of_order["00000001"]) == 2)
	assert("00000000" not in improc.image_dict)
	print("class ImageProccessing tests passed 4/4")


	
