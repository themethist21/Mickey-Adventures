#include <iostream>
#include "Object.h"
#include "TileMap.h"


#define THROW_ANGLE_STEP 4
#define THROW_STEP 4
#define INIT_THROW_ANGLE 70
#define INIT_ITEM_ANGLE 30
#define DESTROY_TIME 0.3f

enum TileAnims
{
	BASE, DESTROY_TW, DESTROY_IT, NUM_ANIMS
};

void Object::init(const glm::vec2 &pos, const string &filename, ShaderProgram &shaderProgram, int tileSize, const glm::vec2 &spritesheetSize, const glm::vec2 &spritesheetDispl)
{
	posObj = pos;
	objSize = tileSize;
	spriteSheetSize = spritesheetSize;
	spriteDispl = spritesheetDispl;

	spritesheet.loadFromFile(filename, TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(tileSize), glm::vec2(float(tileSize) / (spritesheetSize.x*tileSize), float(tileSize) / (spritesheetSize.y*tileSize)), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(NUM_ANIMS); // BASE y DESTROY

	sprite->setAnimationSpeed(DESTROY_TW, 5);
	sprite->addKeyframe(DESTROY_TW, glm::vec2(0.f / (spritesheetSize.x*tileSize), 32.f/ (spritesheetSize.y*tileSize)));
	sprite->addKeyframe(DESTROY_TW, glm::vec2(16.f / (spritesheetSize.x*tileSize), 32.f / (spritesheetSize.y*tileSize)));
	
	sprite->setAnimationSpeed(DESTROY_IT, 5);
	sprite->addKeyframe(DESTROY_IT, glm::vec2(0.f / (spritesheetSize.x*tileSize), 48.f / (spritesheetSize.y*tileSize)));
	sprite->addKeyframe(DESTROY_IT, glm::vec2(16.f / (spritesheetSize.x*tileSize), 48.f / (spritesheetSize.y*tileSize)));
	


	sprite->setPosition(glm::ivec2(posObj.x + spriteDispl.x, posObj.y + spriteDispl.y));
}


void Object::update(int deltaTime)
{
	destroyTimer -= deltaTime / 1000.f;

	sprite->update(deltaTime);
	if (objState != ObjectStates::DESTROY) {
		switch (objType) {
		case ObjectType::TROWABLE: updateTrowable(deltaTime); break;
		case ObjectType::ITEM: updateItem(deltaTime); break;
		}
	}
	else updateDestroy(deltaTime);
}

void Object::render() const
{
	sprite->render();
}

void Object::updateTrowable(int deltaTime)
{
	if (objState == ObjectStates::MOVING) {
		bool isDestroyed = false;

		posObj = glm::ivec2(posObj.x + velObj.x, posObj.y);

		Object *collidedObj = nullptr;
		if ((velObj.x < 0) && map->collisionStaticHorizontal(posObj, glm::ivec2(objSize))) {
			isDestroyed = true;
			setDestroy();
		}

		if (bThrow) {
			throwAngle += THROW_ANGLE_STEP;
			if (throwAngle == 180) {
				bThrow = false;
			}
			else {
				posObj.y = int(startY - 96 * sin(3.14159f * throwAngle / 180.0f));
			}
		}
		else posObj.y += THROW_STEP;

		if (!isDestroyed && map->collisionStaticDown(posObj, glm::ivec2(objSize))) {
			setDestroy();
		}

		if (!isDestroyed && map->collisionEnemyDamaging(posObj, glm::ivec2(objSize))) {
			setDestroy();
		}

		sprite->setPosition(glm::ivec2(posObj.x + spriteDispl.x, posObj.y + spriteDispl.y));
	}
}

void Object::updateItem(int deltaTime)
{
	if (objState == ObjectStates::MOVING) {

		if (bThrow) {
			throwAngle += THROW_ANGLE_STEP;
			if (throwAngle == 180 - INIT_ITEM_ANGLE) {
				bThrow = false;
			}
			else {
				posObj.y = int(startY - 96 * sin(3.14159f * throwAngle / 180.0f));
			}
		}
		else {
			posObj.y += THROW_STEP;
			if (map->collisionStaticDown(posObj, glm::ivec2(objSize))) setInteractable();
		}
		sprite->setPosition(glm::ivec2(posObj.x + spriteDispl.x, posObj.y + spriteDispl.y));
	}
}

void Object::updateDestroy(int deltaTime) {
	if (sprite->animation() == BASE) {
		if (objType == ObjectType::TROWABLE) sprite->changeAnimation(DESTROY_TW);
		else if (objType == ObjectType::ITEM) sprite->changeAnimation(DESTROY_IT);
	}
	if (destroyTimer < 0) {
		if (bHasItem) map->createItem(glm::vec2(posObj.x, posObj.y), item, objSize, spriteSheetSize, spriteDispl);
		objState = ObjectStates::INACTIVE;
	}
}

void Object::setTileMap(TileMap * tileMap)
{
	map = tileMap;
}

void Object::setTexPosition(const glm::vec2 & texturePos)
{
	sprite->setAnimationSpeed(BASE, 0);
	sprite->addKeyframe(BASE, glm::vec2(texturePos.x / spriteSheetSize.x, texturePos.y / spriteSheetSize.y));
	sprite->changeAnimation(BASE);
}

void Object::setPos(const glm::vec2 &pos) {
	posObj = pos;
	sprite->setPosition(glm::ivec2(posObj.x + spriteDispl.x, posObj.y + spriteDispl.y));
}

void Object::setVelocity(const glm::vec2 & v)
{
	velObj = v;
}

glm::ivec2 Object::getPosition() const {
	return posObj;
}

int Object::getSize() const {
	return objSize;
}

bool Object::canBeMoved(int yPos) const
{
	if (isInteractible() && yPos != (posObj.y + objSize)) {

		return false;
	}
	return !map->collisionStaticUp(glm::vec2(posObj.x, posObj.y - 1), glm::ivec2(objSize));
}

void Object::throwObject(const glm::vec2 & v)
{
	setMoving();
	bThrow = true;
	setVelocity(v);
	if (objType == ObjectType::ITEM) throwAngle = INIT_ITEM_ANGLE;
	else throwAngle = INIT_THROW_ANGLE;
	startY = posObj.y + int(96 * sin(3.14159f * throwAngle / 180.0f));
}

bool Object::isActive() const
{
	return objState != ObjectStates::INACTIVE;
}

bool Object::isInteractible() const
{
	return objState == ObjectStates::INTERACTABLE;
}

bool Object::canCollide() const
{
	return objState == ObjectStates::INTERACTABLE || objState == ObjectStates::STATIC;
}

bool Object::hasItem()
{
	return bHasItem;
}

string Object::itemType()
{
	return item;
}

void Object::setInteractable()
{
	objState = ObjectStates::INTERACTABLE;
}

void Object::setHeld()
{
	objState = ObjectStates::HELD;
}

void Object::setMoving()
{
	objState = ObjectStates::MOVING;
}

void Object::setDestroy()
{
	objState = ObjectStates::DESTROY;
	destroyTimer = DESTROY_TIME;
}

void Object::setContainItem(const string & i)
{
	bHasItem = true;
	item = i;
}

void Object::setIsItem(const string &type)
{
	objType = ObjectType::ITEM;
	item = type;
}

