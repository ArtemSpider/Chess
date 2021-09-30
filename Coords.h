#pragma once

#include <string>
#include <sstream>
#include <math.h>

#if __has_include(<SFML/System/Vector2.hpp>)
#   include <SFML/System/Vector2.hpp>
#endif
#if __has_include(<b2_math.h>)
#   include <b2_math.h>
#endif


template <typename Type>
class Point
{
public:
    Type x, y;


    Point()
    : x(), y()
    {}

    template <typename CType>
	constexpr explicit Point(const CType& xy)
    : x((Type)xy), y((Type)xy)
    {}

    template <typename CType1, typename CType2>
    constexpr Point(const CType1& x, const CType2& y)
    : x((Type)x), y((Type)y)
    {}

    template <typename CType>
    constexpr Point(const Point<CType>& copy)
    : x((Type)copy.x), y((Type)copy.y)
    {}

    template <typename CType1, typename CType2>
    constexpr Point(const std::pair<CType1, CType2>& copy)
    : x(copy.first), y(copy.second)
    {}

#if __has_include(<SFML/System/Vector2.hpp>)
    template <typename CType>
    constexpr Point(const sf::Vector2<CType>& copy)
        : x((Type)copy.x), y((Type)copy.y)
    {}
#endif

#if __has_include(<b2_math.h>)
    constexpr Point(const b2Vec2& copy)
        : x((Type)copy.x), y((Type)copy.y)
    {}
#endif


    constexpr Type sum() const // x + y
    {
        return x + y;
    }
    constexpr Type rsum() const // y + x
    {
        return y + x;
    }
    constexpr Type diff() const // x - y
    {
        return x - y;
    }
    constexpr Type rdiff() const // y - x
    {
        return y - x;
    }
    constexpr Type mult() const // x * y
    {
        return x * y;
    }
    constexpr Type rmult() const // y * x
    {
        return y * x;
    }
    constexpr Type div() const // x / y
    {
        return x / y;
    }
    constexpr Type rdiv() const // y / x
    {
        return y / x;
    }
    constexpr Type mod() const // x % y
    {
        return x % y;
    }
    constexpr Type rmod() const // y % x
    {
        return y % x;
    }
    Type get(Type(*f)(Type, Type)) const
    {
        return f(x, y);
    }
    Type rget(Type(*f)(Type, Type)) const
    {
        return f(y, x);
    }
    constexpr Point<Type> reversed() const
    {
        return Point<Type>(y, x);
    }


    constexpr void abs()
    {
        x = abs(x);
        y = abs(y);
    }
    constexpr void sqr()
    {
        x = x * x;
        y = y * y;
    }
    constexpr void sqrt()
    {
        x = std::sqrt(x);
        y = std::sqrt(y);
    }
    constexpr void reverse()
    {
        Type t = x;
        x = y;
        y = t;
    }
    void apply(Type (*f)(Type))
    {
        x = f(x);
        y = f(y);
    }


    constexpr Point operator+ (const Type& oth) const
    {
        return Point(x + oth, y + oth);
    }
    constexpr Point operator+ (const Point& oth) const
    {
        return Point(x + oth.x, y + oth.y);
    }

    constexpr Point operator- (const Type& oth) const
    {
        return Point(x - oth, y - oth);
    }
    constexpr Point operator- (const Point& oth) const
    {
        return Point(x - oth.x, y - oth.y);
    }

    constexpr Point operator* (const Type& oth) const
    {
        return Point(x * oth, y * oth);
    }
    constexpr Point operator* (const Point& oth) const
    {
        return Point(x * oth.x, y * oth.y);
    }

    constexpr Point operator/ (const Type& oth) const
    {
        return Point(x / oth, y / oth);
    }
    constexpr Point operator/ (const Point& oth) const
    {
        return Point(x / oth.x, y / oth.y);
    }

    constexpr Point operator% (const Type& oth) const
    {
        return Point(x % oth, y % oth);
    }
    constexpr Point operator% (const Point& oth) const
    {
        return Point(x % oth.x, y % oth.y);
    }


    constexpr bool operator== (const Type& oth) const
    {
        return (x == oth && y == oth);
    }
    constexpr bool operator== (const Point& oth) const
    {
        return (x == oth.x && y == oth.y);
    }

    constexpr bool operator!= (const Type& oth) const
    {
        return !(*this == oth);
    }
    constexpr bool operator!= (const Point& oth) const
    {
        return !(*this == oth);
    }

    constexpr bool operator> (const Type& oth) const
    {
        return (x > oth && y > oth);
    }
    constexpr bool operator> (const Point& oth) const
    {
        return (x > oth.x && y > oth.y);
    }

    constexpr bool operator< (const Type& oth) const
    {
        return (x < oth && y < oth);
    }
    constexpr bool operator< (const Point& oth) const
    {
        return (x < oth.x && y < oth.y);
    }

    constexpr bool operator>= (const Type& oth) const
    {
        return (x >= oth && y >= oth);
    }
    constexpr bool operator>= (const Point& oth) const
    {
        return (x >= oth.x && y >= oth.y);
    }

    constexpr bool operator<= (const Type& oth) const
    {
        return (x <= oth && y <= oth);
    }
    constexpr bool operator<= (const Point& oth) const
    {
        return (x <= oth.x && y <= oth.y);
    }


    constexpr Point& operator+= (const Type& oth)
    {
        *this = *this + oth;
        return *this;
    }
    constexpr Point& operator+= (const Point& oth)
    {
        *this = *this + oth;
        return *this;
    }

    constexpr Point& operator-= (const Type& oth)
    {
        *this = *this - oth;
        return *this;
    }
    constexpr Point& operator-= (const Point& oth)
    {
        *this = *this - oth;
        return *this;
    }

    constexpr Point& operator*= (const Type& oth)
    {
        *this = *this * oth;
        return *this;
    }
    constexpr Point& operator*= (const Point& oth)
    {
        *this = *this * oth;
        return *this;
    }

    constexpr Point& operator/= (const Type& oth)
    {
        *this = *this / oth;
        return *this;
    }
    constexpr Point& operator/= (const Point& oth)
    {
        *this = *this / oth;
        return *this;
    }

    constexpr Point& operator%= (const Type& oth)
    {
        *this = *this % oth;
        return *this;
    }
    constexpr Point& operator%= (const Point& oth)
    {
        *this = *this % oth;
        return *this;
    }

    /// Префиксные
    constexpr Point& operator++ ()
    {
        x++; y++;
        return *this;
    }
    constexpr Point& operator-- ()
    {
        x--; y--;
        return *this;
    }

    /// Постфиксные
    constexpr Point operator++ (int)
    {
        Point prev = *this;
        ++*this;
        return prev;
    }
    constexpr Point operator-- (int)
    {
        Point prev = *this;
        --*this;
        return prev;
    }

    // Унарный минус/плюс
    constexpr Point operator-() const
    {
        return Point(-x, -y);
    }
    constexpr Point operator+() const
    {
        return Point(+x, +y);
    }


    // Явные преобразования
    std::string AsString(const std::string& between = ", ") const
    {
        std::stringstream ss;
        ss << x << between << y;
        return ss.str();
    }
    std::pair<Type, Type> AsPair() const
    {
        return std::pair<Type, Type>(x, y);
    }
#if __has_include(<SFML/System/Vector2.hpp>)
    sf::Vector2<Type> AsVector2() const
    {
        return sf::Vector2<Type>(x, y);
    }
#endif
#if __has_include(<b2_math.h>)
    b2Vec2 AsB2Vec2() const
    {
        return b2Vec2(x, y);
    }
#endif

    // Неявные преобразования
    operator std::pair<Type, Type>() const
    {
        return std::pair<Type, Type>(x, y);
    }
#if __has_include(<SFML/System/Vector2.hpp>)
    operator sf::Vector2<Type>() const
    {
        return sf::Vector2<Type>(x, y);
    }
#endif
#if __has_include(<b2_math.h>)
    operator b2Vec2() const
    {
        return b2Vec2(x, y);
    }
#endif
};

typedef Point<int> Position;
typedef Point<int> Size;

typedef Point<float> PositionF;
typedef Point<float> SizeF;