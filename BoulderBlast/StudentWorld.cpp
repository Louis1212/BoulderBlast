#include "StudentWorld.h"
#include "Actor.h"
#include "Level.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <list>

using namespace std;


// unsigned int getLevel() const;
// unsigned int getLives() const;
// void decLives();
// void incLives();
// unsigned int getScore() const;
// void increaseScore(unsigned int howMuch);
// void setGameStatText(string text);
// bool getKey(int& value);
// void playSound(int soundID);


StudentWorld::StudentWorld(std::string assetDir)
  :GameWorld(assetDir), p(nullptr), e(nullptr), tick(0)
{
  map.resize(15);
  for(int i = 0; i < 15; i++)
    map[i].resize(15);
  for(int i = 0; i < 15; i++)
    for(int j = 0; j < 15; j++)
      map[i][j] = nullptr;
}

StudentWorld::~StudentWorld()
{
  list<Actor*>::iterator o = objects.begin();
  while(o != objects.end()){
    delete *o;
    o = objects.erase(o);
  }
  list<Robot*>::iterator t = timer.begin();
  while(t != timer.end()){
    delete *t;
    t = timer.erase(t);
  }
  delete p;
  p = nullptr;
  delete e;
  e = nullptr;
}

void StudentWorld::update(int x, int y, Actor* ptrA) // update on the map it's location.
{
  map[x][y] = ptrA;
}

void StudentWorld::deUpdate(int x, int y)
{
  map[x][y] = nullptr;
}

bool StudentWorld::isEmpty(int x, int y)
{
  if(x >= 15 || y >= 15 || x < 0 || y < 0)
    return false;
  return (map[x][y] == nullptr);
}

bool StudentWorld::isWalkable(int x, int y)
{
  if(x >= 15 || y >= 15 || x < 0 || y < 0)
    return false;
  if(isBlocked(x, y))
    return false;
  Hole* h = dynamic_cast<Hole*>(map[x][y]);
  return (h == nullptr);
}

bool StudentWorld::isBlocked(int x, int y)
{
  if(x >= 15 || y >= 15 || x < 0 || y < 0)
    return true;
  Character* c = dynamic_cast<Character*>(map[x][y]);
  Wall* w  = dynamic_cast<Wall*>(map[x][y]);

  return (c != nullptr || w != nullptr);
}

bool StudentWorld::isKleptoBot(int x, int y)
{
  KleptoBot* k = dynamic_cast<KleptoBot*>(map[x][y]);
  return (k != nullptr);
}

Actor* StudentWorld::getActor(int x, int y)
{
  return map[x][y];
}

void StudentWorld::addActor(Actor* a, bool ifUpdate)
{
  if(a == nullptr)
    return;
  Robot* r = dynamic_cast<Robot*>(a);
  if(r != nullptr)
    timer.push_back(r);
  else
    objects.push_back(a);
  if(ifUpdate)
    update(a->getX(), a->getY(), a);
}

bool StudentWorld::isPlayer(int x, int y)
{
  return(x == p->getX() && y == p->getY());
}

bool StudentWorld::isExit(int x, int y)
{
  return(x == e->getX() && y == e->getY());
}

Exit* StudentWorld::getExit()
{
  return e;
}

Player* StudentWorld::getPlayer()
{
  return p;
}

bool StudentWorld::isComplete()
{
  for(list<Actor*>::iterator p = objects.begin();
      p != objects.end(); p++){
    Jewel* j = dynamic_cast<Jewel*>(*p);
    if(j != nullptr)
      return false;
  }
  return true;
}

int StudentWorld::init()
{
  bonus = 1000;
  ostringstream tempS;
  tempS << "level";
  tempS.fill('0');
  tempS << setw(2) << getLevel() << ".dat";
  string cLevel = tempS.str();

  Level lev(assetDirectory());
  Level::LoadResult result = lev.loadLevel(cLevel);

  int speed_Snarl = ( ( (28 - getLevel()) / 4 ) > 3 )
    ? ((28 - getLevel()) /4 ): 3;

  if (result == Level::load_fail_file_not_found)
    return -1;
  else if (result == Level:: load_fail_bad_format)
    return -1;

  for(int i = 0; i < 15; i++)
    for(int j = 0; j < 15; j++){
      Level::MazeEntry ge = lev.getContentsOf(i,j);
      Actor* tmp = nullptr;
      switch(ge){
      case Level::hole:
        tmp = new Hole(i,j, this);
        break;
      case Level::wall:
        tmp = new Wall(i,j,this);
        break;
      case Level::player:
        p = new Player(i,j,this);
        update(i,j, p);
        break;
      case Level::boulder:
        tmp = new Boulder(i,j,this);
        break;
      case Level::exit:
        e = new Exit(i,j,this);
        break;
      case Level::jewel:
        tmp = new Jewel(i, j, this);
        break;
      case Level::extra_life:
        tmp = new Extra_life(i, j, this);
        break;
      case Level::restore_health:
        tmp = new Restore_health(i, j, this);
        break;
      case Level::ammo:
        tmp = new Ammo(i, j, this);
      case Level::empty:
        deUpdate(i, j);
        break;
      case Level::horiz_snarlbot:
        tmp = new SnarlBot(i, j, this, speed_Snarl, Actor::right);
        break;
      case Level::vert_snarlbot:
        tmp = new SnarlBot(i, j, this, speed_Snarl, Actor::down);
        break;
      case Level::kleptobot_factory:
        tmp = new KleptoBot_Factory(i, j, this, speed_Snarl);
        break;
      case Level::angry_kleptobot_factory:
        tmp = new KleptoBot_Factory(i, j, this, speed_Snarl, true);
      default:
        break;
      }
      addActor(tmp);
    }
  return 1;
}

int StudentWorld::move()
{
  if(tick >= 5000)
    tick = 0;
  else
    tick++;

  if(e->isAlive())
    e->doSomething();
  else{
    playSound(SOUND_FINISHED_LEVEL);
    increaseScore(bonus);
    return GWSTATUS_FINISHED_LEVEL;
  }

  if(p->isAlive())
    p->doSomething();
  else{
    decLives();
    playSound(SOUND_PLAYER_DIE);
    return GWSTATUS_PLAYER_DIED;
  }
  // let the robot move first, then the objects may update their position
  // before the player move/step on them
  list<Robot*>::iterator iterT = timer.begin();
  while(iterT != timer.end())
    {
      if(!(*iterT)->isAlive())
        {
          (*iterT)->die();
          delete *iterT;
          iterT = timer.erase(iterT);
        }
      else
        {
          (*iterT)->doSomething(tick);
          iterT++;
        }
    }

  list<Actor*>::iterator iterO = objects.begin();
  while(iterO != objects.end())
    {
      if(!(*iterO)->isAlive())
        {
          delete *iterO;
          iterO = objects.erase(iterO);
        }
      else
        {
          (*iterO)->doSomething();
          iterO++;
        }
    }

  if(bonus > 0)
    bonus--;
  ostringstream tmpS;
  tmpS << "Score: ";
  tmpS.fill('0');
  tmpS << setw(7) << getScore() << "  Level: "
       << setw(2) << getLevel() << "  Lives: ";
  tmpS.fill(' ');
  tmpS << setw(2) << getLives() << "  Health: "
       << setw(3) << (p->getHealth() * 100 / 20) << "%  Ammo: "
       << setw(3) << p->getAmmo() << "  Bonus: "
       << setw(4) << bonus;
  string s = tmpS.str();
  setGameStatText(s);

  return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
  // for(vector< vector<Actor*> >::iterator o = map.begin();
  //     o != map.end(); o++)
  //   for(vector<Actor*>::iterator q = o->begin(); q != o->end(); o++)
  //     delete *q;
  // delete p;

  list<Actor*>::iterator q = objects.begin();
  while(q != objects.end()){
    delete *q;
    q = objects.erase(q);
  }
  list<Robot*>::iterator t = timer.begin();
  while(t != timer.end()){
    delete *t;
    t = timer.erase(t);
  }

  for(int i = 0; i < 15; i++)
    for(int j = 0; j < 15; j++)
      map[i][j] = nullptr;

  delete p;
  p = nullptr;
  delete e;
  e = nullptr;
}


GameWorld* createStudentWorld(string assetDir)
{
  return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
