#ifndef CAMERA_H
#define CAMERA_H

#include "util.h"
#include "ray.h"

class Camera
{
private:
	glm::vec3 origin;
	glm::vec3 forward;
	glm::vec3 up;
	glm::vec3 right;

	float height, width;

public:
	Camera(glm::vec3 origin, glm::vec3 target, glm::vec3 upVec, float fov, float aspectRatio)
		: origin(origin)
	{
		forward = glm::normalize(target - origin); // from camera origin to image center
		right = glm::normalize(glm::cross(forward, upVec));
		up = glm::cross(right, forward); // already normalizes

		height = glm::tan(fov);
		width = height * aspectRatio;
	}

	virtual ~Camera() {}

	// we can use this to loop through all the image pixels and check if this ray intersects any obj in the scene!
	Ray makeRay(glm::vec2 point) const
	{
		glm::vec3 direction = forward + (point.x * width * right) + (point.y * height * up);

		return Ray(origin, glm::normalize(direction));
	}
};

#endif // CAMERA_H