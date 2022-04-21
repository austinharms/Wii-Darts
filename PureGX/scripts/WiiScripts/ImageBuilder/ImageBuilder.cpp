#include <fstream>
#include <iostream>
#include <filesystem>
#include <stdint.h>

#include "stb_image.h"
namespace fs = std::filesystem;

void setPixel(const int x, const int y, const uint32_t color, uint32_t width, uint8_t* img) {
	uint32_t  offs;
	offs = (((y & (~3)) << 2) * width) + ((x & (~3)) << 4) + ((((y & 3) << 2) + (x & 3)) << 1);

	*((uint16_t*)(img + offs)) = (uint16_t)((color << 8) | (color >> 24));
	*((uint16_t*)(img + offs + 32)) = (uint16_t)(color >> 8);
}

int main(int argc, char** argv, char** envp)
{
	std::string path("./textures/");
	std::string ext(".png");
	std::ofstream outfile("test.txt");
	for (auto& p : fs::recursive_directory_iterator(path))
	{
		outfile << p.path().string() << std::endl;
		if (p.path().extension() == ext) {
			int w;
			int h;
			int comp;
			uint32_t* image = (uint32_t*)stbi_load(p.path().string().c_str(), &w, &h, &comp, 4);
			if (image != nullptr) {
				uint8_t* rawImg = (uint8_t*)malloc((w * h * 4) + 4);
				((uint32_t*)rawImg)[0] = (uint32_t)((w << 16) + (uint16_t)h);
				uint32_t index = 0;
				for (int y = 0; y < h; ++h) {
					for (int x = 0; x < w; ++x) {
						setPixel(x, y, image[index++], w, rawImg + 4);
					}
				}

				std::ofstream wf(("./" + p.path().stem().string() + ".img").c_str(), std::ios::out | std::ios::binary);
				outfile << "Save: " << ("./" + p.path().filename().string() + ".img") << std::endl;
				wf.write((const char*)rawImg, (w * h * 4) + 4);
				wf.close();
				stbi_image_free(image);
			}
		}
	}

	//std::ofstream outfile("test.txt");
	//for (char** env = envp; *env != 0; env++)
	//{
	//    outfile << *env << std::endl;
	//}

	outfile.close();
	return 0;
}