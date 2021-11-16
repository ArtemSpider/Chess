#pragma once

#include <memory>
#include <functional>
#include <set>

#include <SFML/Graphics.hpp>


class Button : public sf::Drawable, public sf::Transformable
{
private:
    typedef std::set<Button*> ButtonGroup;

    bool pressed;

    sf::Vector2f size;

    ButtonGroup* group;

    sf::Drawable* notPressedDrawable;       // will be drawn if button is not pressed
    sf::Drawable* pressedDrawable;          // will be drawn if button is pressed

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        if (pressed)
        {
            if (pressedDrawable != nullptr)
                target.draw(*pressedDrawable, states);
        }
        else
        {
            if (notPressedDrawable != nullptr)
                target.draw(*notPressedDrawable, states);
        }
    }
public:
    // will button unpress when you click on it twice or not
    bool canBeUnpressed;

    typedef std::function<void()> Action;
    Action pressedAction;
    Action unpressedAction;

    Button(sf::Drawable* notPressedDraw, sf::Drawable* pressedDraw, sf::Vector2f size = sf::Vector2f(0.f, 0.f)) :
        pressed(false), size(size), group(nullptr),
        notPressedDrawable(notPressedDraw), pressedDrawable(pressedDraw),
        canBeUnpressed(true), pressedAction(nullptr), unpressedAction(nullptr)
    {}

    ~Button()
    {
        if (pressedDrawable != nullptr)
            delete pressedDrawable;
        if (notPressedDrawable != nullptr)
            delete notPressedDrawable;
    }

    // connect buttons together, only one button in the group can be pressed simultaneously
    void connect(Button* button)
    {
        if (group == nullptr)
        {
            group = new ButtonGroup();
            group->insert(this);
        }

        if (button->group != nullptr)
        {
            auto* g = button->group;
            for (auto& b : *button->group)
            {
                this->group->insert(b);
                b->group = this->group;
            }
            delete g;
        }
        else
        {
            button->group = group;
            this->group->insert(button);
        }
    }
    // disconnect button from others
    void disconnect()
    {
        if (group == nullptr)
            throw "Button is not connected";

        group->erase(this);
        if (group->empty())
            delete group;
        group = nullptr;
    }

    void press()
    {
        if (pressed) return;

        pressed = true;
        if (pressedAction != nullptr)
            pressedAction();
    }
    void unpress()
    {
        if (!pressed) return;

        pressed = false;
        if (unpressedAction != nullptr)
            unpressedAction();
    }

    bool isPressed() const
    {
        return pressed;
    }
    void click()
    {
        if (group != nullptr)
            for (auto& b : *group)
                if (b != this)
                    b->unpress();

        if (canBeUnpressed)
        {
            pressed = !pressed;
            auto& action = (pressed ? pressedAction: unpressedAction);
            if (action != nullptr)
                action();
        }
        else
        {
            if (!pressed && pressedAction != nullptr)
                pressedAction();
            pressed = true;
        }
    }

    sf::FloatRect getBounds() const
    {
        return sf::FloatRect(getPosition() - getOrigin(), size);
    }
};