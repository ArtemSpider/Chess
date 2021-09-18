#pragma once

#include <iostream>

#include "Board.h"
#include "Pieces.h"
#include "Boards.h"
#include "Graphics.h"
#include "GameIO.h"


class Game
{
	ChessBoard* board;
	Graphics graphics;

	void LoadBoard(string from)
	{
		if (board != nullptr)
		{
			delete board;
			board = nullptr;
		}

		board = GameIO::Load(from);
		graphics.SetChessBoard(board);
	}
	void SaveBoard(string to)
	{
		GameIO::Save(board, to);
	}

	void Move(const Piece* piece, Position to)
	{
		selectedPiece = nullptr;
		board->MovePiece(piece->GetPosition(), to);

		if (board->GetGameState().state != GameState::State::Game)
			graphics.SetResultScreen(board->GetGameState());
	}

	const Piece* selectedPiece;
	void Input()
	{
		sf::Event event;
		while (graphics.GetWindow().pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				graphics.GetWindow().close();
				break;
			}

			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Left)
			{
				Position pos = Position(event.mouseButton.x, event.mouseButton.y) / Graphics::SQUARE_SIZE;
				pos = Position(pos.x, board->SIZE.y - 1 - pos.y);

				if (selectedPiece != nullptr && pos != selectedPiece->GetPosition())
				{
					const auto& possibleMoves = board->GetLegalMoves(selectedPiece);
					if (find(possibleMoves.begin(), possibleMoves.end(), pos) != possibleMoves.end())
						Move(selectedPiece, pos);
					selectedPiece = nullptr;
				}

				if (board->InBounds(pos) && !board->IsEmpty(pos) && board->GetTeam(pos) == board->GetTurn())
					selectedPiece = board->GetPieceAt(pos);
			}

			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::S)
					SaveBoard("save.board");
				else if (event.key.code == sf::Keyboard::L)
					LoadBoard("save.board");
				else if (event.key.code == sf::Keyboard::Left)
				{
					board->MoveBackward();
					selectedPiece = nullptr;
				}
				else if (event.key.code == sf::Keyboard::Right)
				{
					board->MoveForward();
					selectedPiece = nullptr;
				}
			}
		}
	}

	sf::Clock moveClock;
	void Update()
	{
		static int remMillis = 0;
		static PlayerTeam lastTurn = PlayerTeam::White;

		if (board->GetTurn() != lastTurn)
		{
			remMillis = 0;
			lastTurn = board->GetTurn();
		}

		while (moveClock.getElapsedTime().asMilliseconds() + remMillis >= 1000)
		{
			remMillis = (moveClock.getElapsedTime().asMilliseconds() + remMillis) - 1000;
			board->ChangeRemainingTime(-1);

			moveClock.restart();
		}
	}

	void Draw()
	{
		graphics.Draw();
	}
public:
	Game(TimeControl timeControl) :
		board(CreateBoard(timeControl)),
		graphics(board, &selectedPiece, &board->remainingTimeWhite, &board->remainingTimeBlack)
	{}

	bool Step()
	{
		Input();
		if (!graphics.GetWindow().isOpen()) return false;

		Update();
		Draw();

		return true;
	}

	void Run()
	{
		while (Step());
	}
};