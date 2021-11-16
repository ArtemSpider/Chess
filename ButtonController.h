#pragma once

#include "Button.h"

// static class for easy control over Button
class ButtonController
{
	ButtonController() {}
public:
	enum class ActionType
	{
		None,			// no action was performed
		Clicked,		// button was clicked
	};

	static ActionType CheckEvent(Button* button, sf::Event event)
	{
		if (event.type == sf::Event::EventType::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Button::Left)
			{
				sf::FloatRect bounds = button->getBounds();
				sf::Vector2i clickPos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);

				if (bounds.contains(sf::Vector2f(clickPos)))
				{
					button->click();
					return ActionType::Clicked;
				}
			}
		}

		return ActionType::None;
	}
};