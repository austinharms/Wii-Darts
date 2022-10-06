import sys
import os
import struct
from collections import OrderedDict

os.chdir(os.path.dirname(os.getcwd()))
obj_path = ""
for path in sys.argv[2:-1]:
	obj_path = os.path.join(os.getcwd(), path, sys.argv[1])
	if os.path.isfile(obj_path):
		break

obj_file = open(obj_path, "r")
obj_file_lines = obj_file.readlines()
obj_file.close()
file_verticies = []
file_uvs = []
file_faces = []

for line in obj_file_lines:
	if line.startswith("v "):
		file_verticies.append(line.split(" ")[1:4])
	elif line.startswith("vt "):
		file_uvs.append(line.split(" ")[1:3])
	elif line.startswith("f "):
		# we only support triangle faces
		faces = line.split(" ")[1:4]
		# fix vertex order
		file_faces.append(faces[0].split("/"))
		file_faces.append(faces[2].split("/"))
		file_faces.append(faces[1].split("/"))

mesh_verticies = OrderedDict()
mesh_indicies = []
for face in file_faces:
	vertex_name = face[0] + "/" + face[1]
	if not vertex_name in mesh_verticies:
		mesh_verticies[vertex_name] = [len(mesh_verticies)] + file_verticies[int(face[0]) - 1] + file_uvs[int(face[1]) - 1]
	mesh_indicies.append(mesh_verticies[vertex_name][0])

mesh_file = open(os.path.join(os.getcwd(), sys.argv[-1], sys.argv[1][:-3] + "mesh"), "wb")
mesh_file.write(struct.pack( ">H", len(mesh_verticies)))
mesh_file.write(struct.pack( ">H", len(mesh_indicies)))

for key, vertex in mesh_verticies.items():
	mesh_file.write(struct.pack( ">f", float(vertex[1])))
	mesh_file.write(struct.pack( ">f", float(vertex[2])))
	mesh_file.write(struct.pack( ">f", float(vertex[3])))
	# fix texture u flipped
	mesh_file.write(struct.pack( ">f", 1 - float(vertex[4])))
	mesh_file.write(struct.pack( ">f", float(vertex[5])))

for index in mesh_indicies:
	mesh_file.write(struct.pack( ">H", index))

mesh_file.close()
print(sys.argv[1][:-3] + "mesh")