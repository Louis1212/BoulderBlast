#ifndef ACTOR_H_
#define ACTOR_H_

#include "StudentWorld.h"
#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

//----------Actor Class----------
class Actor: public GraphObject
{
public:
  Actor(int id, int x, int y, StudentWorld* ptr, Direction d = none);
  virtual void doSomething() = 0;
  bool isAlive();
  StudentWorld* getWorld();
private:
  bool isDead;
  StudentWorld* world;
};

//----------Character Class----------
class Character: public Actor // TOASK -- abstract class or not?
{
public:
  Character(int x, int y, Direction d,
            int points, int id, StudentWorld* ptr);
private:
  int hitPoint;
};

//----------Wall Class----------
class Wall: public Actor
{
public:
  Wall(int x, int y, StudentWorld* ptr);
  virtual void doSomething();
private:
  int id; // TOASK -- why need it?
};

//----------Player Class----------
class Player: public Character
{
public:
  Player(int x, int y, StudentWorld* ptr);
  virtual void doSomething();
private:
  int id;
  int ammu;
};





#endif // ACTOR_H_
