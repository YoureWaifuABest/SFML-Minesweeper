#include "main.h"
#include <SFML/Graphics.hpp>
#include <iostream>

#ifndef FLG_BOMBDETECT
#define FLG_BOMBDETECT
#include "bomb_detection.cpp"
#endif

#define MINES 33 

void mine_distribute(std::vector< std::vector<int> > &grid);
int flood(std::vector< std::vector<int> > &grid, struct point xy);

class tile_map : public sf::Drawable
{
	public:
		bool load(const std::string& tileset, sf::Vector2u tileSize, std::vector< std::vector<int> > grid, unsigned int width, unsigned int height)
		{
			if (!m_tileset.loadFromFile(tileset))
				return false;

			m_vertices.setPrimitiveType(sf::Quads);
			m_vertices.resize(width * height * 4);

			for (unsigned int i = 0; i < width; ++i)
				for (unsigned int j = 0; j < height; ++j) {
					int tileNumber = grid[i][j];

					int tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
					int tv = tileNumber / (m_tileset.getSize().x / tileSize.x);
					
					sf::Vertex* quad = &m_vertices[(i + j * width) * 4];
					
					quad[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
					quad[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
					quad[2].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);
					quad[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);
					
					quad[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
					quad[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
					quad[2].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
					quad[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
				}

			return true;
		}
		
	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			states.texture = &m_tileset;

			target.draw(m_vertices, states);
		}
		
		sf::VertexArray m_vertices;
		sf::Texture m_tileset;
};

void render_grid_graphical(std::vector< std::vector<int> > &grid)
{
	sf::RenderWindow window(sf::VideoMode(512, 256), "Minesweeper");
	bool lost, won, al_sel, distributed;
	struct point xy;
	sf::Font tewi;
	sf::Text lost_text;
	sf::Text won_text;
	sf::Text restart_exit;
	sf::Text restart;
	sf::Text exit;
	sf::Text text;

	if (!tewi.loadFromFile("tewi.bdf"))
		std::cout << "Couldn't find tewi font!" << std::endl;

	/* There's definitely a better way to do this */
	lost_text.setFont(tewi);
	lost_text.setColor(sf::Color::Red);
	lost_text.setString("You lose!");
	lost_text.setCharacterSize(11);
	lost_text.setPosition(0, 201);

	won_text.setFont(tewi);
	won_text.setColor(sf::Color::Green);
	won_text.setString("You win!");
	won_text.setCharacterSize(11);
	won_text.setPosition(0, 201);

	restart_exit.setFont(tewi);
	restart_exit.setColor(sf::Color::White);
	restart_exit.setString("Restart or exit?");	
	restart_exit.setCharacterSize(11);
	restart_exit.setPosition(0, 212);

	restart.setFont(tewi);
	restart.setColor(sf::Color::Green);
	restart.setString("Restart");	
	restart.setCharacterSize(11);
	restart.setPosition(10, 223);

	exit.setFont(tewi);
	exit.setColor(sf::Color::Red);
	exit.setString("Exit");	
	exit.setCharacterSize(11);
	exit.setPosition(60, 223);

	text.setFont(tewi);
	text.setColor(sf::Color::White);
	text.setString("Already Selected!");	
	text.setCharacterSize(11);
	text.setPosition(0, 201);

	tile_map map;

	lost = won = al_sel = distributed = 0;
	if (!map.load("tileset.png", sf::Vector2u(20, 20), grid, 10, 10))
		return;

	window.setVerticalSyncEnabled(true);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
				case sf::Event::Closed:
					window.close();
					break;

				case sf::Event::KeyPressed:
					if (event.key.code == sf::Keyboard::Escape)
						window.close();
					break;

				case sf::Event::MouseButtonReleased:
					if (event.mouseButton.button == sf::Mouse::Left) {
						unsigned int mousex = event.mouseButton.x;
						unsigned int mousey = event.mouseButton.y;
						if (mousex < 200 && mousey < 200) {
							/* There's probably faster way to do this than casting to int */
							xy.y = (unsigned int)(mousex / 20);
							xy.x = (unsigned int)(mousey / 20); 
							if (!distributed) {
								mine_distribute(grid);
								distributed = 1;
								grid[xy.y][xy.x] = 0;
							}
							switch (grid[xy.y][xy.x]) {
								case 0:
									flood(grid, xy);
									al_sel = 0;
									break;
								case 1:
									{ int i, n;
									grid[xy.y][xy.x] = 2;
									for (i = 0; i != GRIDSIZE; ++i)
										for (n = 0; n != GRIDSIZE; ++n)
											if (grid[i][n] == 1)
												grid[i][n] = 2;
									}
									lost = 1;
								default:
									al_sel = 1;
									break;
							}
							if (!map.load("tileset.png", sf::Vector2u(20, 20), grid, 10, 10))
								window.close();
						}
						else if (lost || won) {
							if (mousey > 223 && mousey < 234) {
								if (mousex > 10 && mousex < 55) {
									distributed = 0;
									lost = won = al_sel = 0;
									{ int i, j;
									for (i = 0; i != GRIDSIZE; i++)
										for (j = 0; j != GRIDSIZE; j++)
											grid[i][j] = 0;
									if (!map.load("tileset.png", sf::Vector2u(20, 20), grid, 10, 10))
										window.close();
									}
								}
								else if (mousex > 55 && mousex < 100)
									window.close();
							}
						}
					}
					break;

				default:
					break;
			}
		}
		{ int count, n, i;
		count = n = i = 0;
		for (i = 0; i != GRIDSIZE && grid[i][n] != 0; ++i) 
			for (n = 0; n != GRIDSIZE && grid[i][n] != 0; ++n) 
				++count;
		if (count == GRIDSIZE*GRIDSIZE) {
				won = 1;
		}
		}
		window.clear(sf::Color::Black);
		window.draw(map);
		if (lost)
			window.draw(lost_text);
		if (won)
			window.draw(won_text);
		if (!won && !lost && al_sel)
			window.draw(text);
		if (lost || won) {
			window.draw(restart_exit);
			window.draw(exit);
			window.draw(restart);
		}
		window.display();
	}
}

void mine_distribute(std::vector< std::vector<int> > &grid)
{
	struct point xy;
	int i;

	for (i = 0; i != MINES; i++) {
		xy.y = randombytes_uniform(GRIDSIZE);
		xy.x = randombytes_uniform(GRIDSIZE);

		/* Make sure the correct amount of bombs is placed */
		if (grid[xy.y][xy.x] != 0)
			i--;
		else
			grid[xy.y][xy.x] = 1;
	}
}
