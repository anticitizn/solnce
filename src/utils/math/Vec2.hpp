
#pragma once

#include <src/utils/macros.hpp>

class Vec2
{
public:

	Vec2()
		: x(0.0f), y(0.0f)
	{}

	Vec2(float x, float y)
		: x(x), y(y)
	{}

	Vec2 operator+(Vec2 const& v)
	{
		return Vec2(
			x + v.x,
			y + v.y);
	}

	Vec2 operator+=(Vec2 const& v)
	{
		x += v.x;
		y += v.y;

		return *this;
	}

	Vec2 operator-(Vec2 const& v)
	{
		return Vec2(
			x - v.x,
			y - v.y);
	}

	Vec2 operator-=(Vec2 const& v)
	{
		x -= v.x;
		y -= v.y;

		return *this;
	}

	void archive(pugi::xml_node& root)
    {
        pugi::xml_node arch = root.append_child("Vec2");
        
        ARCHIVE_VAR(arch, x);
		ARCHIVE_VAR(arch, y);
    }

	float x, y;
};
