#ifndef RAY_H
#define RAY_H

#include "util.h"
#include "Color.h"

// distance between ray and its origin shape - prevent self-intersecting
#define RAY_T_MIN 0.0001f 

// 'infinite' distance
#define RAY_T_MAX 1.0e30f 

#ifndef PI
#define PI 3.1415926f
#endif

struct Ray
{
	glm::vec3 origin; // where the ray starts
	glm::vec3 direction;
	float tMax;

	Ray()
		: origin(glm::vec3(0.f)), direction(glm::vec3(0.f, 1.f, 0.f)), tMax(RAY_T_MAX)
	{}

	Ray(const Ray& r)
		:origin(r.origin), direction(r.direction), tMax(r.tMax)
	{}

	Ray(const glm::vec3& origin, const glm::vec3& direction, float tMax = RAY_T_MAX)
		: origin(origin), direction(direction), tMax(tMax)
	{}
	
	virtual ~Ray(){}

	Ray& operator =(const Ray& r)
	{
		origin = r.origin;
		direction = r.direction;
		tMax = r.tMax;
		return *this;
	}

	// given a length t, returns the point along the ray
	glm::vec3 calculateRayPoint(float t) const
	{
		return origin + direction * t;
	}
};

class Shape;

struct Intersection
{
	Ray ray; // the ray that is intersecting
	float t; // distance from origin to point where intersection happens
	Shape* pShape; 
	Color color;

	Intersection()
		:ray(ray), t(RAY_T_MAX), pShape(NULL), color(Color())
	{}

	Intersection(const Intersection& i)
		:ray(i.ray), t(i.t), pShape(i.pShape), color(i.color)
	{}
	
	Intersection(const Ray& ray)
		:ray(ray), t(ray.tMax), pShape(NULL), color(Color())
	{}

	virtual ~Intersection(){}

	Intersection& operator =(const Intersection& i)
	{
		ray = i.ray;
		t = i.t;
		pShape = i.pShape;
		return *this;
	}

	// checks if ray intersects with something
	bool intersected() const
	{
		return (pShape != NULL); // if shape is null, then there is no intersection
	}

	// position/point where intersection happened
	glm::vec3 position() const
	{
		return ray.calculateRayPoint(t);
	}
};


#endif
