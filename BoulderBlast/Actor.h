#ifndef ACTOR_H_
#define ACTOR_H_

#include "StudentWorld.h"
#include "GraphObject.h"

//----------Actor Class----------
class Actor: public GraphObject
{
public:
  Actor(int id, int x, int y, StudentWorld* ptr, Direction d = none);
  virtual void doSomething()=0;
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
  virtual bool moveNMark(int ox, int oy, bool isBoulder= false);
  virtual void attacked(int sound);
  virtual void fire(int sound);
  virtual ~Character();
  void setHealth(int p1);
  int getHealth();
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
  virtual void fire(int sound = SOUND_PLAYER_FIRE);
  virtual void attacked(int blank);
  void pickUp(int x, int y);
  void buff(char c);
  int getAmmo();
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

//----------Collectable Class----------
class Collectable: public Object
{
public:
  Collectable(int x, int y, int id, StudentWorld* ptr);
  virtual void collected();
};


//----------Jewel Classs----------
class Jewel: public Collectable
{
public:
  Jewel(int x, int y, StudentWorld* ptr);
};

//----------Ammo Classs----------
class Ammo: public Collectable
{
public:
  Ammo(int x, int y, StudentWorld* ptr);
};

//----------Extra life Classs----------
class Extra_life: public Collectable
{
public:
  Extra_life(int x, int y, StudentWorld* ptr);
};

//----------Restore health Classs----------
class Restore_health: public Collectable
{
public:
  Restore_health(int x, int y, StudentWorld* ptr);
};

//----------Exit Classs----------
class Exit: public Actor
{
public:
  Exit(int x, int y, StudentWorld* ptr);
  virtual void doSomething();
  bool isRevealed();
 private:
  bool revealed;
};

//----------Robot Classs----------
class Robot: public Character
{
public:
  Robot(int x, int y, StudentWorld* ptr, int id, Direction d, int sr);
  bool shouldMove(int tick);
  bool shouldFire(Player* p);
  virtual void doSomething(int tick); // moves until blocked
  virtual ~Robot();
private:
  int speed_recip;
};

//----------SnarlBot Classs----------
class SnarlBot: public Robot
{
public:
  SnarlBot(int x, int y, StudentWorld* ptr, int sr, Direction d);
  virtual void fire(int sound = SOUND_ENEMY_FIRE);
  virtual void doSomething();
  virtual void attacked(int blank);
  virtual void doSomething(int tick);
};

//---------- Classs----------
class SnarlBot: public Robot
{
public:

}

#endif // ACTOR_H_
