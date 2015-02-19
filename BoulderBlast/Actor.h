#ifndef ACTOR_H_
#define ACTOR_H_

#include "StudentWorld.h"
#include "GraphObject.h"

//----------Actor Class----------
class Actor: public GraphObject
{
public:
  Actor(int id, int x, int y, StudentWorld* ptr, Direction d = none);
  virtual void doSomething();
  bool isAlive();
  StudentWorld* getWorld();
private:
  bool isDead;
  StudentWorld* world;
};

//----------Character Class----------
class Character: public Actor
{
public:
  Character(int x, int y, int points, int id,
            StudentWorld* ptr, Direction d);
  virtual void moveNMark(int ox, int oy);
private:
  int hitPoint;
};

//----------Wall Class----------
class Wall: public Actor
{
public:
  Wall(int x, int y, StudentWorld* ptr);
private:

};

//----------Player Class----------
class Player: public Character
{
public:
  Player(int x, int y, StudentWorld* ptr);
  virtual void doSomething();
   // Construction!v
  void push(int x, int y);
  void fire();
  void exit();
private:

  int ammu;
};

//----------Boulder Class----------
class Boulder: public Character
{
public:
  Boulder(int x, int y, StudentWorld* ptr);
private:

};



#endif // ACTOR_H_
