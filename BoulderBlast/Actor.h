#ifndef ACTOR_H_
#define ACTOR_H_

#include "StudentWorld.h"
#include "GraphObject.h"

//----------Actor Class----------
class Actor: public GraphObject
{
public:
  Actor(int id, int x, int y, StudentWorld* ptr, Direction d = none);
  virtual void doSomething() = 0;
  bool isAlive();
  void setDead();
  StudentWorld* getWorld();
  virtual ~Actor();
private:
  bool isDead;
  StudentWorld* world;
};

//----------Object Class----------
class Object: public Actor
{
public:
  Object(int x, int y, int id, StudentWorld* ptr);
  virtual void doSomething();
  virtual ~Object();
};

//----------Character Class----------
class Character: public Actor
{
public:
  Character(int x, int y, int points, int id,
            StudentWorld* ptr, Direction d);
  virtual void moveNMark(int ox, int oy);
  virtual void attacked();
  virtual ~Character();
private:
  int hitPoint;
};

//----------Wall Class----------
class Wall: public Object
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
  void push(int x, int y);
  void fire();
  //construction:
  void exit();
private:

  int ammu;
};

//----------Boulder Class----------
class Boulder: public Character
{
public:
  Boulder(int x, int y, StudentWorld* ptr);
  bool fill(int x, int y);
  virtual void doSomething();
private:

};

//----------Hole Class----------
class Hole: public Object
{
public:
  Hole(int x, int y, StudentWorld* ptr);
};

//----------Bullet Class----------
class Bullet: public Actor
{
public:
  Bullet(int x, int y, StudentWorld* ptr, Direction d);
  void damage(int x, int y);
  virtual void doSomething();
};

#endif // ACTOR_H_
