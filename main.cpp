#include <iostream>

#include "util.h"
#include "Image.h"
#include "Camera.h"
#include "Shape.h"

void rayTracer(Image& image, Camera* camera, Shape* scene)
{
	for (int x = 0; x < image.getWidth(); x++)
	{
		for (int y = 0; y < image.getHeight(); y++)
		{
			glm::vec2 screenCoord((2.0f * x) / image.getWidth() - 1.0f, (-2.0f * y) / image.getHeight() + 1.0f);

			Ray ray = camera->makeRay(screenCoord);

			Intersection intersection(ray);

			if (scene->intersect(intersection))
				image.setPixel(intersection.color.r, intersection.color.g, intersection.color.b, x, y);
			else
				image.setPixel(0,0,0, x, y);
		}
		std::cout << x << "\n";
	}
}


int main(void)
{
	int width = 1920;
	int height = 1080;

	Image image(width, height);


	//int p;
	//p = 150;
	//for (int j = 0; j < 100; j++)
	//{
	//	for (int i=0; i<300; i++)
	//	{ 
	//		image.setPixel(255, 113, 232, i, j);
	//	}
	//}
	//
	//p = 0;
	//for (int j = 100; j < 200; j++)
	//{
	//	for (int i = 0; i < 300; i++)
	//	{
	//		image.setPixel(253, 255, 43, i, j);
	//	}
	//}
	//
	//p = 255;
	//for (int j = 200; j < 300; j++)
	//{
	//	for (int i = 0; i < 300; i++)
	//	{
	//		image.setPixel(47, 249, 255, i, j);
	//	}
	//}

	Camera camera(glm::vec3(-5.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(), 25.0f * PI / 180.0f, (float)width / (float)height);

	ShapeSet scene;

	Plane floor(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(), Color(0.5f, 1.0f, 0.5f));
	scene.addShape(&floor);

	Sphere sphere(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, Color(0.5f, 0.5f, 1.0f));
	scene.addShape(&sphere);

	rayTracer(image, &camera, &scene);

	image.saveImage("image.ppm");

	return 0;
}