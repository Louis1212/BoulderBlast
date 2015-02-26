#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <list>
#include <vector>
#include <string>
using namespace std;


// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class Player;
class Exit;
class Robot;

class StudentWorld: public GameWorld
{
public:
  StudentWorld(std::string assetDir);
  virtual int init();
  virtual int move();
  virtual void cleanUp();
  virtual void update(int x, int y, Actor* ptrA);
  virtual void deUpdate_character(int x, int y);
  virtual void deUpdate_collectable(int x, int y);
  virtual bool isEmpty(int x, int y);
  virtual bool isWalkable(int x, int y);
  virtual bool isBlocked(int x, int y);
  virtual bool isKleptoBot(int x, int y);
  virtual Actor* getActor(int x, int y);
  virtual Actor* getCollectable(int x, int y);
  virtual void addActor(Actor* a, bool ifUpdate = true);
  virtual bool isComplete();
  virtual Exit* getExit();
  virtual Player* getPlayer();
  bool isPlayer(int x, int y);
  bool isExit(int x, int y);
  ~StudentWorld();
private:
  int bonus;
  Player* p;
  Exit* e;
  list<Actor*> objects;
  list<Robot*> timer;
  vector< vector<Actor*> > map_character;
  vector< vector<Actor*> > map_collectable;
  int tick;
};






#endif // STUDENTWORLD_H_
