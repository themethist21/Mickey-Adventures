#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "TileMap.h"
#include "Object.h"
#include "TreeEnemy.h"
#include "SpiderEnemy.h"
#include "Player.h"

using namespace std;

enum PlayerCollisions
{
	OBJH, OBJD, ENEMY, NCOLLISIONS
};

TileMap *TileMap::createTileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program, Player* playerPtr)
{
	TileMap *map = new TileMap(levelFile, minCoords, program, playerPtr);

	return map;
}


TileMap::TileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program, Player* playerPtr)
{
	texProgram = &program;
	loadLevel(levelFile, minCoords, program, playerPtr);
	prepareArrays(minCoords, program);
}

TileMap::~TileMap()
{
	if (map != NULL)
		delete map;

}


void TileMap::render() const
{
	if (hasBackmap) {
		glEnable(GL_TEXTURE_2D);
		backTilesheet.use();
		glBindVertexArray(backVao);
		glEnableVertexAttribArray(backPosLocation);
		glEnableVertexAttribArray(backTexCoordLocation);
		glDrawArrays(GL_TRIANGLES, 0, 6 * backNTiles);
		glDisable(GL_TEXTURE_2D);
	}

	glEnable(GL_TEXTURE_2D);
	tilesheet.use();
	glBindVertexArray(vao);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);
	glDrawArrays(GL_TRIANGLES, 0, 6 * nTiles);
	glDisable(GL_TEXTURE_2D);

	for (int i = 0; i < int(objects.size()); ++i) {
		objects[i]->render();
	}
	for (int i = 0; i < int(items.size()); ++i) {
		items[i]->render();
	}
	for (int i = 0; i < int(treeEnemies.size()); ++i) {
		treeEnemies[i]->render();
	}
	for (int i = 0; i < int(spiderEnemies.size()); ++i) {
		spiderEnemies[i]->render();
	}
}

void TileMap::update(int deltaTime)
{
	for (int i = 0; i < int(items.size()); ++i) {
		items[i]->update(deltaTime);
	}
	for (int i = 0; i < int(treeEnemies.size()); ++i) {
		treeEnemies[i]->update(deltaTime);
	}
	for (int i = 0; i < int(spiderEnemies.size()); ++i) {
		spiderEnemies[i]->update(deltaTime);
	}
	for (int i = 0; i < int(objects.size()); ++i) {
		objects[i]->update(deltaTime);
	}

	auto it = objects.cbegin();
	while (it != objects.cend()) {
		if (!(*it)->isActive()) {
			it = objects.erase(it);
		}
		else ++it;
	}

	it = items.cbegin();
	while (it != items.cend()) {
		if (!(*it)->isActive()) {
			it = items.erase(it);
		}
		else ++it;
	}
}

void TileMap::free()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &backVbo);
}

bool TileMap::loadLevel(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program, Player* playerPtr)
{
	ifstream fin;
	string line, tilesheetFile;
	stringstream sstream;
	char tile;

	fin.open(levelFile.c_str());
	if (!fin.is_open())
		return false;
	getline(fin, line);
	if (line.compare(0, 7, "TILEMAP") != 0)
		return false;
	getline(fin, line);
	sstream.str(line);
	sstream >> mapSize.x >> mapSize.y;
	getline(fin, line);
	sstream.str(line);
	sstream >> tileSize >> blockSize;
	getline(fin, line);
	sstream.str(line);
	sstream >> tilesheetFile;
	tilesheet.loadFromFile(tilesheetFile, TEXTURE_PIXEL_FORMAT_RGBA);
	tilesheet.setWrapS(GL_CLAMP_TO_EDGE);
	tilesheet.setWrapT(GL_CLAMP_TO_EDGE);
	tilesheet.setMinFilter(GL_NEAREST);
	tilesheet.setMagFilter(GL_NEAREST);
	getline(fin, line);
	sstream.str(line);
	sstream >> tilesheetSize.x >> tilesheetSize.y;
	tileTexSize = glm::vec2(1.f / tilesheetSize.x, 1.f / tilesheetSize.y);

	map = new int[mapSize.x * mapSize.y];
	for (int j = 0; j<mapSize.y; j++)
	{
		for (int i = 0; i<mapSize.x; i++)
		{
			fin.get(tile);
			if (tile == ' ')
				map[j*mapSize.x + i] = 0;
			else
				map[j*mapSize.x + i] = (tile - int('0'))*10;
			fin.get(tile);
			if (tile != ' ')
				map[j*mapSize.x + i] += tile - int('0');
			fin.get(tile);
		}
#ifndef _WIN32
		fin.get(tile);
#endif
	}
	getline(fin, line);
	if (line.compare(0, 7, "BACKMAP") == 0) {
		hasBackmap = true;
		getline(fin, line);
		sstream.str(line);
		sstream >> tilesheetFile;
		backTilesheet.loadFromFile(tilesheetFile, TEXTURE_PIXEL_FORMAT_RGBA);
		backTilesheet.setWrapS(GL_CLAMP_TO_EDGE);
		backTilesheet.setWrapT(GL_CLAMP_TO_EDGE);
		backTilesheet.setMinFilter(GL_NEAREST);
		backTilesheet.setMagFilter(GL_NEAREST);
		getline(fin, line);
		sstream.str(line);
		sstream >> backTilesheetSize.x >> backTilesheetSize.y;
		backTileTexSize = glm::vec2(1.f / backTilesheetSize.x, 1.f / backTilesheetSize.y);

		backMap = new int[mapSize.x * mapSize.y];
		for (int j = 0; j < mapSize.y; j++)
		{
			for (int i = 0; i < mapSize.x; i++)
			{
				fin.get(tile);
				if (tile == ' ')
					backMap[j*mapSize.x + i] = 0;
				else
					backMap[j*mapSize.x + i] = (tile - int('0')) * 10;
				fin.get(tile);
				if (tile != ' ')
					backMap[j*mapSize.x + i] += tile - int('0');
				fin.get(tile);
			}
#ifndef _WIN32
			fin.get(tile);
#endif
		}
		getline(fin, line);
	}
	if (line.compare(0, 7, "OBJECTS") == 0) {

		getline(fin, line);
		while (line.compare(0, 3, "END") != 0) {
			int objN, objSize;
			glm::ivec2 sheetSize, tilePos;
			string item;
			bool gem = line.compare(0, 3, "Gem") == 0;
			getline(fin, line);
			sstream.str(line);
			sstream >> objN >> objSize;
			getline(fin, line);
			sstream.str(line);
			sstream >> item;
			getline(fin, line);
			sstream.str(line);
			sstream >> tilesheetFile;
			getline(fin, line);
			sstream.str(line);
			sstream >> sheetSize.x >> sheetSize.y;
			getline(fin, line);
			sstream.str(line);
			sstream >> tilePos.x >> tilePos.y;
			for (int i = 0; i < objN; ++i) {
				glm::ivec2 objPos;
				getline(fin, line);
				sstream.str(line);
				sstream >> objPos.x >> objPos.y;
				Object *newObj = new Object();
				newObj->init(glm::vec2(objPos.x * tileSize, objPos.y * tileSize), tilesheetFile, program, objSize, sheetSize, minCoords);
				newObj->setTexPosition(tilePos);
				newObj->setTileMap(this);

				if (item.compare(0, 2, "NO") != 0) {
					if (item.compare(0, 4, "CAKE") == 0) newObj->setContainItem("CAKE");
					else if (item.compare(0, 4, "COIN") == 0) newObj->setContainItem("COIN");
				}

				if (gem) {
					newObj->setIsItem("GEM");
					newObj->setInteractable();
					items.push_back(newObj);
				}
				else {
					newObj->setInteractable();
					objects.push_back(newObj);
				}
			}
			getline(fin, line);
			
		}
	}
	getline(fin, line);
	if (line.compare(0, 7, "ENEMIES") == 0) {
		getline(fin, line);
		while (line.compare(0, 3, "END") != 0) {
			int enemyN;
			string type, orientation;
			sstream.str(line);
			sstream >> type;
			getline(fin, line);
			sstream.str(line);
			sstream >> enemyN;
			for (int i = 0; i < enemyN; ++i) {
				glm::ivec2 enemyPos;
				getline(fin, line);
				sstream.str(line);
				sstream >> enemyPos.x >> enemyPos.y;
				if (type.compare(0, 4, "TREE")== 0) {
					sstream >> orientation;
					TreeEnemy *enemy = new TreeEnemy();
					enemy->init(minCoords, program);
					enemy->setPosition(glm::vec2(enemyPos.x * tileSize, enemyPos.y * tileSize));
					enemy->setTileMap(this);
					enemy->setPlayer(playerPtr);
					enemy->setFacingLeft((orientation.compare(0, 1, "L") == 0));
					treeEnemies.push_back(enemy);
				}
				else if (type.compare(0, 6, "SPIDER") == 0) {
					SpiderEnemy *enemy = new SpiderEnemy();
					enemy->init(minCoords, program);
					enemy->setPosition(glm::vec2(enemyPos.x * tileSize, enemyPos.y * tileSize));
					enemy->setTileMap(this);
					enemy->setPlayer(playerPtr);
					spiderEnemies.push_back(enemy);
				}
			}
			getline(fin, line);
		}
	}
	fin.close();

	return true;
}

void TileMap::prepareArrays(const glm::vec2 &minCoords, ShaderProgram &program)
{
	int tile;
	glm::vec2 posTile, texCoordTile[2], halfTexel;
	vector<float> vertices;
	
	nTiles = 0;
	halfTexel = glm::vec2(0.5f / tilesheet.width(), 0.5f / tilesheet.height());
	for (int j = 0; j<mapSize.y; j++)
	{
		for (int i = 0; i<mapSize.x; i++)
		{
			tile = map[j * mapSize.x + i];
			if (tile != 0)
			{
				// Non-empty tile
				nTiles++;
				posTile = glm::vec2(minCoords.x + i * tileSize, minCoords.y + j * tileSize);
				texCoordTile[0] = glm::vec2(float((tile - 1) % tilesheetSize.x) / tilesheetSize.x, float((tile - 1) / tilesheetSize.x) / tilesheetSize.y);
				texCoordTile[1] = texCoordTile[0] + tileTexSize;
				//texCoordTile[0] += halfTexel;
				texCoordTile[1] -= halfTexel;
				// First triangle
				vertices.push_back(posTile.x); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
				// Second triangle
				vertices.push_back(posTile.x); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
				vertices.push_back(posTile.x); vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[1].y);
			}
		}
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * nTiles * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	posLocation = program.bindVertexAttribute("position", 2, 4 * sizeof(float), 0);
	texCoordLocation = program.bindVertexAttribute("texCoord", 2, 4 * sizeof(float), (void *)(2 * sizeof(float)));

	if (hasBackmap) {
		vertices.clear();
		backNTiles = 0;
		halfTexel = glm::vec2(0.5f / backTilesheet.width(), 0.5f / backTilesheet.height());
		for (int j = 0; j<mapSize.y; j++)
		{
			for (int i = 0; i<mapSize.x; i++)
			{
				tile = backMap[j * mapSize.x + i];
				if (tile != 0)
				{
					// Non-empty tile
					backNTiles++;
					posTile = glm::vec2(minCoords.x + i * tileSize, minCoords.y + j * tileSize);
					texCoordTile[0] = glm::vec2(float((tile - 1) % backTilesheetSize.x) / backTilesheetSize.x, float((tile - 1) / backTilesheetSize.x) / backTilesheetSize.y);
					texCoordTile[1] = texCoordTile[0] + backTileTexSize;
					//texCoordTile[0] += halfTexel;
					texCoordTile[1] -= halfTexel;
					// First triangle
					vertices.push_back(posTile.x); vertices.push_back(posTile.y);
					vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
					vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y);
					vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[0].y);
					vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y + blockSize);
					vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
					// Second triangle
					vertices.push_back(posTile.x); vertices.push_back(posTile.y);
					vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
					vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y + blockSize);
					vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
					vertices.push_back(posTile.x); vertices.push_back(posTile.y + blockSize);
					vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[1].y);
				}
			}
		}

		glGenVertexArrays(1, &backVao);
		glBindVertexArray(backVao);
		glGenBuffers(1, &backVbo);
		glBindBuffer(GL_ARRAY_BUFFER, backVbo);
		glBufferData(GL_ARRAY_BUFFER, 24 * backNTiles * sizeof(float), &vertices[0], GL_STATIC_DRAW);
		backPosLocation = program.bindVertexAttribute("position", 2, 4 * sizeof(float), 0);
		backTexCoordLocation = program.bindVertexAttribute("texCoord", 2, 4 * sizeof(float), (void *)(2 * sizeof(float)));
	}
}

// Collision tests for axis aligned bounding boxes.
// Method collisionMoveDown also corrects Y coordinate if the box is
// already intersecting a tile below.

glm::vec2 TileMap::getSize() const
{
	return glm::vec2(mapSize.x * tileSize, mapSize.y * tileSize);
}

bool TileMap::collisionMoveLeft(const glm::ivec2 &pos, const glm::ivec2 &size, bool *collision, int *posX, Object*& interactedObj) const
{
	int x, y0, y1;

	x = pos.x / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;
	for (int y = y0; y <= y1; y++)
	{
		if (map[y*mapSize.x + x] != 0) {
			*posX = tileSize * x + tileSize - (pos.x - *posX);
			return true;
		}
	}

	x = pos.x;
	y0 = pos.y;
	y1 = (pos.y + size.y - 1);
	for (int i = 0; i < int(objects.size()); ++i) {
		if (objects[i]->canCollide()) {
			glm::ivec2 objPos = objects[i]->getPosition();
			int objSize = objects[i]->getSize();
			if ((x > objPos.x && x <= (objPos.x + objSize)) &&
				(y0 < (objPos.y + objSize) && objPos.y < y1)) {
				*collision = true;
				interactedObj = objects[i];
				*posX = objPos.x + objSize - (pos.x - *posX);
				return true;
			}
		}
	}

	return false;
}

bool TileMap::collisionMoveRight(const glm::ivec2 &pos, const glm::ivec2 &size, bool *collision, int *posX, Object*& interactedObj) const
{
	int x, y0, y1;

	x = (pos.x + size.x - 1) / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;
	for (int y = y0; y <= y1; y++)
	{
		if (map[y*mapSize.x + x] != 0) {
			*posX = tileSize * x - size.x - (pos.x - *posX);
			return true;
		}
	}

	x = (pos.x + size.x - 1);
	y0 = pos.y;
	y1 = (pos.y + size.y - 1);
	for (int i = 0; i < int(objects.size()); ++i) {
		if (objects[i]->canCollide()) {
			glm::ivec2 objPos = objects[i]->getPosition();
			int objSize = objects[i]->getSize();
			if ((x >= objPos.x && x < (objPos.x + objSize)) &&
				(y0 < (objPos.y + objSize) && objPos.y < y1)) {
				*collision = true;
				interactedObj = objects[i];
				*posX = objPos.x - size.x - (pos.x - *posX);
				return true;
			}
		}
	}

	return false;
}

bool TileMap::collisionMoveDown(const glm::ivec2 &pos, const glm::ivec2 &size, int object_height, int *posY, bool *collision, Object*& interactedObj) const
{
	int x0, x1, y, y2;

	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y = (pos.y + size.y - 1) / tileSize;
	for (int x = x0; x <= x1; x++)
	{
		if (map[y*mapSize.x + x] != 0)
		{
			if (*posY - tileSize * y + object_height <= 5)
			{
				*posY = tileSize * y - object_height;
				return true;
			}
		}
	}

	x0 = pos.x;
	x1 = (pos.x + size.x - 1);
	y2 = (pos.y + size.y - 1);
	for (int i = 0; i < int(objects.size()); ++i)
	{
		if (objects[i]->canCollide()) {
			glm::ivec2 objPos = objects[i]->getPosition();
			int objSize = objects[i]->getSize();

			if ((y2 >= objPos.y && y2 < (objPos.y + objSize)) &&
				(x0 < (objPos.x + objSize) && objPos.x < x1))
			{
				if (*posY - tileSize * y + object_height <= 5)
				{
					*posY = tileSize * y - object_height;
					interactedObj = objects[i];
					*collision = true;
					return true;
				}
			}
		}
	}

	return false;
}

bool TileMap::collisionMoveUp(const glm::ivec2 &pos, const glm::ivec2 &size, int object_height, int *posY) const
{
	int x0, x1, y, y2;

	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y = (pos.y) / tileSize;
	for (int x = x0; x <= x1; x++)
	{
		if (map[y*mapSize.x + x] != 0)
		{
			*posY = tileSize * (y+1) - (pos.y - *posY) + 2;
			return true;
		}
	}

	x0 = pos.x;
	x1 = (pos.x + size.x - 1);
	y2 = (pos.y);
	for (int i = 0; i < int(objects.size()); ++i)
	{
		if (objects[i]->canCollide()) {
			glm::ivec2 objPos = objects[i]->getPosition();
			int objSize = objects[i]->getSize();

			if ((y2 >= objPos.y && y2 < (objPos.y + objSize)) &&
				(x0 < (objPos.x + objSize) && objPos.x < x1))
			{
				*posY = objPos.y + objSize + (pos.y - *posY);
				return true;
			}
		}
	}

	return false;
}

bool TileMap::collisionStaticHorizontal(const glm::ivec2 & pos, const glm::ivec2 & size) const
{
	int x0, x1, y0, y1;

	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;
	for (int y = y0; y <= y1; y++)
	{
		if (map[y*mapSize.x + x0] != 0 || map[y*mapSize.x + x1] != 0) {
			return true;
		}
	}

	x0 = pos.x;
	x1 = (pos.x + size.x - 1);
	y0 = pos.y;
	y1 = (pos.y + size.y - 1);
	for (int i = 0; i < int(objects.size()); ++i) {
		if (objects[i]->canCollide()) {
			glm::ivec2 objPos = objects[i]->getPosition();
			int objSize = objects[i]->getSize();
			if ((x0 < (objPos.x + objSize) && objPos.x < x1) &&
				(y0 < (objPos.y + objSize) && objPos.y < y1)) {
				return true;
			}
		}
	}

	return false;
}

bool TileMap::collisionStaticUp(const glm::ivec2 &pos, const glm::ivec2 &size) const
{
	int x0, x1, y, y2;

	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y = pos.y / tileSize;
	for (int x = x0; x <= x1; x++)
	{
		if (map[y*mapSize.x + x] != 0)
		{
			return true;
		}
	}

	x0 = pos.x;
	x1 = (pos.x + size.x - 1);
	y2 = pos.y;
	for (int i = 0; i < int(objects.size()); ++i)
	{
		if (objects[i]->canCollide()) {
			glm::ivec2 objPos = objects[i]->getPosition();
			int objSize = objects[i]->getSize();

			if ((y2 > objPos.y && y2 <= (objPos.y + objSize)) &&
				(x0 < (objPos.x + objSize) && objPos.x < x1))
			{
				return true;
			}
		}
	}

	return false;
}

bool TileMap::collisionStaticDown(const glm::ivec2 & pos, const glm::ivec2 & size) const
{
	int x0, x1, y, y2;

	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y = (pos.y + size.y - 1) / tileSize;
	for (int x = x0; x <= x1; x++)
	{
		if (map[y*mapSize.x + x] != 0)
		{
			return true;
		}
	}

	x0 = pos.x;
	x1 = (pos.x + size.x - 1);
	y2 = (pos.y + size.y - 1);
	for (int i = 0; i < int(objects.size()); ++i)
	{
		if (objects[i]->canCollide()) {
			glm::ivec2 objPos = objects[i]->getPosition();
			int objSize = objects[i]->getSize();

			if ((y2 >= objPos.y && y2 < (objPos.y + objSize)) &&
				(x0 < (objPos.x + objSize) && objPos.x < x1))
			{
				return true;
			}
		}
	}

	return false;
}

bool TileMap::collisionEnemy(const glm::ivec2 & pos, const glm::ivec2 & size)
{
	int x0, x1, y0, y1;

	x0 = pos.x;
	x1 = (pos.x + size.x - 1);
	y0 = pos.y;
	y1 = (pos.y + size.y - 1);
	for (int i = 0; i < int(treeEnemies.size()); ++i) {
		if (treeEnemies[i]->isAttacking()) {
			glm::ivec2 enemyPos = treeEnemies[i]->getPosition();
			glm::ivec2 enemySize = treeEnemies[i]->getSize();
			if ((x0 < (enemyPos.x + enemySize.x) && enemyPos.x < x1) &&
				(y0 < (enemyPos.y + enemySize.y) && enemyPos.y < y1)) {
				return true;
			}
		}
	}
	for (int i = 0; i < int(spiderEnemies.size()); ++i) {
		if (spiderEnemies[i]->isAttacking()) {
			glm::ivec2 enemyPos = spiderEnemies[i]->getPosition();
			glm::ivec2 enemySize = spiderEnemies[i]->getSize();
			if ((x0 < (enemyPos.x + enemySize.x) && enemyPos.x < x1) &&
				(y0 < (enemyPos.y + enemySize.y) && enemyPos.y < y1)) {
				return true;
			}
		}
	}

	return false;
}

bool TileMap::collisionEnemyDamaging(const glm::ivec2 & pos, const glm::ivec2 & size)
{
	int x0, x1, y0, y1;

	x0 = pos.x;
	x1 = (pos.x + size.x - 1);
	y0 = pos.y;
	y1 = (pos.y + size.y - 1);
	bool hit = false;
	for (int i = 0; i < int(treeEnemies.size()); ++i) {
		if (treeEnemies[i]->isAttacking()) {
			glm::ivec2 enemyPos = treeEnemies[i]->getPosition();
			glm::ivec2 enemySize = treeEnemies[i]->getSize();
			if ((x0 < (enemyPos.x + enemySize.x) && enemyPos.x < x1) &&
				(y0 < (enemyPos.y + enemySize.y) && enemyPos.y < y1)) {
				treeEnemies[i]->setDefeat();
				hit = true;
			}
		}
	}

	for (int i = 0; i < int(spiderEnemies.size()); ++i) {
		if (spiderEnemies[i]->isAttacking()) {
			glm::ivec2 enemyPos = spiderEnemies[i]->getPosition();
			glm::ivec2 enemySize = spiderEnemies[i]->getSize();
			if ((x0 < (enemyPos.x + enemySize.x) && enemyPos.x < x1) &&
				(y0 < (enemyPos.y + enemySize.y) && enemyPos.y < y1)) {
				spiderEnemies[i]->setDefeat();
				hit = true;
			}
		}
	}

	return hit;
}


void TileMap::collisionItems(const glm::ivec2 & pos, const glm::ivec2 & size, int * lives, int * score, bool * win)
{
	int x0, x1, y0, y1;

	x0 = pos.x;
	x1 = (pos.x + size.x - 1);
	y0 = pos.y;
	y1 = (pos.y + size.y - 1);
	for (int i = 0; i < int(items.size()); ++i) {
		if (items[i]->isInteractible()) {
			glm::ivec2 itemPos = items[i]->getPosition();
			int itemSize = items[i]->getSize();
			if ((x0 < (itemPos.x + itemSize) && itemPos.x < x1) &&
				(y0 < (itemPos.y + itemSize) && itemPos.y < y1)) {
				string itemType = items[i]->itemType();
				if (itemType.compare(0, 4, "CAKE") == 0 && *lives < 3) *lives = *lives + 1;
				else if (itemType.compare(0, 4, "COIN") == 0) *score += 1000;
				else if (itemType.compare(0, 3, "GEM") == 0) *win = true;
				items[i]->setDestroy();
			}
		}
	}
}

void TileMap::createItem(const glm::ivec2 & pos, const string & type, int itemSize, const glm::vec2 &spritesheetSize, const glm::vec2 &spritesheetDispl)
{
	
	Object *newObj = new Object();
	newObj->init(pos, "images/ObjectSprites.png", *texProgram, itemSize, spritesheetSize, spritesheetDispl);
	
	if (type.compare(0, 4, "CAKE") == 0) newObj->setTexPosition(glm::ivec2(0, 1));
	else if (type.compare(0, 3, "GEM") == 0) newObj->setTexPosition(glm::ivec2(1));
	else if (type.compare(0, 4, "COIN") == 0) newObj->setTexPosition(glm::ivec2(2, 0));

	newObj->setTileMap(this);
	newObj->setIsItem(type);
	if (type.compare(0, 3, "GEM") != 0) newObj->throwObject(glm::vec2(0.f));
	items.push_back(newObj); 
}

void TileMap::resetEnemies()
{
	for (int i = 0; i < int(treeEnemies.size()); ++i) {
		treeEnemies[i]->reset();
	}
	for (int i = 0; i < int(spiderEnemies.size()); ++i) {
		spiderEnemies[i]->reset();
	}
}

