#pragma once

#include "Board.h"


#include <SFML/Graphics.hpp>

class Graphics
{
	sf::Texture piecesTexture;

	sf::RenderWindow window;
	const ChessBoard* board;

	vector<sf::Sprite> whitePieces; // in the order of PieceType
	vector<sf::Sprite> blackPieces;

	const Piece** selectedPiecePtr;
public:
	static const Size SQUARE_SIZE;

	static const sf::Color WHITE_COLOR;
	static const sf::Color BLACK_COLOR;

	Graphics(const ChessBoard* board, const Piece** selectedPiece) :
		window(sf::VideoMode(Graphics::SQUARE_SIZE.x * board->SIZE.x, Graphics::SQUARE_SIZE.y * board->SIZE.y), "Chess"),
		board(board), selectedPiecePtr(selectedPiece)
	{
		piecesTexture.loadFromFile("images/Pieces.png", sf::IntRect(0, 0, 1020, 340));


		const int imageOrder[] = { 5, 3, 2, 4, 1, 0 };

		for (int i = 0; i < (int)PieceType::Count; i++)
		{
			whitePieces.emplace_back(piecesTexture,
				sf::IntRect(imageOrder[i] * SQUARE_SIZE.x, 0, SQUARE_SIZE.x, SQUARE_SIZE.y));
			whitePieces.back().setOrigin(Point<float>(SQUARE_SIZE / 2));
		}

		for (int i = 0; i < (int)PieceType::Count; i++)
		{
			blackPieces.emplace_back(piecesTexture,
				sf::IntRect(imageOrder[i] * SQUARE_SIZE.x, SQUARE_SIZE.y, SQUARE_SIZE.x, SQUARE_SIZE.y));
			blackPieces.back().setOrigin(Point<float>(SQUARE_SIZE / 2));
		}
	}

	sf::RenderWindow& GetWindow()
	{
		return window;
	}

	void Draw()
	{
		window.clear();

		sf::RectangleShape sq(Point<float>(SQUARE_SIZE).AsVector2());
		for (int i = 0; i < board->SIZE.y; i++)
		{
			for (int j = 0; j < board->SIZE.x; j++)
			{
				sq.setFillColor((i + j) % 2 ? BLACK_COLOR : WHITE_COLOR);
				sq.setPosition(Point<float>(SQUARE_SIZE * Position(j, i)));

				window.draw(sq);


				const Piece* p = board->GetPieceAt({ j, board->SIZE.y - 1 - i });
				if (p != nullptr)
				{
					sf::Sprite spr = (p->GetTeam() == PlayerTeam::White ? whitePieces : blackPieces)[(size_t)p->GetType()];
					spr.setPosition(Point<float>(SQUARE_SIZE * Position(j, i) + SQUARE_SIZE / 2));

					window.draw(spr);
				}
			}
		}

		if (*selectedPiecePtr != nullptr)
		{
			auto possibleMoves = (*selectedPiecePtr)->GetMoves();

			sf::CircleShape circle(24);
			circle.setFillColor(sf::Color(0, 0, 0, 64));
			circle.setOrigin(Point<float>(24));
			for (auto& pos : possibleMoves)
			{
				circle.setPosition(Point<float>(SQUARE_SIZE * Position(pos.x, board->SIZE.y - 1 - pos.y) + SQUARE_SIZE / 2));
				window.draw(circle);
			}
		}

		window.display();
	}
};

const Size Graphics::SQUARE_SIZE = Size(106, 106);
const sf::Color Graphics::WHITE_COLOR = sf::Color(238, 238, 210);
const sf::Color Graphics::BLACK_COLOR = sf::Color(118, 150, 86);