#pragma once

#include <optional>

#include <SFML/Graphics.hpp>


class TextBox : public sf::Drawable, public sf::Transformable
{
private:
	sf::String string;							// string before transformations
	mutable sf::Text text;						// displayed text

	std::optional<size_t> maxSize;				// maximum number of symbols in the displayed string

	bool selected;								// is TextBox selected

	sf::Vector2f size;							// size of the TextBox
	sf::Vector2f padding;						// padding from the middle-left

	sf::Color backgroundColor;


	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		states.transform *= getTransform();

		sf::String str = string;

		if (isSelected() && !isFull())
			str += '_';

		text.setPosition(padding);
		text.setString(str);


		sf::RectangleShape rect;
		rect.setSize(size);
		rect.setFillColor(backgroundColor);
		rect.setOutlineColor(sf::Color::White);
		rect.setOutlineThickness(1);


		target.draw(rect, states);
		target.draw(text, states);
	}
public:
	TextBox(const sf::Font& font, unsigned int characterSize = 30) :
		text("", font, characterSize), selected(false), backgroundColor(sf::Color::Black)
	{}
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
		this->padding = padding;
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
		this->backgroundColor = backgroundColor;
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
		return size;
	}
	sf::FloatRect getBounds() const
	{
		return sf::FloatRect(getPosition(), getTextBoxSize());
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
	const sf::Font* getFont() const
	{
		text.getFont();
	}
	unsigned getCharacterSize()
	{
		return text.getCharacterSize();
	}
	sf::Uint32 getTextStyle()
	{
		text.getStyle();
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