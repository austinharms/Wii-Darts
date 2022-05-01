#include <fstream>
#include <iostream>
#include <filesystem>
#include <stdint.h>

#include "stb_image.h"
namespace fs = std::filesystem;

void setPixel(const int x, const int y, const uint32_t color, uint32_t width, uint8_t* img, uint32_t s) {
	uint32_t  offs;
	offs = (((y & (~3)) << 2) * width) + ((x & (~3)) << 4) + ((((y & 3) << 2) + (x & 3)) << 1);

	if (offs + 16 > s || offs + 48 > s) {
		//std::cout << "OB: " << x << ", " << y << " loc: " << offs << std::endl;
	}
	else
	{
		*((uint16_t*)(img + offs)) = (uint16_t)((color << 8) | (color >> 24));
		*((uint16_t*)(img + offs + 32)) = (uint16_t)(color >> 8);
	}
}

int main(int argc, char** argv, char** envp)
{
	std::string path("./textures/");
	std::string ext(".png");
	fs::current_path(fs::path("C:\\Users\\austi\\source\\repos\\Wii\\PureGX"));
	for (auto& p : fs::recursive_directory_iterator(path))
	{
		if (p.path().extension() == ext) {
			int w;
			int h;
			int comp;
			uint32_t* image = (uint32_t*)stbi_load((std::filesystem::current_path().string() + "\\textures\\" + p.path().stem().string() + ".png").c_str(), &w, &h, &comp, 4);
			if (image != nullptr) {
				//std::cout << "Size: " << (((uint64_t)w * h * 4) + 4) << std::endl;
				uint8_t* rawImg = (uint8_t*)malloc(((uint64_t)w * h * 4) + 4);
				uint32_t uW = w;
				uint32_t uH = h;
				rawImg[0] = ((uint8_t*)&uW)[1];
				rawImg[1] = ((uint8_t*)&uW)[0];
				rawImg[2] = ((uint8_t*)&uH)[1];
				rawImg[3] = ((uint8_t*)&uH)[0];
				//((uint32_t*)rawImg)[0] = (uint32_t)((w << 16) + (uint16_t)h);
				uint32_t index = 0;
				for (int y = 0; y < h; ++y) {
					for (int x = 0; x < w; ++x) {
						setPixel(x, y, image[index++], uH, rawImg + 4, ((uint64_t)w * h * 4));
					}
				}

				std::ofstream wf((std::filesystem::current_path().string() + "\\data\\" + p.path().stem().string() + ".img").c_str(), std::ios::out | std::ios::binary);
				wf.write((const char*)rawImg, ((uint64_t)w * h * 4) + 4);
				wf.close();
				free(rawImg);
				stbi_image_free(image);
			}
		}
	}

	return 0;
}