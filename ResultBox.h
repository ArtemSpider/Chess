#pragma once

#include "Coords.h"
#include "Board.h"

#include <SFML/Graphics.hpp>


class ResultBox : public sf::Drawable, public sf::Transformable
{
private:
	SizeF size;

	mutable sf::RectangleShape background;
	mutable sf::Text resultText;
	mutable sf::Text reasonText;

	GameState state;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		states.transform *= getTransform();


		background.setSize(size);


		switch (state.state)
		{
		case GameState::State::WhiteWon: resultText.setString("White won"); break;
		case GameState::State::BlackWon: resultText.setString("Black won"); break;
		case GameState::State::Draw:	 resultText.setString("Draw"); break;
		default:						 resultText.setString("NULL"); break;
		}

		sf::FloatRect textRect = resultText.getLocalBounds();
		resultText.setOrigin(textRect.left + textRect.width / 2.0f,
			textRect.top + textRect.height / 2.0f);


		reasonText.setString(state.reason);

		textRect = reasonText.getLocalBounds();
		reasonText.setOrigin(textRect.left + textRect.width / 2.0f,
			textRect.top + textRect.height / 2.0f);


		target.draw(background, states);
		target.draw(resultText, states);
		target.draw(reasonText, states);
	}
public:
	ResultBox(const sf::Font& font, sf::Vector2f position, sf::Vector2f size, GameState state) :
		resultText("", font), reasonText("", font), state(state)
	{
		setPosition(position);
		SetSize(size);

		resultText.setFillColor(sf::Color::Black);
		reasonText.setFillColor(sf::Color::Black);
	}

	void SetSize(SizeF size)
	{
		this->size = size;
		background.setOrigin(this->size / 2.f);
	}
	void SetResultCharacterSize(unsigned size)
	{
		resultText.setCharacterSize(size);
	}
	void SetReasonCharacterSize(unsigned size)
	{
		resultText.setCharacterSize(size);
	}
	void SetResultTextPadding(PositionF padding)
	{
		resultText.setPosition(padding);
	}
	void SetReasonTextPadding(PositionF padding)
	{
		reasonText.setPosition(padding);
	}
	void SetState(GameState state)
	{
		this->state = state;
	}

	sf::RectangleShape& GetBackground()
	{
		return background;
	}
	const sf::RectangleShape& GetBackground() const
	{
		return background;
	}

	sf::Text& GetResultText()
	{
		return resultText;
	}
	const sf::Text& GetResultText() const
	{
		return resultText;
	}

	sf::Text& GetReasonText()
	{
		return reasonText;
	}
	const sf::Text& GetReasonText() const
	{
		return reasonText;
	}
};