#include "Debug.h"
#include "Theme.h"
#include "Color.h"
#include <cstdio>
#include <cstring>
#include "SDL.h"
#include "SDL_rwops.h"

Theme::Theme()
{
	mFontWidth = 8;
	mFontHeight = 8;
	mWidth = 480;
	mHeight = 360;
	mBasePath = "assets/";
	mBackgroundPath = mBasePath+"gui.png";
	mFontPath = mBasePath+"font.png";
}


bool Theme::load(const std::string& path)
{
	mPath = path;

	if (!loadDefinition(path))
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Could not load theme", ("Could not load "+path+". Perhaps you need to set the working directory?").c_str(), NULL);
		return false;
	}

	return true;
}


const std::string& Theme::getName() const
{
	return mName;
}


const std::string& Theme::getPath() const
{
	return mPath;
}

const std::string& Theme::getFontPath() const
{
	return mFontPath;
}

const std::string& Theme::getBackgroundPath() const
{
	return mBackgroundPath;
}


int Theme::getFontWidth() const
{
	return mFontWidth;
}


int Theme::getFontHeight() const
{
	return mFontHeight;
}

int Theme::getWidth() const
{
	return mWidth;
}


int Theme::getHeight() const
{
	return mHeight;
}


const Theme::Element& Theme::getElement(int index) const
{
	return mElement[index];
}


int Theme::getElementCount() const
{
	return mElement.size();
}


static char *rwgets(char *buf, int count, SDL_RWops *rw)
{
    int i;

    buf[count - 1] = '\0';

    for (i = 0; i < count - 1; i++)
    {
        if (SDL_RWread(rw, buf + i, 1, 1) != 1)
        {
            if (i == 0)
            {
                return NULL;
            }

            break;
        }

        if (buf[i] == '\n')
        {
            break;
        }
    }

    buf[i] = '\0';

    return buf;
}


bool Theme::loadDefinition(const std::string& path)
{
	SDL_RWops *rw = SDL_RWFromFile(path.c_str(), "r");

	if (!rw)
		return false;

	int lineCounter = 0;

	static const char *colorNames[] = {
		"CurrentRow",
		"BlockMarker",
		"EditCursor",
		"NonEditCursor",
		"RowCounter",
		"SelectedRow",
		"ModalBackground",
		"ModalBorder",
		"ModalTitleBackground",
		"ModalTitleText",
		"NormalText",
		"ScrollBar",
		"PlayHead",
		"TextCursor",
		"TextBackground",
		"TextFocus",
		"Oscilloscope",
		NULL,
	};

	static const char *elementNames[] = {
		"PatternEditor",
		"SequenceEditor",
		"MacroEditor",
		"Oscilloscope",
		"SongName",
		"MacroName",
		"MacroNumber",
		"SequencePosition",
		"SequenceLength",
		"PatternLength",
		"OctaveNumber",
		"TouchRegion",
		NULL
	};

	while (true)
	{
		++lineCounter;

		char line[500];
		if (rwgets(line, sizeof(line), rw) == NULL)
			break;

		// Check if comment

		if (line[0] == '#')
			continue;

		char elementName[20], path[100], strParameters[10][50] = {0};
		int parameters[10] = {0};

		if (sscanf(line, "%19s \"%99[^\"]\" %d %d", elementName, path, &parameters[0], &parameters[1]) >= 4 && strcmp("Font", elementName) == 0)
		{
			mFontPath = mBasePath + path;
			mFontWidth = parameters[0];
			mFontHeight = parameters[1];
		}
		else if (sscanf(line, "%19s \"%99[^\"]\" %d %d %d", elementName, path, &parameters[0], &parameters[1], &parameters[2]) >= 4 &&
			(strcmp("GUI", elementName) == 0 || strcmp("Color", elementName) == 0))
		{
			if (strcmp(elementName, "Color") == 0)
			{
				ColorType colorType = NumColors;
				for (int index = 0 ; colorNames[index] ; ++index)
				{
					if (strcmp(colorNames[index], path) == 0)
					{
						colorType = static_cast<Theme::ColorType>(index);
						break;
					}
				}

				if (colorType < NumColors)
				{
					mColors[colorType] = Color(parameters[0], parameters[1], parameters[2]);
				}
				else
				{
					debug("Unknown color %s", elementName);
				}
			}
			else
			{
				mBackgroundPath = mBasePath + path;
				mWidth = parameters[0];
				mHeight = parameters[1];
			}
		}
		else if ((sscanf(line, "%19s %d %d %d %d \"%50[^\"]\" \"%50[^\"]\"", elementName, &parameters[0], &parameters[1], &parameters[2], &parameters[3], strParameters[0], strParameters[1]) >= 5
			&& strcmp("TouchRegion", elementName) == 0)
			||sscanf(line, "%19s %d %d %d %d %d %d", elementName, &parameters[0], &parameters[1], &parameters[2], &parameters[3], &parameters[4], &parameters[5]) >= 5)
		{
			Element element;
			element.type = Theme::Unknown;

			for (int index = 0 ; elementNames[index] ; ++index)
			{
				if (strcmp(elementNames[index], elementName) == 0)
				{
					element.type = static_cast<Theme::ElementType>(index);
					break;
				}
			}

			if (element.type != Theme::Unknown)
			{
				memcpy(element.parameters, parameters, sizeof(element.parameters));
				memcpy(element.strParameters, strParameters, sizeof(element.strParameters));
				mElement.push_back(element);
			}
			else
			{
				debug("Unknown element %s on line %d", elementName, lineCounter);
			}
		}
		else
		{
			debug("Weirdness on line %d", lineCounter);
		}
	}

	SDL_RWclose(rw);

	return true;
}


const Color& Theme::getColor(ColorType type) const
{
	return mColors[type];
}
