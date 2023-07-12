#ifndef IMAGE_H
#define IMAGE_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "util.h"

class Image
{
private:
	int height;
	int width;
	std::vector<int> pixelColor;

public:
	Image(int width, int height)
		:width(width), height(height)
	{
		size_t size = width * height * 3; // 3 is for rgb

		// initialize with white
		for (int i = 0; i < size; i++)
			pixelColor.push_back(0);
	}

	virtual ~Image()
	{
	}

	int getWidth() const
	{
		return width;
	}

	int getHeight() const
	{
		return height;
	}

	/*unsigned int* getPixel(int x, int y)
	{
		return pixelColor + x + y * width;
	}*/



	void setPixel(int r, int g, int b, int x, int y)
	{
		pixelColor.insert(pixelColor.begin() + (x + y * width) * 3, r);
		pixelColor.insert(pixelColor.begin() + (x + y * width) * 3 + 1, g);
		pixelColor.insert(pixelColor.begin() + (x + y * width) * 3 + 2, b);
	}

	int saveImage(std::string filename) const
	{
		std::ofstream file;
		file.open(filename);

		if (file.fail())
		{
			std::cout << "ERROR::Could not open file" << filename << std::endl;
			/*getchar();
			getchar();*/
			return 0;
		}

		// WRITE HEADER
		file << "P3" << std::endl;						// set ASCII color values
		file << width << " " << height << std::endl;	
		file << "255" << std::endl;						// max color value

		int pixel = 0;

		// sets background
		//for (int row = 0; row < width; row++) {
		//	for (int col = 0; col < height; col++) {

		//		image_data[pixel * 3] = row % width; //red
		//		image_data[pixel * 3 + 1] = col % width; //green
		//		image_data[pixel * 3 + 2] = col % width; //blue
		//		pixel++;
		//	}
		//}
		

		{ //Image Body - outputs image_data array to the .ppm file, creating the image
			for (int i = 0; i < width * height * 3; i++) {
				//int value = pixelColor[x];		//Sets value as an integer, not a character value
				file << pixelColor[i] << " " << std::endl;		//Sets 3 bytes of colour to each pixel	
			}
		}

		file.close();


		//free(image_data);
		//image_data = NULL;
		/*getchar();
		getchar();*/

		return 0;
	}

};

#endif // !IMAGE_H

