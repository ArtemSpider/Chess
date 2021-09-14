#pragma once

#include <iostream>

#include "Board.h"
#include "Pieces.h"
#include "Graphics.h"


class Game
{
	static ChessBoard* CreateBoard()
	{
		ChessBoard* res = new ChessBoard(Size(8, 8));

		vector<vector<Piece*> > grid(8, vector<Piece*>(8, nullptr));
		for (int j = 0; j < 8; j++)
		{
			grid[1][j] = new Pawn({ j, 1 }, PlayerTeam::White, res);
			grid[6][j] = new Pawn({ j, 6 }, PlayerTeam::Black, res);
		}

		grid[0][0] = new Rook({ 0, 0 }, PlayerTeam::White, res);
		grid[7][0] = new Rook({ 0, 7 }, PlayerTeam::Black, res);

		grid[0][1] = new Knight({ 1, 0 }, PlayerTeam::White, res);
		grid[7][1] = new Knight({ 1, 7 }, PlayerTeam::Black, res);

		grid[0][2] = new Bishop({ 2, 0 }, PlayerTeam::White, res);
		grid[7][2] = new Bishop({ 2, 7 }, PlayerTeam::Black, res);

		grid[0][3] = new Queen({ 3, 0 }, PlayerTeam::White, res);
		grid[7][3] = new Queen({ 3, 7 }, PlayerTeam::Black, res);

		grid[0][4] = new King({ 4, 0 }, PlayerTeam::White, res);
		grid[7][4] = new King({ 4, 7 }, PlayerTeam::Black, res);

		grid[0][5] = new Bishop({ 5, 0 }, PlayerTeam::White, res);
		grid[7][5] = new Bishop({ 5, 7 }, PlayerTeam::Black, res);

		grid[0][6] = new Knight({ 6, 0 }, PlayerTeam::White, res);
		grid[7][6] = new Knight({ 6, 7 }, PlayerTeam::Black, res);

		grid[0][7] = new Rook({ 7, 0 }, PlayerTeam::White, res);
		grid[7][7] = new Rook({ 7, 7 }, PlayerTeam::Black, res);

		res->InitGrid(grid);
		return res;
	}

	ChessBoard* board;
	Graphics graphics;

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
					auto possibleMoves = board->GetLegalMoves(selectedPiece);
					if (find(possibleMoves.begin(), possibleMoves.end(), pos) != possibleMoves.end())
						Move(selectedPiece, pos);
					selectedPiece = nullptr;
				}

				if (board->InBounds(pos) && !board->IsEmpty(pos))
				{
					const Piece* p = board->GetPieceAt(pos);

					if (p->GetTeam() == board->GetTurn())
						selectedPiece = p;
				}
			}
		}
	}

	void Draw()
	{
		graphics.Draw();
	}
public:
	Game() : board(CreateBoard()), graphics(board, &selectedPiece) {}

	bool Step()
	{
		Input();
		if (!graphics.GetWindow().isOpen()) return false;

		Draw();

		return true;
	}

	void Run()
	{
		while (Step());
	}
};