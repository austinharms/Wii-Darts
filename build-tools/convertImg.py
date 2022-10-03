import sys
import os
import struct
import math
from PIL import Image

os.chdir(os.path.dirname(os.getcwd()))
image_path = ""
for path in sys.argv[2:-1]:
	image_path = os.path.join(os.getcwd(), path, sys.argv[1])
	if os.path.isfile(image_path):
		break

img = Image.open(image_path, "r").rotate(180).convert("RGBA")
width, height = img.size
if not (math.log(width, 2).is_integer() and math.log(height, 2).is_integer()):
	print(f'Invalid image "{image_path}", Image width and height must be a power of 2')
	quit()
	
img_file = open(os.path.join(os.getcwd(), sys.argv[-1], sys.argv[1][:-3] + "RGBA8"), "wb")
img_file.write(struct.pack( ">H", width))
img_file.write(struct.pack( ">H", height))
# pad file 28 bytes to align pixel bytes to 32 byte boundary
img_file.write(struct.pack( ">Q", 0)) # 8
img_file.write(struct.pack( ">Q", 0)) # 8
img_file.write(struct.pack( ">Q", 0)) # 8
img_file.write(struct.pack( ">L", 0)) # 4
for y in range(0, width, 4):
	for x in range(0, height, 4):
		pixel_values = []
		for y2 in range(4):
			for x2 in range(4):
				pixel_values.append(img.getpixel((x + x2, y + y2)))
		for (r, g, b, a) in pixel_values:
			img_file.write(struct.pack( ">B", a))
			img_file.write(struct.pack( ">B", r))
		for (r, g, b, a) in pixel_values:
			img_file.write(struct.pack( ">B", g))
			img_file.write(struct.pack( ">B", b))
img_file.close()
print(sys.argv[1][:-3] + "RGBA8")