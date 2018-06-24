#pragma once

struct EffectParam;

struct Color
{
	int r, g, b, a;
	Color(int r = 255, int g = 255, int b = 255, int a = 255);

	static Color getEffectColor(const EffectParam& effect);
};

inline Color operator*(const Color& lhs, const Color& rhs){ return Color(lhs.r * rhs.r / 255, lhs.g * rhs.g / 255, lhs.b * rhs.b / 255, lhs.a * rhs.a / 255); }
