#pragma once

#include "Board.h"


#include <SFML/Graphics.hpp>

class Graphics
{
	sf::Texture piecesTexture;

	vector<sf::Sprite> whitePieces; // in the order of PieceType
	vector<sf::Sprite> blackPieces;
public:
	static const Size SQUARE_SIZE;

	static const sf::Color WHITE_COLOR;
	static const sf::Color BLACK_COLOR;

	Graphics()
	{
		piecesTexture.loadFromFile("images/Pieces.png", sf::IntRect(0, 0, 1020, 340));

		whitePieces.emplace_back(piecesTexture,
			sf::IntRect(5 * SQUARE_SIZE.x, 0, SQUARE_SIZE.x, SQUARE_SIZE.y));
		whitePieces.emplace_back(piecesTexture,
			sf::IntRect(3 * SQUARE_SIZE.x, 0, SQUARE_SIZE.x, SQUARE_SIZE.y));
		whitePieces.emplace_back(piecesTexture,
			sf::IntRect(2 * SQUARE_SIZE.x, 0, SQUARE_SIZE.x, SQUARE_SIZE.y));
		whitePieces.emplace_back(piecesTexture,
			sf::IntRect(4 * SQUARE_SIZE.x, 0, SQUARE_SIZE.x, SQUARE_SIZE.y));
		whitePieces.emplace_back(piecesTexture,
			sf::IntRect(1 * SQUARE_SIZE.x, 0, SQUARE_SIZE.x, SQUARE_SIZE.y));
		whitePieces.emplace_back(piecesTexture,
			sf::IntRect(0 * SQUARE_SIZE.x, 0, SQUARE_SIZE.x, SQUARE_SIZE.y));

		blackPieces.emplace_back(piecesTexture,
			sf::IntRect(5 * SQUARE_SIZE.x, 0, SQUARE_SIZE.x, SQUARE_SIZE.y));
		blackPieces.emplace_back(piecesTexture,
			sf::IntRect(3 * SQUARE_SIZE.x, 0, SQUARE_SIZE.x, SQUARE_SIZE.y));
		blackPieces.emplace_back(piecesTexture,
			sf::IntRect(2 * SQUARE_SIZE.x, 0, SQUARE_SIZE.x, SQUARE_SIZE.y));
		blackPieces.emplace_back(piecesTexture,
			sf::IntRect(4 * SQUARE_SIZE.x, 0, SQUARE_SIZE.x, SQUARE_SIZE.y));
		blackPieces.emplace_back(piecesTexture,
			sf::IntRect(1 * SQUARE_SIZE.x, 0, SQUARE_SIZE.x, SQUARE_SIZE.y));
		blackPieces.emplace_back(piecesTexture,
			sf::IntRect(0 * SQUARE_SIZE.x, 0, SQUARE_SIZE.x, SQUARE_SIZE.y));
	}

	void Draw(sf::RenderWindow& window, const ChessBoard& board)
	{
		for (int i = 0; i < board.SIZE.y; i++)
		{
			for (int j = 0; j < board.SIZE.x; j++)
			{
				sf::RectangleShape sq(Point<float>(SQUARE_SIZE).AsVector2());
				sq.setFillColor((i + j) % 2 ? BLACK_COLOR : WHITE_COLOR);
				sq.setPosition(Point<float>(SQUARE_SIZE * Position(j, i)));

				window.draw(sq);
				

				const Piece* p = board.GetPieceAt({ j, board.SIZE.y - 1 - i });
				if (p != nullptr)
				{
					sf::Sprite spr = (p->GetTeam() == PlayerTeam::White ? whitePieces : blackPieces)[(size_t)p->GetType()];
					spr.setPosition(Point<float>(SQUARE_SIZE * Position(j, i)));

					window.draw(spr);
				}
			}
		}
	}
};

const Size Graphics::SQUARE_SIZE = Size(106, 106);
const sf::Color Graphics::WHITE_COLOR = sf::Color(238, 238, 210);
const sf::Color Graphics::BLACK_COLOR = sf::Color(118, 150, 86);