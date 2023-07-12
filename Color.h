#ifndef COLOR_H
#define COLOR_h

struct Color
{
	int r;
	int g;
	int b;

	Color()
		:r(0), g(0), b(0)
	{}

	Color(int r, int g, int b)
		:r(r), g(g), b(b)
	{}
};

#endif // !COLOR_H

