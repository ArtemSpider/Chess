#pragma once

#include <optional>

#include "Board.h"

#include <SFML/Graphics.hpp>


class Graphics
{
	sf::Texture piecesTexture;

	sf::RenderWindow window;

	vector<sf::Sprite> whitePieces; // in the order of PieceType
	vector<sf::Sprite> blackPieces;

	const ChessBoard* board;
	const Piece** selectedPiecePtr;

	optional<GameState> resultWindowData;

	sf::Font font;

	void DrawBoard()
	{
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
			auto possibleMoves = board->GetLegalMoves(*selectedPiecePtr);

			sf::CircleShape circle(24);
			circle.setFillColor(sf::Color(0, 0, 0, 64));
			circle.setOrigin(Point<float>(24));
			for (auto& pos : possibleMoves)
			{
				circle.setPosition(Point<float>(SQUARE_SIZE * Position(pos.x, board->SIZE.y - 1 - pos.y) + SQUARE_SIZE / 2));
				window.draw(circle);
			}
		}

		if (resultWindowData.has_value())
		{
			assert(resultWindowData->state != GameState::State::Game);


			Size rectSize(SQUARE_SIZE.x * 3.0f, SQUARE_SIZE.y * 1.5f);

			sf::RectangleShape rect;
			rect.setPosition(Point<float>(SQUARE_SIZE * board->SIZE / 2));
			rect.setSize(Point<float>(rectSize));
			rect.setOrigin(Point<float>(rectSize / 2));
			rect.setFillColor(sf::Color::White);

			window.draw(rect);


			sf::Text resultText;
			resultText.setFillColor(sf::Color::Black);
			resultText.setFont(font);
			resultText.setCharacterSize(40);
			resultText.setPosition(Point<float>(SQUARE_SIZE * board->SIZE / 2) - Point<float>(0.f, SQUARE_SIZE.y * 0.2f));

			switch (resultWindowData->state)
			{
			case GameState::State::WhiteWon: resultText.setString("White won"); break;
			case GameState::State::BlackWon: resultText.setString("Black won"); break;
			case GameState::State::Draw:	 resultText.setString("Draw"); break;
			}

			sf::FloatRect textRect = resultText.getLocalBounds();
			resultText.setOrigin(textRect.left + textRect.width / 2.0f,
				textRect.top + textRect.height / 2.0f);

			window.draw(resultText);


			sf::Text reasonText;
			reasonText.setFillColor(sf::Color::Black);
			reasonText.setFont(font);
			reasonText.setCharacterSize(25);
			reasonText.setPosition(Point<float>(SQUARE_SIZE * board->SIZE / 2) + Point<float>(0.f, SQUARE_SIZE.y * 0.3f));
			reasonText.setString("by " + resultWindowData->reason);

			textRect = reasonText.getLocalBounds();
			reasonText.setOrigin(textRect.left + textRect.width / 2.0f,
				textRect.top + textRect.height / 2.0f);

			window.draw(reasonText);
		}
	}


	string TimeToString(int time)
	{
		if (time / 3600 >= 1)	// mote then an hour
			return to_string(time / 3600) + ":" + to_string(time % 3600 / 60) + ":" + to_string(time % 60);
		return to_string(time % 3600 / 60) + ":" + (time % 60 < 10 ? "0" : "") + to_string(time % 60);
	}

	const int* remainingTimeWhite;	// in seconds
	const int* remainingTimeBlack;	// in seconds
	void DrawSide()
	{
		// drawing remaining time
		{
			sf::Text time;
			time.setFillColor(sf::Color::White);
			time.setFont(font);
			time.setCharacterSize(40);
			time.setPosition(Point<float>(
				SQUARE_SIZE.x * board->SIZE.x + SQUARE_SIZE.y / 4.f, 
				SQUARE_SIZE.y / 2.f));
			time.setString(TimeToString(*remainingTimeBlack));

			sf::FloatRect textRect = time.getLocalBounds();
			time.setOrigin(0.f, textRect.top + textRect.height / 2.0f);

			window.draw(time);
		}
		{
			sf::Text time;
			time.setFillColor(sf::Color::White);
			time.setFont(font);
			time.setCharacterSize(40);
			time.setPosition(Point<float>(
				SQUARE_SIZE.x * board->SIZE.x + SQUARE_SIZE.y / 4.f, 
				SQUARE_SIZE.y * board->SIZE.y - SQUARE_SIZE.y / 2.f));
			time.setString(TimeToString(*remainingTimeWhite));

			sf::FloatRect textRect = time.getLocalBounds();
			time.setOrigin(0.f, textRect.top + textRect.height / 2.0f);

			window.draw(time);
		}
	}
public:
	static const Size SQUARE_SIZE;

	static const sf::Color WHITE_COLOR;
	static const sf::Color BLACK_COLOR;

	static const string PATH_TO_FONT;


	Graphics(const ChessBoard* board, const Piece** selectedPiece, const int* remainingTimeWhite, const int* remainingTimeBlack) :
		window(sf::VideoMode(Graphics::SQUARE_SIZE.x * board->SIZE.x + 200, Graphics::SQUARE_SIZE.y * board->SIZE.y), "Chess", sf::Style::Titlebar | sf::Style::Close),
		board(board), selectedPiecePtr(selectedPiece), font(),
		remainingTimeWhite(remainingTimeWhite), remainingTimeBlack(remainingTimeBlack)
	{
		window.setFramerateLimit(60);

		piecesTexture.loadFromFile("images/Pieces.png", sf::IntRect(0, 0, 1020, 340));
		font.loadFromFile(PATH_TO_FONT);

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

	void Draw()
	{
		window.clear();

		DrawBoard();
		DrawSide();

		window.display();
	}

	sf::RenderWindow& GetWindow()
	{
		return window;
	}

	void SetChessBoard(const ChessBoard* board)
	{
		this->board = board;
	}

	void SetResultScreen(GameState result)
	{
		resultWindowData = result;
	}
	void ResetResultScreen()
	{
		resultWindowData.reset();
	}

	void FlipBoard() // TODO
	{

	}
};

const Size Graphics::SQUARE_SIZE = Size(106, 106);
const sf::Color Graphics::WHITE_COLOR = sf::Color(238, 238, 210);
const sf::Color Graphics::BLACK_COLOR = sf::Color(118, 150, 86);
const string Graphics::PATH_TO_FONT = "font.ttf";