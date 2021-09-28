#pragma once

#include "TextBox.h"


#define DELETE_KEY	0x08
#define ESCAPE_KEY	0x1B

// static class for easy control over TextBox
class TextBoxController
{
private:

public:
	enum class ActionType
	{
		None,			// no action was performed
		SymbolRemoved,	// last symbol was removed
		SymbolAdded,	// new symbol was added to the back
		LostFocus,		// textBox is no longer selected
		GainFocus		// textBox is now selected
	};

	static ActionType CheckEvent(TextBox* textBox, sf::Event event)
	{
		if (event.type == sf::Event::EventType::TextEntered)
		{
			if (textBox->isSelected())
			{
				if (event.text.unicode == DELETE_KEY)
				{
					if (!textBox->isEmpty())
					{
						textBox->pop_back();
						return ActionType::SymbolRemoved;
					}
				}
				else if (event.text.unicode == ESCAPE_KEY)
				{
					textBox->resetSelected();
					return ActionType::LostFocus;
				}
				else
				{
					textBox->push(event.text.unicode);
					return ActionType::SymbolAdded;
				}
			}
		}
		else if (event.type == sf::Event::EventType::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Button::Left)
			{
				sf::FloatRect bounds = textBox->getBounds();
				sf::Vector2f clickPos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);

				if (bounds.contains(clickPos))
				{
					textBox->setSelected(true);
					return ActionType::GainFocus;
				}
				else
				{
					textBox->resetSelected();
					return ActionType::LostFocus;
				}
			}
		}

		return ActionType::None;
	}
};