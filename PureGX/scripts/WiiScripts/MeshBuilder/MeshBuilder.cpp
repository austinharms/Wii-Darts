#include <fstream>
#include <iostream>
#include <filesystem>
#include <stdint.h>

namespace fs = std::filesystem;

struct Vec3 {
	float x;
	float y;
	float z;
};

struct Vec2 {
	float x;
	float y;
};

void write32LE(std::ofstream& wf, uint8_t* f) {
	wf.write(((const char*)f) + 3, 1);
	wf.write(((const char*)f) + 2, 1);
	wf.write(((const char*)f) + 1, 1);
	wf.write(((const char*)f), 1);
}

#pragma warning(disable : 4996)

int main(int argc, char** argv, char** envp)
{
	std::string path("./meshes/");
	std::string ext(".obj");

	for (auto& p : fs::recursive_directory_iterator(path))
	{
		if (p.path().extension() == ext) {
			//(std::filesystem::current_path().string() + "\\textures\\" + p.path().stem().string() + ".png").c_str()
			// 
			//Taken from: http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
			//std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
			std::vector< Vec3 > temp_vertices;
			std::vector< Vec2 > temp_uvs;
			std::vector<float> temp_floats;
			//std::vector< Vec3 > temp_normals;

			FILE* file = fopen((std::filesystem::current_path().string() + "\\meshes\\" + p.path().stem().string() + ".obj").c_str(), "r");
			if (file == NULL) 
				return 1;

			while (1) {
				char lineHeader[512];
				// read the first word of the line
				int res = fscanf(file, "%s", lineHeader);
				if (res == EOF)
					break; // EOF = End Of File. Quit the loop.

				if (strcmp(lineHeader, "v") == 0) {
					Vec3 vertex;
					fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
					temp_vertices.push_back(vertex);
				}
				else if (strcmp(lineHeader, "vt") == 0) {
					Vec2 uv;
					fscanf(file, "%f %f\n", &uv.x, &uv.y);
					temp_uvs.push_back(uv);
				}
				//else if (strcmp(lineHeader, "vn") == 0) {
				//	Vec3 normal;
				//	fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				//	temp_normals.push_back(normal);
				//}
				else if (strcmp(lineHeader, "f") == 0) {
					//std::string vertex1, vertex2, vertex3;
					unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
					int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
					if (matches != 9)
						return 1;

					for (uint8_t i = 0; i < 3; ++i) {
						temp_floats.push_back(temp_vertices.at(vertexIndex[i] - 1).x);
						temp_floats.push_back(temp_vertices.at(vertexIndex[i] - 1).y);
						temp_floats.push_back(temp_vertices.at(vertexIndex[i] - 1).z);
						temp_floats.push_back(temp_uvs.at(uvIndex[i] - 1).x);
						temp_floats.push_back(temp_uvs.at(uvIndex[i] - 1).y);
					}

					//vertexIndices.push_back(vertexIndex[0]);
					//vertexIndices.push_back(vertexIndex[1]);
					//vertexIndices.push_back(vertexIndex[2]);
					//uvIndices.push_back(uvIndex[0]);
					//uvIndices.push_back(uvIndex[1]);
					//uvIndices.push_back(uvIndex[2]);
					//normalIndices.push_back(normalIndex[0]);
					//normalIndices.push_back(normalIndex[1]);
					//normalIndices.push_back(normalIndex[2]);
				}
			}

			fclose(file);
			std::ofstream wf((std::filesystem::current_path().string() + "\\data\\" + p.path().stem().string() + ".mesh").c_str(), std::ios::out | std::ios::binary);
			uint32_t vertextCount = temp_floats.size()/5;
			write32LE(wf, (uint8_t*)&vertextCount);
			vertextCount = temp_floats.size();
			for (uint32_t i = 0; i < vertextCount; ++i) {
				float f = temp_floats.at(i);
				write32LE(wf, (uint8_t*)&f);
			}

			wf.close();
		}
	}

	return 0;
}