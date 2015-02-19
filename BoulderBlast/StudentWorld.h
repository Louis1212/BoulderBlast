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

class StudentWorld: public GameWorld
{
public:
  StudentWorld(std::string assetDir);
  virtual int init();
  virtual int move();
  virtual void cleanUp();
  virtual void update(int x, int y, Actor* ptrA);
  virtual void deUpdate(int x, int y);
  virtual bool isEmpty(int x, int y);
  Actor* getActor(int x, int y);
  ~StudentWorld();
private:
  Player* p;
  list<Actor*> objects;
  vector< vector<Actor*> > map;
};






#endif // STUDENTWORLD_H_
