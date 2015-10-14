#ifndef ACTOR_H_
#define ACTOR_H_

#include "StudentWorld.h"
#include "GraphObject.h"

//----------Actor Class----------
class Actor: public GraphObject
{
public:
  Actor(int id, int x, int y, StudentWorld* ptr, Direction d = none);
  virtual void doSomething(); // do nothing
  bool isAlive();
  void setDead();
  StudentWorld* getWorld();
  virtual ~Actor();
private:
  bool isDead;
  StudentWorld* world;
};

//----------Wall Class----------
class Wall: public Actor
{
public:
  Wall(int x, int y, StudentWorld* ptr, int id = IID_WALL);
};

//----------KleptoBot_Factory Classs----------
class KleptoBot_Factory: public Wall
{
public:
  KleptoBot_Factory(int x, int y, StudentWorld* ptr, bool angry = false);
  virtual void doSomething();
  virtual bool shouldCreate();
private:
  bool isAngry;
};

//----------Hole Class----------
class Hole: public Actor
{
public:
  Hole(int x, int y, StudentWorld* ptr);
};

//----------Bullet Class----------
class Bullet: public Actor
{
public:
  Bullet(int x, int y, StudentWorld* ptr, Direction d);
  void damage(int x, int y); // check if Character
  virtual void doSomething(); // check if blocked
};

//----------Collectable Class----------
class Collectable: public Actor
{
public:
  Collectable(int x, int y, int id, StudentWorld* ptr);
  virtual void collected();
  virtual ~Collectable();
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
class Exit: public Collectable
{
public:
  Exit(int x, int y, StudentWorld* ptr);
  virtual void doSomething();
  bool isRevealed();
private:
  bool revealed;
};

//----------Character Class----------
class Character: public Actor
{
public:
  Character(int x, int y, int points, int id,
            StudentWorld* ptr, Direction d = none);
  virtual bool moveNMark(int ox, int oy, bool isBoulder= false);
  // boulder has special bloking rules
  virtual void attacked(int sound);
  virtual void fire(int sound);
  virtual ~Character();
  void setHealth(int p1);
  int getHealth();
private:
  int hitPoint;
};

//----------Player Class----------
class Player: public Character
{
public:
  Player(int x, int y, StudentWorld* ptr);
  virtual void doSomething();
  virtual void fire(int sound = SOUND_PLAYER_FIRE);
  virtual void attacked(int blank);
  void push(int x, int y);
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
};

//----------Robot Classs----------
class Robot: public Character
{
public:
  Robot(int x, int y, StudentWorld* ptr, int id, Direction d);
  bool shouldMove(int tick);
  bool shouldFire(Player* p);
  virtual void attacked(int blank);
  virtual void fire(int sound = SOUND_ENEMY_FIRE);
  virtual ~Robot();
private:
  int speed_recip;
};

//----------SnarlBot Classs----------
class SnarlBot: public Robot
{
public:
  SnarlBot(int x, int y, StudentWorld* ptr, Direction d);
  virtual void doSomething();
  virtual ~SnarlBot();
};

//----------KleptoBot Classs----------
class KleptoBot: public Robot
{
public:
  KleptoBot(int x, int y, StudentWorld* ptr, int id);
  // doesn't set Health
  virtual void doSomething();
  // doesn't time;
  virtual void randDirection();
  virtual bool isEnclosed(int& x, int& y);
  virtual bool pickUp(int x, int y);
  virtual ~KleptoBot();
private:
  int turnUntil;
  Collectable* picked;
};

//----------Normal_KleptoBot Classs----------
class Normal_KleptoBot: public KleptoBot
{
public:
  Normal_KleptoBot(int x, int y, StudentWorld* ptr);
  virtual void doSomething();
  virtual ~Normal_KleptoBot();
};

//----------Angry_KleptoBot Classs----------
class Angry_KleptoBot: public KleptoBot
{
public:
  Angry_KleptoBot(int x, int y, StudentWorld* ptr);
  virtual void doSomething();
  virtual ~Angry_KleptoBot();
};

#endif // ACTOR_H_
