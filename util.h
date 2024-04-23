#pragma once

#include <algorithm>

struct Rect
{
	float x, y, w, h;

	[[nodiscard]] bool intersects(const Rect& rect) const
	{
		if (y + h <= rect.y)
		{
			return false;
		}
		if (y >= rect.y + rect.h)
		{
			return false;
		}
		if (x + w <= rect.x)
		{
			return false;
		}
		if (x >= rect.x + rect.w)
		{
			return false;
		}
		return true;
	}

	[[nodiscard]] Rect intersectRect(const Rect& r2) const
	{
		auto intersectX = std::max(x, r2.x);
		auto intersectY = std::max(y, r2.y);
		auto intersectW = std::min(x + w, r2.x + r2.w) - intersectX;
		auto intersectH = std::min(y + h, r2.y + r2.h) - intersectY;

		if (intersectW > 0 && intersectH > 0)
		{
			return Rect{ intersectX, intersectY, intersectW, intersectH };
		}
		return Rect{};
	}
};

struct Point2D
{
	union {
		float x;
		float w;
	};

	union {
		float y;
		float h;
	};
};

using Size2D = Point2D;