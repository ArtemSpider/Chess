#pragma once

#include <iostream>

#include "Board.h"
#include "Pieces.h"
#include "Boards.h"
#include "Graphics.h"
#include "GameIO.h"
#include "TextBoxController.h"



enum class InputState
{
	General,		// Input to show previous moves, save/load the game
	Moves,			// General + Input moves
	Promotion,		// Input what type pawn promotes to
	FilePathSave,	// Input path to .board file to save the game
	FilePathLoad	// Input path to .board file to load the game
};


class Game
{
	ChessBoard* board;
	Graphics graphics;

	InputState inputState;

	void LoadBoard(string from)
	{
		ChessBoard* newBoard;
		try
		{
			newBoard = GameIO::Load(from);
		}
		catch (...)
		{
			cerr << "File not found" << endl;
			return;
		}

		if (board != nullptr)
		{
			delete board;
			board = nullptr;
		}

		board = newBoard;
		graphics.SetChessBoard(board);

		graphics.SetRemainingTimeWhite(&board->remainingTimeWhite);
		graphics.SetRemainingTimeBlack(&board->remainingTimeBlack);
	}
	void SaveBoard(string to)
	{
		try
		{
			GameIO::Save(board, to);
		}
		catch (...)
		{
			cerr << "File not found" << endl;
			return;
		}
	}

	void Move(const Piece* piece, Position to)
	{
		selectedPiece = nullptr;
		board->MovePiece(piece->GetPosition(), to);
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

			if (inputState == InputState::General || inputState == InputState::Moves)
			{
				if (event.type == sf::Event::KeyPressed)
				{
					if (event.key.code == sf::Keyboard::Left)
					{
						board->MoveBackward();
						selectedPiece = nullptr;
					}
					else if (event.key.code == sf::Keyboard::Right)
					{
						board->MoveForward();
						selectedPiece = nullptr;
					}
					else if (event.key.code == sf::Keyboard::S)
					{
						inputState = InputState::FilePathSave;
					}
					else if (event.key.code == sf::Keyboard::L)
					{
						inputState = InputState::FilePathLoad;
					}
				}
			}

			if (inputState == InputState::Moves)
			{
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
			}
			else if (inputState == InputState::Promotion)
			{

			}
			else if (inputState == InputState::FilePathSave || inputState == InputState::FilePathLoad)
			{
				if (graphics.GetTextBox() == nullptr)
					graphics.AddTextBox();
				else
				{
					auto res = TextBoxController::CheckEvent(graphics.GetTextBox(), event);
					if (res == TextBoxController::ActionType::LostFocus)
					{
						inputState = InputState::Moves;
						graphics.RemoveTextBox();
					}
					else if (event.type == sf::Event::KeyPressed)
					{
						if (event.key.code == sf::Keyboard::Enter)
						{
							std::string path = graphics.RemoveTextBox();
							if (!path.empty())
							{
								if (!path.ends_with(".board"))
									path += ".board";
								if (inputState == InputState::FilePathSave)
									SaveBoard(path);
								else
									LoadBoard(path);
								inputState = InputState::Moves;
							}
						}
					}
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

		if (board->GetGameState().state != GameState::State::Game)
			graphics.AddResultBox(board->GetGameState());
		else
			graphics.RemoveResultBox();
	}

	void Draw()
	{
		graphics.Draw();
	}
public:
	Game(TimeControl timeControl) :
		board(CreateBoard(timeControl)),
		graphics(board, &selectedPiece, &board->remainingTimeWhite, &board->remainingTimeBlack),
		inputState(InputState::Moves)
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