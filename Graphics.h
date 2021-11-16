#pragma once

#include <optional>

#include "DrawableArray.h"
#include "Board.h"
#include "ResultBox.h"
#include "TextBox.h"
#include "Button.h"

#include <SFML/Graphics.hpp>


class Graphics
{
	sf::Texture piecesTexture;

	sf::RenderWindow window;

	vector<sf::Sprite> whitePieces;		// array of pieces sprites in the order of PieceType
	vector<sf::Sprite> blackPieces;

	const ChessBoard* board;
	const Piece** selectedPiecePtr;		// pointer to the selected piece

	const int* remainingTimeWhite;		// displayed in seconds
	const int* remainingTimeBlack;		// displayed in seconds

	/*
	* buttons for choosing what type piece promotes to
	* first are white pieces in order of PieceType(except Pawn and King),
	* then black pieces in the same order
	*/
	vector<Button*> promoteToButtons;	

	TextBox* textBox;					// displayed TextBox
	ResultBox* resultBox;				// displayed result screen

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
	}


	string TimeToString(int time)
	{
		if (time / 3600 >= 1)	// mote then an hour
			return to_string(time / 3600) + ":" + to_string(time % 3600 / 60) + ":" + to_string(time % 60);
		return to_string(time % 3600 / 60) + ":" + (time % 60 < 10 ? "0" : "") + to_string(time % 60);
	}

	void DrawSide()
	{
		// drawing remaining time
		if (!board->WithoutTime())
		{
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
		
		for (int i = 0; i < promoteToButtons.size() / 2; i++)
		{
			promoteToButtons[i]->setPosition(Point<float>(
				SQUARE_SIZE.x * board->SIZE.x + SQUARE_SIZE.y / 4.f + i * 40.f,
				SQUARE_SIZE.y));
		}
		for (int i = promoteToButtons.size() / 2; i < promoteToButtons.size(); i++)
		{
			promoteToButtons[i]->setPosition(Point<float>(
				SQUARE_SIZE.x * board->SIZE.x + SQUARE_SIZE.y / 4.f + (i - promoteToButtons.size() / 2) * 40.f,
				SQUARE_SIZE.y * (board->SIZE.y - 1)));
		}
		for (auto& b : promoteToButtons)
			window.draw(*b);
	}


	void DrawMenus()
	{
		if (resultBox != nullptr)
			window.draw(*resultBox);
		if (textBox != nullptr)
			window.draw(*textBox);
	}
public:
	static const Size SQUARE_SIZE;

	static const sf::Color WHITE_COLOR;
	static const sf::Color BLACK_COLOR;

	static const string PATH_TO_FONT;


	Graphics(const ChessBoard* board, const Piece** selectedPiece, const int* remainingTimeWhite, const int* remainingTimeBlack) :
		window(sf::VideoMode(Graphics::SQUARE_SIZE.x * board->SIZE.x + 200, Graphics::SQUARE_SIZE.y * board->SIZE.y), "Chess", sf::Style::Titlebar | sf::Style::Close),
		board(board), selectedPiecePtr(selectedPiece), font(),
		remainingTimeWhite(remainingTimeWhite), remainingTimeBlack(remainingTimeBlack),
		textBox(nullptr), resultBox(nullptr)
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


		// buttons for choosing promotion type
		for (int i = (int)PieceType::Knight; i <= (int)PieceType::Queen; i++)
		{
			sf::RectangleShape* rect1 = new sf::RectangleShape(sf::Vector2f(40.f, 40.f));
			sf::Sprite* spr1 = new sf::Sprite(blackPieces[i]);
			spr1->setScale(0.3f, 0.3f);
			spr1->setPosition(20.f, 20.f);

			sf::RectangleShape* rect2 = new sf::RectangleShape(sf::Vector2f(40.f, 40.f));
			rect2->setFillColor(sf::Color::Blue);
			sf::Sprite* spr2 = new sf::Sprite(blackPieces[i]);
			spr2->setScale(0.3f, 0.3f);
			spr2->setPosition(20.f, 20.f);

			promoteToButtons.push_back(new Button(new DrawableArray({ rect1, spr1 }), new DrawableArray({ rect2, spr2 }), sf::Vector2f(40.f, 40.f)));
			promoteToButtons.back()->canBeUnpressed = false;
		}
		for (int i = (int)PieceType::Knight; i <= (int)PieceType::Queen; i++)
		{
			sf::RectangleShape* rect1 = new sf::RectangleShape(sf::Vector2f(40.f, 40.f));
			sf::Sprite* spr1 = new sf::Sprite(whitePieces[i]);
			spr1->setScale(0.3f, 0.3f);
			spr1->setPosition(20.f, 20.f);

			sf::RectangleShape* rect2 = new sf::RectangleShape(sf::Vector2f(40.f, 40.f));
			rect2->setFillColor(sf::Color::Blue);
			sf::Sprite* spr2 = new sf::Sprite(whitePieces[i]);
			spr2->setScale(0.3f, 0.3f);
			spr2->setPosition(20.f, 20.f);

			promoteToButtons.push_back(new Button(new DrawableArray({ rect1, spr1 }), new DrawableArray({ rect2, spr2 }), sf::Vector2f(40.f, 40.f)));
			promoteToButtons.back()->setOrigin(0.f, 40.f);
			promoteToButtons.back()->canBeUnpressed = false;
		}
		promoteToButtons[promoteToButtons.size() / 2 - 1]->press();
		promoteToButtons.back()->press();

		for (int i = 1; i < promoteToButtons.size() / 2; i++)
			promoteToButtons[i]->connect(promoteToButtons[0]),
			promoteToButtons[i + promoteToButtons.size() / 2]->connect(promoteToButtons[promoteToButtons.size() / 2]);
	}

	void Draw()
	{
		window.clear();

		DrawBoard();
		DrawSide();
		DrawMenus();

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

	void SetRemainingTimeWhite(const int* time)
	{
		remainingTimeWhite = time;
	}
	void SetRemainingTimeBlack(const int* time)
	{
		remainingTimeBlack = time;
	}

	void AddResultBox(GameState result)
	{
		if (resultBox == nullptr)
		{
			resultBox = new ResultBox(font, PositionF(SQUARE_SIZE * board->SIZE / 2), SizeF(3.0f, 1.5f) * SQUARE_SIZE, result);
			resultBox->SetResultTextPadding(PositionF(0.f, -SQUARE_SIZE.y * 0.2f));
			resultBox->SetReasonTextPadding(PositionF(0.f, +SQUARE_SIZE.y * 0.3f));
			resultBox->SetResultCharacterSize(40);
			resultBox->SetReasonCharacterSize(25);
		}
		else
			resultBox->SetState(result);
	}
	ResultBox* GetResultBox()
	{
		return resultBox;
	}
	void RemoveResultBox()
	{
		delete resultBox;
		resultBox = nullptr;
	}

	void AddTextBox()
	{
		TextBox* tb = new TextBox(GetFont());
		tb->setPadding({ 6.f, 0.f });
		tb->setMaxStringSize(16);
		tb->autoSize();
		tb->setOrigin(tb->getTextBoxSize() / 2.f);
		tb->setPosition({ 8.f * Graphics::SQUARE_SIZE.x / 2.f, 8.f * Graphics::SQUARE_SIZE.y / 2.f });
		tb->setSelected(true);

		if (textBox != nullptr)
			delete textBox;
		textBox = tb;
	}
	TextBox* GetTextBox()
	{
		return textBox;
	}
	sf::String RemoveTextBox()
	{
		sf::String res = textBox->getString();
		delete textBox;
		textBox = nullptr;
		return res;
	}

	/*
	* buttons for choosing what type piece promotes to
	* first are white pieces in order of PieceType(except Pawn and King),
	* then black pieces in the same order
	*/
	vector<Button*> GetPromotesToButtons() const
	{
		return promoteToButtons;
	}

	sf::Font& GetFont()
	{
		return font;
	}

	void FlipBoard() // TODO
	{

	}
};

const Size Graphics::SQUARE_SIZE = Size(106, 106);
const sf::Color Graphics::WHITE_COLOR = sf::Color(238, 238, 210);
const sf::Color Graphics::BLACK_COLOR = sf::Color(118, 150, 86);
const string Graphics::PATH_TO_FONT = "font.ttf";