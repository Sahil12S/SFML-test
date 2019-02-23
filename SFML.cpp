#include "pch.h"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>

using namespace std;

class TileMap : public sf::Drawable, public sf::Transformable
{
public:

	bool load(const std::string& tileset, sf::Vector2u tileSize, const vector< pair<int, int>>* tiles, unsigned int width, unsigned int height)
	{
		// load the tileset texture
		if (!m_tileset.loadFromFile(tileset))
			return false;

		// resize the vertex array to fit the level size
		m_vertices.setPrimitiveType(sf::Quads);
		m_vertices.resize(width * height * 4);

		// populate the vertex array, with one quad per tile
		for (unsigned int i = 0; i < width; ++i)
			for (unsigned int j = 0; j < height; ++j)
			{

				// get the current tile number
				pair<int, int> tileNumber = tiles[0][i + j * width];

				// find its position in the tileset texture
				int tu = tileNumber.first % (m_tileset.getSize().x / tileSize.x);
				int tv = tileNumber.second % (m_tileset.getSize().x / tileSize.x);

				// get a pointer to the current tile's quad
				sf::Vertex* quad = &m_vertices[(i + j * width) * 4];

				// define its 4 corners
				quad[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
				quad[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
				quad[2].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);
				quad[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);

				// define its 4 texture coordinates
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
		// apply the transform
		states.transform *= getTransform();

		// apply the tileset texture
		states.texture = &m_tileset;

		// draw the vertex array
		target.draw(m_vertices, states);
	}

	sf::VertexArray m_vertices;
	sf::Texture m_tileset;
};
// And now, the application that uses it :
void readFile(string fileName, vector< pair<int, int>>* level, int& height, int& width);

int main()
{
		const sf::Vector2f PREF_RESOLUTION = { 1280.0, 720.0 };

		sf::Vector2f resolution;
		resolution.x = sf::VideoMode::getDesktopMode().width;
		resolution.y = sf::VideoMode::getDesktopMode().height;
	// create the window
	sf::RenderWindow window(sf::VideoMode(resolution.x, resolution.y), "Tilemap", sf::Style::Fullscreen);

	// define the level with an array of tile indices
	/*const int level[] =
	{
		0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0,
		1, 1, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3,
		0, 1, 0, 0, 2, 0, 3, 3, 3, 0, 1, 1, 1, 0, 0, 0,
		0, 1, 1, 0, 3, 3, 3, 0, 0, 0, 1, 1, 1, 2, 0, 0,
		0, 0, 1, 0, 3, 0, 2, 2, 0, 0, 1, 1, 1, 1, 2, 0,
		2, 0, 1, 0, 3, 0, 2, 2, 2, 0, 1, 1, 1, 1, 1, 1,
		0, 0, 1, 0, 3, 2, 2, 2, 0, 0, 0, 0, 1, 1, 1, 1,
	};*/

	int height, width;
	vector< pair<int, int>> level;
	readFile("level.txt", &level, height, width);

	//cout << level.size() << endl;

	//for (pair<int, int> coords : level)
	//{
	//	cout << coords.first << "," << coords.second << endl;
	//}

	// create the tilemap from the level definition
	TileMap map;
	if (!map.load("mariotileset.png", sf::Vector2u(16, 16), &level, width, height))
		return -1;

	// run the main loop
	while (window.isOpen())
	{
		// handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// draw the map
		window.clear();


		//cout << resolution.x << ", " << resolution.y << endl;

		map.setScale((resolution.x / PREF_RESOLUTION.x), (resolution.y / PREF_RESOLUTION.y));
		//map.setScale(1.5,1.5);

		window.draw(map);
		window.display();
	}

	return 0;
}

void readFile(string fileName, vector< pair<int, int>>* level, int& height, int& width)
{
	ifstream inputfile(fileName);
	string s;

	height = 0;

	string num = "";
	while (getline(inputfile, s))
	{
		width = 0;
		string str = "";
		for (int j = 0; j <= s.length(); j++)
		{
			if (j != s.length() && s[j] != '\t')
			{
				str += s[j];
			}
			else
			{
				num = "";
				pair<int, int> coords;
				for (int i = 1; i < str.length() - 1; i++)
				{
					if (str[i] != ',')
					{
						num += str[i];
					}
					else
					{
						coords.first = stoi(num);
						num = "";
					}
				}
				coords.second = stoi(num);
				level->emplace_back(coords);
				++width;
				str = "";
			}
		}
		++height;
	}
}
