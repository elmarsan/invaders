#pragma once

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

	[[nodiscard]] bool isZero() const
	{
		return x == 0 && y == 0 && w == 0 && h == 0;
	}

	void zero()
	{
		x = 0;
		y = 0;
		w = 0;
		h = 0;
	}
};