#include "StudentWorld.h"
#include "Actor.h"
#include "Level.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <list>
using namespace std;

StudentWorld::StudentWorld(std::string assetDir)
  :GameWorld(assetDir), p(nullptr), tick(0), isFinish(false)
{
  // set the 2 maps all to nullptr
  map_character.resize(15);
  for(int i = 0; i < 15; i++)
    map_character[i].resize(15);
  for(int i = 0; i < 15; i++)
    for(int j = 0; j < 15; j++)
      map_character[i][j] = nullptr;

  map_collectable.resize(15);
  for(int i = 0; i < 15; i++)
    map_collectable[i].resize(15);
  for(int i = 0; i < 15; i++)
    for(int j = 0; j < 15; j++)
      map_collectable[i][j] = nullptr;
}

int StudentWorld::init()
{
  if(getLevel() >= 99)
    return GWSTATUS_PLAYER_WON;
  isFinish = false;
  bonus = 1000;
  ostringstream tempS;
  tempS << "level";
  tempS.fill('0');
  tempS << setw(2) << getLevel() << ".dat";
  string cLevel = tempS.str();

  Level lev(assetDirectory());
  Level::LoadResult result = lev.loadLevel(cLevel);

  if (result == Level::load_fail_file_not_found)
    return GWSTATUS_PLAYER_WON;
  if (result == Level:: load_fail_bad_format)
    return GWSTATUS_LEVEL_ERROR;
  // create Actors according to the level data
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
        tmp = new Exit(i,j,this);
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
        break;
      case Level::horiz_snarlbot:
        tmp = new SnarlBot(i, j, this, Actor::right);
        break;
      case Level::vert_snarlbot:
        tmp = new SnarlBot(i, j, this, Actor::down);
        break;
      case Level::kleptobot_factory:
        tmp = new KleptoBot_Factory(i, j, this);
        break;
      case Level::angry_kleptobot_factory:
        tmp = new KleptoBot_Factory(i, j, this, true);
        break;
      case Level::empty:
        deUpdate_character(i, j);
        deUpdate_collectable(i,j );
        break;
      default:
        break;
      }
      addActor(tmp);
    }
  return 1;
}

int StudentWorld::move()
{
  if(isFinish){
    playSound(SOUND_FINISHED_LEVEL);
    increaseScore(bonus);
    return GWSTATUS_FINISHED_LEVEL;
  }
  // keep track of ticks.
  if(tick >= 10000)
    tick = 0;
  else
    tick++;
  // if player dies
  if(p->isAlive())
    p->doSomething();
  else{
    decLives();
    playSound(SOUND_PLAYER_DIE);
    return GWSTATUS_PLAYER_DIED;
  }
  // loop the list of all Actors
  list<Actor*>::iterator iter = objects.begin();
  while(iter != objects.end()){
    if(!(*iter)->isAlive()){
      delete *iter;
      iter = objects.erase(iter);
    }
    else{
      (*iter)->doSomething();
      iter++;
    }
  }
  // decreament bonus, and update textbar
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
  // loop-delete the list of All Actors
  list<Actor*>::iterator q = objects.begin();
  while(q != objects.end()){
    delete *q;
    q = objects.erase(q);
  }
  // empty the maps
  for(int i = 0; i < 15; i++)
    for(int j = 0; j < 15; j++)
      map_character[i][j] = nullptr;

  for(int i = 0; i < 15; i++)
    for(int j = 0; j < 15; j++)
      map_collectable[i][j] = nullptr;
  // also delete player
  delete p;
  p = nullptr;
}

void StudentWorld::finish()
{
  isFinish = true;
}

void StudentWorld::addActor(Actor* a, bool ifUpdate)
{
  if(a == nullptr)
    return;
  objects.push_back(a);
  if(ifUpdate)
    update(a->getX(), a->getY(), a);
}

void StudentWorld::update(int x, int y, Actor* ptrA)
{
  // update to different maps according to different class
  Collectable* c = dynamic_cast<Collectable*>(ptrA);
  if(c != nullptr)
    map_collectable[x][y] = ptrA;
  else
    map_character[x][y] = ptrA;
}

void StudentWorld::deUpdate_character(int x, int y)
{
  map_character[x][y] = nullptr;
}

void StudentWorld::deUpdate_collectable(int x, int y)
{
  map_collectable[x][y] = nullptr;
}

bool StudentWorld::isEmpty(int x, int y)
{
  if(x >= 15 || y >= 15 || x < 0 || y < 0)
    return false;
  return (map_character[x][y] == nullptr &&
          map_collectable[x][y] == nullptr);
}

bool StudentWorld::isWalkable(int x, int y)
{
  if(x >= 15 || y >= 15 || x < 0 || y < 0)
    return false;
  return (map_character[x][y] == nullptr);
}

bool StudentWorld::isBlocked(int x, int y)
{
  Hole* h = dynamic_cast<Hole*>(map_character[x][y]);
  return (h == nullptr && !isWalkable(x, y));
}

bool StudentWorld::isKleptoBot(int x, int y)
{
  KleptoBot* k = dynamic_cast<KleptoBot*>(map_character[x][y]);
  return (k != nullptr);
}

bool StudentWorld::isPlayer(int x, int y)
{
  return(x == p->getX() && y == p->getY());
}

bool StudentWorld::isExit(int x, int y)
{
  Exit* e = dynamic_cast<Exit*>(map_character[x][y]);
  return (e != nullptr);
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

int StudentWorld::getTick()
{
  return tick;
}

Player* StudentWorld::getPlayer()
{
  return p;
}

Actor* StudentWorld::getActor(int x, int y)
{
  return map_character[x][y];
}

Actor* StudentWorld::getCollectable(int x, int y)
{
  return map_collectable[x][y];
}

StudentWorld::~StudentWorld()
{
  list<Actor*>::iterator o = objects.begin();
  while(o != objects.end()){
    delete *o;
    o = objects.erase(o);
  }
  // no need to care about the maps. There will be new ones.
  delete p;
  p = nullptr;
}

GameWorld* createStudentWorld(string assetDir)
{
  return new StudentWorld(assetDir);
}
