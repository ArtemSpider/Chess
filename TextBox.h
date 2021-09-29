#pragma once

#include <optional>

#include <SFML/Graphics.hpp>


class TextBox : public sf::Drawable, public sf::Transformable
{
private:
	sf::String string;							// string before transformations
	mutable sf::Text text;						// displayed text
	mutable sf::RectangleShape outlineRect;		// displayed box

	std::optional<size_t> maxSize;				// maximum number of symbols in the displayed string

	sf::Vector2f size;							// size of the TextBox
	bool selected;								// is TextBox selected


	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		states.transform *= getTransform();

		sf::String str = string;

		if (isSelected() && !isFull())
			str += '_';

		text.setString(str);
		outlineRect.setSize(size);

		target.draw(outlineRect, states);
		target.draw(text, states);
	}
public:
	TextBox(const sf::Font& font, unsigned int characterSize = 30) :
		text("", font, characterSize), selected(false)
	{
		setBackgroundColor(sf::Color::Black);
		setOutlineColor(sf::Color::White);
		setOutlineThickness(1.f);
	}
	~TextBox() {}


	// append the string	
	void push(sf::String str)
	{
		string += str;
		if (maxSize.has_value() && string.getSize() > maxSize.value())
			string = string.substring(0, maxSize.value());
	}
	// returns first element of the string
	sf::Uint32 front()
	{
		return string[0];
	}
	// returns last element of the string
	sf::Uint32 back()
	{
		return string[string.getSize() - 1];
	}
	// removes first element from the string
	void pop_front()
	{
		string.erase(0);
	}
	// removes last element from the string
	void pop_back()
	{
		string.erase(string.getSize() - 1);
	}
	// erases count symbols from the string, starting from position
	void erase(size_t position, size_t count = 1Ui64)
	{
		string.erase(position, count);
	}
	// removes all symbols from the string
	void clear()
	{
		string.clear();
	}
	
	// automatically sets size
	void autoSizeX(float widthMultiplyer = 0.75f)
	{
		size.x = text.getCharacterSize() * maxSize.value_or(10) * widthMultiplyer;
	}
	void autoSizeY(float heightMultiplyer = 1.25f)
	{
		size.y = text.getCharacterSize() * heightMultiplyer;
	}
	void autoSize(float widthMultiplyer = 0.75f, float heightMultiplyer = 1.25f)
	{
		autoSizeX(widthMultiplyer);
		autoSizeY(heightMultiplyer);
	}

	// set displayed string
	void setString(sf::String string)
	{
		this->string = string;
		if (maxSize.has_value() && this->string.getSize() > maxSize.value())
			this->string = this->string.substring(0, maxSize.value());
	}
	// set size of the TextBox
	void setSize(sf::Vector2f size)
	{
		this->size = size;
	}
	// set padding betweeen position of the TextBox and the text
	void setPadding(sf::Vector2f padding)
	{
		text.setPosition(padding);
	}
	void setText(sf::Text text)
	{
		this->text = text;
	}
	void setFont(const sf::Font& font)
	{
		this->text.setFont(font);
	}
	void setCharacterSize(unsigned size)
	{
		this->text.setCharacterSize(size);
	}
	void setTextStyle(sf::Uint32 style)
	{
		this->text.setStyle(style);
	}
	void setBackgroundColor(sf::Color backgroundColor)
	{
		outlineRect.setFillColor(backgroundColor);
	}
	void setOutlineColor(sf::Color outlineColor)
	{
		outlineRect.setOutlineColor(outlineColor);
	}
	void setOutlineThickness(float thickness)
	{
		outlineRect.setOutlineThickness(thickness);
	}

	// set max number of symbols of the displayed string
	void setMaxStringSize(size_t maxSize)
	{
		this->maxSize = maxSize;
	}
	void resetMaxStringSize()
	{
		this->maxSize.reset();
	}


	// if selected, different pattern will be applied
	void setSelected(bool selected = true)
	{
		this->selected = selected;
	}
	void resetSelected()
	{
		this->selected = false;
	}


	sf::String& getString()
	{
		return string;
	}
	const sf::String& getString() const
	{
		return string;
	}
	sf::Vector2f getTextBoxSize() const
	{
		return sf::Vector2f(size.x * getScale().x, size.y * getScale().y);
	}
	sf::FloatRect getBounds() const
	{
		return sf::FloatRect(getPosition() - getOrigin(), getTextBoxSize());
	}
	size_t getStringSize() const
	{
		return string.getSize();
	}
	sf::Text& getText()
	{
		return text;
	}
	const sf::Text& getText() const
	{
		return text;
	}
	sf::RectangleShape& getOutline()
	{
		return outlineRect;
	}
	const sf::RectangleShape& getOutline() const
	{
		return outlineRect;
	}
	const sf::Font* getFont() const
	{
		text.getFont();
	}
	unsigned getCharacterSize() const
	{
		return text.getCharacterSize();
	}
	sf::Uint32 getTextStyle() const
	{
		text.getStyle();
	}
	sf::Color getBackgroundColor() const
	{
		return outlineRect.getFillColor();
	}
	sf::Color getOutlineColor() const
	{
		return outlineRect.getOutlineColor();
	}
	float getOutlineThickness() const
	{
		return outlineRect.getOutlineThickness();
	}
	// maximum size of this sf::String
	std::optional<size_t> getMaxStringSize() const
	{
		return maxSize;
	}
	bool getSelected() const
	{
		return selected;
	}
	bool isSelected() const
	{
		return selected;
	}
	bool isEmpty() const
	{
		return string.isEmpty();
	}
	bool isFull() const
	{
		return (maxSize.has_value() ? maxSize.value() == string.getSize() : false);
	}
};