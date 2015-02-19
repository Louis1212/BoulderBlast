#include "StudentWorld.h"
#include "Actor.h"
#include "Level.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

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
  :GameWorld(assetDir), p(nullptr)
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
  for(list<Actor*>::iterator q = objects.begin(); q != objects.end(); q++)
    delete *q;
  delete p;
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
  if(x >= 15 || y >= 15 ||
     x < 0 || y < 0)
    return false;
  return (map[x][y] == nullptr);
}

Actor* StudentWorld::getActor(int x, int y)
{
  return map[x][y];
}

int StudentWorld::init()
{
  ostringstream tempS;
  tempS << "level";
  tempS.fill('0');
  tempS << setw(2) << getLevel() << ".dat";
  string cLevel = tempS.str();

  Level lev(assetDirectory());
  Level::LoadResult result = lev.loadLevel(cLevel);

  if (result == Level::load_fail_file_not_found)
    return -1;
  else if (result == Level:: load_fail_bad_format)
    return -1;

  for(int i = 0; i < 15; i++)
    for(int j = 0; j < 15; j++)
      {
        Level::MazeEntry ge = lev.getContentsOf(i,j);
        switch(ge)
          {
          case Level::wall:
            {
              Actor* tmp = new Wall(i,j,this);
              objects.push_back(tmp);
              update(i,j, tmp);
              break;
            }
          case Level::player:
            {
              p = new Player(i,j,this);
              update(i,j, p);
              break;
            }
              case Level::boulder:
            {
              Actor* tmp = new Boulder(i,j,this);
              objects.push_back(tmp);
              update(i,j, tmp);
              break;
            }
          default:
            break;
          // case Level::empty:
          //   break;
          // case Level::exit:
          //   break;
          // case Level::horiz_snarlbot:
          //   break;
          // case Level::vert_snarlbot:
          //   break;
          // case Level::kleptobot_factory:
          //   break;
          // case Level::angry_kleptobot_factory:
          //   break;
          }
      }
    return 1;
}

int StudentWorld::move()
{
  if(p->isAlive())
    p->doSomething();
  return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
  // for(vector< vector<Actor*> >::iterator o = map.begin();
  //     o != map.end(); o++)
  //   for(vector<Actor*>::iterator q = o->begin(); q != o->end(); o++)
  //     delete *q;
  // delete p;

  for(list<Actor*>::iterator q = objects.begin(); q != objects.end(); q++)
    delete *q;
}


GameWorld* createStudentWorld(string assetDir)
{
  return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
