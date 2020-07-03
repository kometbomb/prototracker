#include "TrackEditor.h"
#include "PatternRow.h"
#include "Renderer.h"
#include "Color.h"
#include "Player.h"
#include "Pattern.h"
#include "EditorState.h"
#include <cstdio>


TrackEditor::TrackEditor(EditorState& editorState, TrackEditorState& trackEditorState, IPlayer& player, Song& song, int tracks)
	: ColumnEditor(editorState, trackEditorState, tracks, PatternRow::NumColumns), mPlayer(player), mSong(song), mTriggerNotes(true), mAddMacroEffect(true)
{

}


TrackEditor::~TrackEditor()
{
}


void TrackEditor::killCurrentTrack()
{
	killTrack(mTrackEditorState.currentTrack);
	// TODO: These shoyld say "macro" in the macro editor!
	showMessage(MessageInfo, "Killed pattern");
}


void TrackEditor::findCurrentUnusedTrack()
{
	findUnusedTrack(mTrackEditorState.currentTrack);
	// TODO: These shoyld say "macro" in the macro editor!
	showMessage(MessageInfo, "Found an unused pattern");
}


void TrackEditor::setTriggerNotes(bool state)
{
	mTriggerNotes = state;
}


void TrackEditor::setAddMacroEffect(bool state)
{
	mAddMacroEffect = state;
}


int TrackEditor::getColumnFlagsFromModifier(int mod) const
{
	int flags = PatternRow::FlagAllColumns;

	if (mod & KMOD_LALT)
		flags = PatternRow::FlagEffect;
	else if (mod & KMOD_LCTRL)
		flags = PatternRow::FlagNote;

	return flags;
}


void TrackEditor::changeColumn(int d)
{
	if (d < 0)
	{
		--mTrackEditorState.currentColumn;
		if (mTrackEditorState.currentColumn == 1 && getCurrentPatternRow().shouldSkipParam1())
			mTrackEditorState.currentColumn = 0;
		else if (mTrackEditorState.currentColumn < 0)
		{
			mTrackEditorState.currentColumn = mColumns - 1;
			changeTrack(-1);
		}
	}
	else
	{
		++mTrackEditorState.currentColumn;
		if (mTrackEditorState.currentColumn == 1 && getCurrentPatternRow().shouldSkipParam1())
			mTrackEditorState.currentColumn = 2;
		if (mTrackEditorState.currentColumn >= mColumns)
		{
			mTrackEditorState.currentColumn = 0;
			changeTrack(1);
		}
	}
}


void TrackEditor::killTrack(int track)
{
	getCurrentPattern(track).clear();
	mTrackEditorState.currentRow.notify();
}


bool TrackEditor::onEvent(SDL_Event& event)
{
	switch (event.type)
	{
		case SDL_KEYDOWN:
			if (!(event.key.keysym.mod & (KMOD_SHIFT|KMOD_CTRL|KMOD_ALT)))
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_PAGEDOWN:
						scrollView(16, false);
						return true;

					case SDLK_PAGEUP:
						scrollView(-16, false);
						return true;

					case SDLK_DOWN:
						scrollView(1);
						return true;

					case SDLK_UP:
						scrollView(-1);
						return true;

					case SDLK_HOME:
						mTrackEditorState.currentRow = 0;
						return true;

					case SDLK_END:
						mTrackEditorState.currentRow = maxRows - 1;
						return true;
				}
			}

			switch (event.key.keysym.sym)
			{
				case SDLK_TAB:
					if (event.key.keysym.mod & KMOD_SHIFT)
						changeTrack(-1);
					else
						changeTrack(1);
					return true;

				case SDLK_LEFT:
					changeColumn(-1);
					return true;

				case SDLK_RIGHT:
					changeColumn(1);
					return true;

				case SDLK_BACKSPACE:
					if (mTrackEditorState.currentRow == 0)
						break;
					scrollView(-1);
					if (event.key.keysym.mod & (KMOD_LSHIFT|KMOD_LALT|KMOD_LCTRL))
					{
						deleteRow(false, getColumnFlagsFromModifier(event.key.keysym.mod));
					}
					else
					{
						playRow();
					}
					return true;

				case SDLK_DELETE:
					emptyRow(false, getColumnFlagsFromModifier(event.key.keysym.mod));
					scrollView(mTrackEditorState.editSkip);
					return true;

				case SDLK_INSERT:
					insertRow(false, getColumnFlagsFromModifier(event.key.keysym.mod));
					return true;

				case SDLK_RETURN:
					if (event.key.keysym.mod & (KMOD_LSHIFT|KMOD_LALT|KMOD_LCTRL))
					{
						if (mTrackEditorState.currentRow == maxRows - 1)
							break;
						insertRow(false, getColumnFlagsFromModifier(event.key.keysym.mod));
					}
					else
					{
						playRow();
					}

					scrollView(1);
					return true;

				default:
				{
					if (event.key.keysym.mod & KMOD_LCTRL)
					{
						int hex = getHexFromKey(event.key.keysym);
						if (hex >= 0 && hex <= 9)
						{
							setEditSkip(hex);
						}
						else
							return false;

						return true;
					}

					if (event.key.keysym.mod & (KMOD_CTRL|KMOD_ALT))
						return false;

					if (!mEditorState.editMode)
					{
						if (event.key.repeat == 0)
						{
							int note = getNoteFromKey(event.key.keysym);

							if (note != -1/* && mTrackEditorState.currentColumn == PatternRow::Column::Note && !(event.key.keysym.mod & KMOD_SHIFT)*/)
							{
								mPlayer.triggerNoteWithReset(mTrackEditorState.currentTrack, note + mEditorState.octave * 12, mEditorState.macro);
								return true;
							}
						}

						break;
					}

					PatternRow& patternRow = getCurrentPatternRow();
					bool handled = false;

					{
						int effectParam;
						PatternRow::Column column;

						PatternRow::translateColumnEnum(mTrackEditorState.currentColumn, effectParam, column);

						switch (static_cast<int>(column))
						{
							case PatternRow::Column::EffectType:
							{
								int c = getCharFromKey(event.key.keysym);
								if (c != -1)
								{
									patternRow.getEffect(effectParam).effect = c;
									handled = true;
								}

							}
								break;

							case PatternRow::Column::EffectParam1:
							{
								int hex = getHexFromKey(event.key.keysym);
								if (hex != -1)
								{
									patternRow.getEffect(effectParam).param1 = hex;
									handled = true;
								}
							}
								break;

							case PatternRow::Column::EffectParam2:
							{
								int hex = getHexFromKey(event.key.keysym);
								if (hex != -1)
								{
									patternRow.getEffect(effectParam).param2 = hex;
									handled = true;
								}
							}
								break;

							case PatternRow::Column::NoteParam1:
							{
								int hex = getHexFromKey(event.key.keysym);
								if (hex != -1)
								{
									if (patternRow.getNote().effect != 'n')
									{
										patternRow.getNote().param1 = hex;
									}

									handled = true;
								}
							}
								break;


							case PatternRow::Column::NoteParam2:
							{
								int hex = getHexFromKey(event.key.keysym);
								if (hex != -1)
								{
									patternRow.getNote().param2 = hex;
									handled = true;
								}
							}
								break;

							case PatternRow::Column::Note:
								if (event.key.keysym.mod & KMOD_SHIFT)
								{
									int c = getCharFromKey(event.key.keysym);
									if (c != -1)
									{
										patternRow.getNote().effect = c;

										if (c == 'n')
										{
											patternRow.getNote().param1 = std::min(11, patternRow.getNote().param1);
											patternRow.getNote().param2 = std::min(15, patternRow.getNote().param2);
										}

										handled = true;
									}
								}
								else
								{
									int note = getNoteFromKey(event.key.keysym);

									if (note != -1)
									{
										patternRow.setNoteAndOctave(mEditorState.octave * 12 + note);

										// Add the Mxx effect in first empty column for convenience

										if (mAddMacroEffect)
										{
											for (int param = 0 ; param < PatternRow::effectParams ; ++param)
											{
												if (patternRow.getEffect(param).isEmpty())
												{
													patternRow.getEffect(0).effect = 'm';
													patternRow.getEffect(0).setParamsFromByte(mEditorState.macro);
													break;
												}
											}
										}

										handled = true;

										//mPlayer.triggerNote(mTrackEditorState.currentTrack, patternRow);
										if (mEditorState.followPlayPosition && mTriggerNotes)
											mPlayer.triggerNoteWithReset(mTrackEditorState.currentTrack, note + mEditorState.octave * 12, mEditorState.macro);
									}

								}

								break;
						}

						if (handled)
						{
							mTrackEditorState.currentRow += mTrackEditorState.editSkip;
							mTrackEditorState.currentRow %= maxRows;

							return true;
						}
					}
				}
				break;
			}

#if SDL_VERSION_ATLEAST(2,0,4) && !defined(__EMSCRIPTEN__)
			case SDL_CONTROLLERBUTTONDOWN:
			{
				bool aPressed = SDL_GameControllerGetButton(SDL_GameControllerFromInstanceID(event.cbutton.which), SDL_CONTROLLER_BUTTON_A);
				PatternRow& patternRow = getCurrentPatternRow();

				switch (event.cbutton.button)
				{
					case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
					case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
						if (!aPressed)
						{
							changeColumn(event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT ? -1 : 1);
						}
						else
						{
							int effectParam;
							PatternRow::Column column;

							PatternRow::translateColumnEnum(mTrackEditorState.currentColumn, effectParam, column);

							switch (column)
							{
								case PatternRow::Column::Note:
								case PatternRow::Column::NoteParam1:
								case PatternRow::Column::NoteParam2:
									// A + LEFT/RIGHT = alter note

									if (patternRow.getNote().effect == 'n')
									{
										patternRow.getNote().setNoteAndOctave(std::max(0, std::min(0xbf, patternRow.getNote().getNoteWithOctave() + (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT ? -1 : 1))));
									}
									break;

								case PatternRow::Column::EffectType: {
									EffectParam effect = patternRow.getEffect(effectParam);

									if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT)
									{
										if (effect.effect == '0')
											effect.effect = 'z';
										else if (effect.effect == 'a')
											effect.effect = '9';
										else
											effect.effect--;
									}
									else
									{
										if (effect.effect == '9')
											effect.effect = 'a';
										else if (effect.effect == 'z')
											effect.effect = '0';
										else
											effect.effect++;
									}
								} break;

								case PatternRow::Column::EffectParam1:
								case PatternRow::Column::EffectParam2:
									patternRow.getEffect(effectParam).setParamsFromByte(patternRow.getEffect(effectParam).getParamsAsByte() + (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT ? -1 : 1) & 255);
									break;

                default:
                  break;
							}
						}

						break;

					case SDL_CONTROLLER_BUTTON_DPAD_UP:
					case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
						if (!aPressed)
						{
							scrollView(event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP ? -1 : 1);
						}
						else
						{
							int effectParam;
							PatternRow::Column column;

							PatternRow::translateColumnEnum(mTrackEditorState.currentColumn, effectParam, column);

							switch (column)
							{
								case PatternRow::Column::Note:
								case PatternRow::Column::NoteParam1:
								case PatternRow::Column::NoteParam2:
									// A + LEFT/RIGHT = alter note

									if (patternRow.getNote().effect == 'n')
									{
										if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN)
											patternRow.getNote().param2 = std::max(0, patternRow.getNote().param2 - 1);
										else
											patternRow.getNote().param2 = std::min(15, patternRow.getNote().param2 + 1);
									}
									break;

								case PatternRow::Column::EffectType:
									break;

								case PatternRow::Column::EffectParam1:
								case PatternRow::Column::EffectParam2:
									patternRow.getEffect(effectParam).setParamsFromByte(patternRow.getEffect(effectParam).getParamsAsByte() +
										(event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN ? -16 : 16) & 255);
									break;

                default:
                  break;
							}
						}

						break;

					case SDL_CONTROLLER_BUTTON_A:
						bool bPressed = SDL_GameControllerGetButton(SDL_GameControllerFromInstanceID(event.cbutton.which), SDL_CONTROLLER_BUTTON_B);
						int effectParam;
						PatternRow::Column column;

						PatternRow::translateColumnEnum(mTrackEditorState.currentColumn, effectParam, column);

						switch (column)
						{
							case PatternRow::Column::Note:
							case PatternRow::Column::NoteParam1:
							case PatternRow::Column::NoteParam2:
								if (bPressed)
								{
									// B+A = delete

									emptyRow(false, PatternRow::FlagNote);
								}
								else
								{
									if (patternRow.getNote().isEmpty())
									{
										patternRow.getNote().setNoteAndOctave(mEditorState.octave * 12);
									}
								}
								break;

							case PatternRow::Column::EffectType:
							case PatternRow::Column::EffectParam1:
							case PatternRow::Column::EffectParam2:
								if (bPressed)
								{
									// B+A = delete

									emptyRow(false, PatternRow::FlagEffect);
								}
								break;

              default:
                  break;
						}

						break;
				}
				break;
			}
#endif
	}

	return false;
}


void TrackEditor::renderPatternRow(Renderer& renderer, const SDL_Rect& textArea, const PatternRow& row, const Color& color, int columnFlags)
{
	if (row.getNoteNr() != PatternRow::NoNote)
		renderer.renderTextV(textArea, color * Color::getEffectColor(row.getNote()), "%s%x", PatternRow::getNoteName(row.getNoteNr()), row.getOctave());
	else if (!row.getNote().isEmpty())
		renderer.renderTextV(textArea, color * Color::getEffectColor(row.getNote()), "%c%x%x", row.getNote().effect, row.getNote().param1, row.getNote().param2);
	else
		renderer.renderText(textArea, color * Color::getEffectColor(row.getNote()), "---");

	SDL_Rect effectPosition = textArea;

	for (int effectParam = 0 ; effectParam < PatternRow::effectParams ; ++effectParam)
	{
		effectPosition.x += renderer.getFontWidth() * 3;

		if (row.getEffect(effectParam).isEmpty())
			renderer.renderText(effectPosition, color * Color::getEffectColor(row.getEffect(effectParam)), "---");
		else
			renderer.renderTextV(effectPosition, color * Color::getEffectColor(row.getEffect(effectParam)), "%c%x%x",
				row.getEffect(effectParam).effect, row.getEffect(effectParam).param1, row.getEffect(effectParam).param2);
	}
}


void TrackEditor::onDraw(Renderer& renderer, const SDL_Rect& area)
{
	setDirty(false);

	renderer.renderBackground(area);

	int rowNumberWidth = 3 * renderer.getFontWidth() + mRowNumberMargin;
	int trackWidth = mColumns * renderer.getFontWidth() + mTrackMargin;
	int rowHeight = renderer.getFontHeight();

	int countVisible = area.h / rowHeight;
	int firstVisible = mTrackEditorState.currentRow - countVisible / 2;
	int lastVisible = mTrackEditorState.currentRow + countVisible / 2;

	int centerY = (area.h / 2) & ~7;

	if (firstVisible < 0)
		firstVisible = 0;

	if (lastVisible > maxRows)
		lastVisible = maxRows;

	for (int row = firstVisible ; row < lastVisible ; ++row)
	{
		SDL_Rect textArea = {area.x, (row - mTrackEditorState.currentRow) * rowHeight + area.y + centerY, trackWidth, rowHeight};
		Theme::ColorType color = Theme::ColorType::RowCounter;

		if (row == mTrackEditorState.currentRow)
			color = Theme::ColorType::CurrentRow;
		else if (row >= mTrackEditorState.blockStart && row <= mTrackEditorState.blockEnd)
			color = Theme::ColorType::BlockMarker;

		renderer.renderTextV(textArea, color, "%03d", row);
	}

	if (hasFocus())
	{
		int columnWidth = renderer.getFontWidth();

		int columnX = renderer.getFontWidth() * mTrackEditorState.currentColumn;

		SDL_Rect textArea = {mTrackEditorState.currentTrack * trackWidth + area.x + rowNumberWidth + columnX, area.y + centerY, columnWidth, rowHeight};
		renderer.clearRect(textArea, mEditorState.editMode ? Theme::ColorType::EditCursor : Theme::ColorType::NonEditCursor);
	}

	for (int track = 0 ; track < maxTracks ; ++track)
	{
		for (int row = firstVisible ; row < lastVisible ; ++row)
		{
			PatternRow& patternRow = getPatternRow(track, row);
			SDL_Rect textArea = {track * trackWidth + area.x + rowNumberWidth, (row - mTrackEditorState.currentRow) * rowHeight + area.y + centerY, trackWidth, rowHeight};

			Theme::ColorType color = Theme::ColorType::NormalText;;

			if (row == mTrackEditorState.currentRow)
			{
				// Black text color
				color = Theme::ColorType::CurrentRow;
			}
			else if (isRowActive(track, row))
			{
				// Highlight current play row green
				renderer.clearRect(textArea, Theme::ColorType::PlayHead);
			}

			renderPatternRow(renderer, textArea, patternRow, renderer.getTheme().getColor(color));
		}
	}
}


void TrackEditor::playRow()
{
	for (int track = 0 ; track < maxTracks ; ++track)
		mPlayer.triggerNote(track, getPatternRow(track, mTrackEditorState.currentRow));
}


void TrackEditor::insertRow(bool allTracks, int flags)
{
	getCurrentPattern(mTrackEditorState.currentTrack).insertRow(mTrackEditorState.currentRow, flags);
	mTrackEditorState.currentRow.notify();
}


void TrackEditor::deleteRow(bool allTracks, int flags)
{
	getCurrentPattern(mTrackEditorState.currentTrack).deleteRow(mTrackEditorState.currentRow, flags);
}


void TrackEditor::emptyRow(bool allTracks, int flags)
{
	getCurrentPattern(mTrackEditorState.currentTrack).getRow(mTrackEditorState.currentRow).clear(flags);
}


void TrackEditor::copyTrack(int track)
{
	showMessageV(MessageInfo, "Copied track %d on clipboard", track);

	mEditorState.copyBuffer.copy(getCurrentPattern(track), 0, 255);
}


void TrackEditor::pasteTrack(int track)
{
	showMessageV(MessageInfo, "Pasted clipboard on track %d", track);

	mEditorState.copyBuffer.paste(getCurrentPattern(track), 0);
	mTrackEditorState.currentRow.notify();
}


void TrackEditor::setBlockStart(int row)
{
	mTrackEditorState.blockStart = row;

	if (mTrackEditorState.blockEnd < 0)
		mTrackEditorState.blockEnd = row;

	/*if (mTrackEditorState.blockStart > mTrackEditorState.blockEnd)
	{
		int temp = mTrackEditorState.blockStart;
		mTrackEditorState.blockStart = mTrackEditorState.blockEnd;
		mTrackEditorState.blockEnd = temp;
	}*/

	mTrackEditorState.currentRow.notify();
}


void TrackEditor::setBlockEnd(int row)
{
	mTrackEditorState.blockEnd = row;

	if (mTrackEditorState.blockStart < 0)
		mTrackEditorState.blockStart = row;

	/*if (mTrackEditorState.blockStart > mTrackEditorState.blockEnd)
	{
		int temp = mTrackEditorState.blockStart;
		mTrackEditorState.blockStart = mTrackEditorState.blockEnd;
		mTrackEditorState.blockEnd = temp;
	}*/

	mTrackEditorState.currentRow.notify();
}


void TrackEditor::copyBlock(int track)
{
	if (mTrackEditorState.blockStart < 0 || mTrackEditorState.blockEnd < 0)
		return;

	mEditorState.copyBuffer.copy(getCurrentPattern(track), mTrackEditorState.blockStart, mTrackEditorState.blockEnd);
}


void TrackEditor::pasteBlock(int track)
{
	mEditorState.copyBuffer.paste(getCurrentPattern(track), mTrackEditorState.currentRow);
	mTrackEditorState.currentRow.notify();
}


void TrackEditor::copyCurrentTrack()
{
	copyTrack(mTrackEditorState.currentTrack);
}


void TrackEditor::pasteCurrentTrack()
{
	pasteTrack(mTrackEditorState.currentTrack);
}


void TrackEditor::copyCurrentBlock()
{
	copyBlock(mTrackEditorState.currentTrack);
	showMessage(MessageInfo, "Copied block");
}


void TrackEditor::pasteCurrentBlock()
{
	pasteBlock(mTrackEditorState.currentTrack);
	showMessage(MessageInfo, "Block pasted");
}


void TrackEditor::setEditSkip(int skip)
{
	mTrackEditorState.editSkip = skip;
	showMessageV(MessageInfo, replacePreviousMessage, "Edit skip set to %d", skip);
}


void TrackEditor::setBlockStartToCurrentRow()
{
	setBlockStart(mTrackEditorState.currentRow);
}


void TrackEditor::setBlockEndToCurrentRow()
{
	setBlockEnd(mTrackEditorState.currentRow);
}
