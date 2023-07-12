#ifndef SHAPE_H
#define SHAPE_H

#include <vector>
#include "Ray.h"

float length_2(glm::vec3 v)
{
	return glm::pow(v.x, 2.f) + glm::pow(v.y, 2.f) + glm::pow(v.z, 2.f);
}

class Shape
{
public:
	virtual ~Shape() {}

	virtual bool intersect(Intersection& intersection) = 0;
	virtual bool doesIntersect(const Ray& ray) = 0;
};

// used to DESCRIBE a SCENE
class ShapeSet : public Shape
{
protected:
	std::vector<Shape*> shapes;

public:
	ShapeSet(){}
	virtual ~ShapeSet(){}

	void addShape(Shape* shape)
	{
		shapes.push_back(shape);
	}

	// looks for intersection in all vector shapes and pass the data to the intersection
	virtual bool intersect(Intersection& intersection)
	{
		bool intersect = false;

		for (auto& s : shapes)
			intersect = s->intersect(intersection);

		return intersect;
	}

	// looks for intersection in all vector shapes
	virtual bool doesIntersect(const Ray& ray)
	{
		for (auto& s : shapes)
		{
			if (s->doesIntersect(ray))
				return true;
		}
		return false;
	}
};

class Plane : public Shape
{
protected:
	glm::vec3 position;
	glm::vec3 normal;
	Color color;

public:
	Plane(const glm::vec3& position, const glm::vec3& normal, const Color& color = Color(0, 0, 0))
		:position(position), normal(normal), color(color)
	{}

	virtual ~Plane(){}

	// check intersection and pass relative data to intersection object 
	virtual bool intersect(Intersection& intersection)
	{
		float dotProduct = glm::dot(intersection.ray.direction, normal);

		// no intersection
		if (dotProduct == 0.0f)
			return false;

		// find distance between ray's starting point and point where intersection happens
		float t = glm::dot(position - intersection.ray.origin, normal) / dotProduct;

		// outside of range - keep the closest intersection, in case there were more than one
		if (t <= RAY_T_MIN || t >= intersection.t)
			return false;

		intersection.t = t;
		intersection.pShape = this;

		return true;
	}

	// check intersection
	virtual bool doesIntersect(const Ray& ray)
	{
		float dotProduct = glm::dot(ray.direction, normal);

		// no intersection
		if (dotProduct == 0.0f)
			return false;

		// find distance between ray's starting point and point where intersection happens
		float t = glm::dot(position - ray.origin, normal) / dotProduct;

		// outside of range - keep the closest intersection, in case there were more than one
		if (t <= RAY_T_MIN || t >= ray.tMax)
			return false;

		return true;
	}
};

class Sphere : public Shape
{
protected:
	glm::vec3 center;
	float radius;
	Color color;

public:
	Sphere(const glm::vec3& center, float radius, const Color& color = Color(0, 0, 0))
		:center(center), radius(radius), color(color)
	{}

	virtual ~Sphere(){}

	virtual bool intersect(Intersection& intersection)
	{
		// transformation that translate sphere's center to origin of system
		Ray localRay = intersection.ray;
		localRay.origin -= center;

		// calculate coefficients
		float a = length_2(localRay.direction);
		float b = 2 * glm::dot(localRay.direction, localRay.origin);	
		float c = length_2(localRay.origin) - radius*radius;

		// check intersection
		float delta = (b*b) - (4*a*c);

		// negative value = no solution
		if (delta < 0.0f)
			return false;

		// find points of intersection - t1 is close and t2 is far - or they are the same if delta==0
		float t1 = (-b + glm::sqrt(delta)) / (2 * a);
		float t2 = (-b - glm::sqrt(delta)) / (2 * a);

		if (t1 > RAY_T_MIN && t1 < intersection.t)
			intersection.t = t1;
		else if (t2 > RAY_T_MIN && t2 < intersection.t)
			intersection.t = t2;
		else
			return false;

		intersection.pShape = this;

		return true;
	}

	virtual bool doesIntersect(const Ray& ray)
	{
		// transformation that translate sphere's center to origin of system
		Ray localRay = ray;
		localRay.origin -= center;

		// calculate coefficients
		float a = length_2(localRay.direction);
		float b = 2 * glm::dot(localRay.direction, localRay.origin);
		float c = length_2(localRay.origin) - radius * radius;

		// check intersection
		float delta = (b * b) - (4 * a * c);

		// negative value = no solution
		if (delta < 0.0f)
			return false;

		// find points of intersection - t1 is close and t2 is far - or they are the same if delta==0
		float t1 = (-b + glm::sqrt(delta)) / (2 * a);
		if (t1 > RAY_T_MIN && t1 < ray.tMax)
			return true;

		float t2 = (-b - glm::sqrt(delta)) / (2 * a);
		if (t2 > RAY_T_MIN && t2 < ray.tMax)
			return true;
		
		return false;
	}
};

#endif // !SHAPE_H
