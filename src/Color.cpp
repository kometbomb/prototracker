#include "Color.h"
#include "EffectParam.h"

Color::Color(int _r, int _g, int _b, int _a)
	: r(_r), g(_g), b(_b), a(_a)
{
}


Color Color::getEffectColor(const EffectParam& effect)
{
	if (effect.isEmpty())
		return Color(128, 128, 128);

	switch (effect.effect)
	{
		default:
			return Color(192, 192, 192);

		case 'n':
			return Color(128, 255, 128);

		case 'b':
		case 'd':
		case 'j':
			return Color(255, 128, 128);

		case 'a':
		case 'c':
		case 'k':
			return Color(192, 192, 255);

		case 'm':
			return Color(255, 255, 255);

		case 'w':
		case 'q':
			return Color(255, 192, 128);

		case 'f':
		case 's':
			return Color(192, 255, 128);
	}
}
