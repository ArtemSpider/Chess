#pragma once

#include <unordered_set>

#include <SFML/Graphics.hpp>


class DrawableArray : public sf::Drawable, public sf::Transformable
{
public:
    typedef std::vector<const Drawable*> Container;
private:
    Container container;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        for (auto& d : container)
            target.draw(*d, states);
    }
public:
    DrawableArray() : container() {}

    template <class Iter>
    DrawableArray(Iter first, Iter last) : container(first, last) {}

    DrawableArray(std::initializer_list<sf::Drawable*> list) : container(list.begin(), list.end()) {}
    

    void push(const Drawable* d)
    {
        container.push_back(d);
    }
    void erase(const Drawable* d)
    {
        container.erase(std::find(container.begin(), container.end(), d));
    }
    bool contains(const Drawable* d)
    {
        return std::find(container.begin(), container.end(), d) != container.end();
    }


    Container& getContainer()
    {
        return container;
    }
    const Container& getContainer() const
    {
        return container;
    }
};