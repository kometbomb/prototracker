#pragma once

#include <string>
#include <vector>
#include "Color.h"

class Theme
{
public:
	enum ElementType
	{
		PatternEditor,
		SequenceEditor,
		MacroEditor,
		Oscilloscope,
		SongName,
		MacroName,
		MacroNumber,
		SequencePosition,
		SequenceLength,
		PatternLength,
		OctaveNumber,
		TouchRegion,
		Unknown
	};

	struct Element
	{
		ElementType type;
		int parameters[10];
		char strParameters[10][50];
	};

	enum ColorType
	{
		CurrentRow,
		BlockMarker,
		EditCursor,
		NonEditCursor,
		RowCounter,
		SelectedRow,
		ModalBackground,
		ModalBorder,
		ModalTitleBackground,
		ModalTitleText,
		NormalText,
		ScrollBar,
		PlayHead,
		TextCursor,
		TextBackground,
		TextFocus,
		OscilloscopeColor,
		NumColors
	};

	static const int numColors = ColorType::NumColors;

private:
	std::string mName;
	std::string mPath, mBackgroundPath, mFontPath, mBasePath;
	int mWidth, mHeight, mFontWidth, mFontHeight;
	std::vector<Element> mElement;
	Color mColors[numColors];

	bool loadDefinition(const std::string& path);

public:

	Theme();

	bool load(const std::string& path);

	const std::string& getName() const;
	const std::string& getPath() const;
	const std::string& getFontPath() const;
	const std::string& getBackgroundPath() const;
	int getFontWidth() const;
	int getFontHeight() const;
	int getWidth() const;
	int getHeight() const;
	const Element& getElement(int index) const;
	int getElementCount() const;
	const Color& getColor(ColorType type) const;
};
