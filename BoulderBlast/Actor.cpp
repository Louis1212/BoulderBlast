#include "Actor.h"
#include "StudentWorld.h"

//----------Help----------


//----------Actor----------
Actor::Actor(int id, int x, int y, StudentWorld* ptr, Direction d)
  :GraphObject(id, x, y, d),
   isDead(false), world(ptr) {setVisible(true);}

void Actor::doSomething()
{return;}

bool Actor::isAlive()
{return !isDead;}

StudentWorld* Actor::getWorld()
{return world;}

//----------Character----------
Character::Character(int x, int y, int points, int id,
                     StudentWorld* ptr, Direction d = none)
  :Actor(id, x, y, ptr, d), hitPoint(points){}


void Character::moveNMark(int ox, int oy)
{
  if(getDirection() != none)
    {
      int n = (getX() - ox) + (getY() - oy) * 3;
      switch(n)
        {
        case 1:
          setDirection(left);
          break;
        case -1:
          setDirection(right);
          break;
        case 3:
          setDirection(down);
          break;
        case -3:
          setDirection(up);
          break;
        default:
          break;
        }
    }
  if(getWorld()->isEmpty(ox, oy))
    {
      getWorld()->update(ox, oy, this);
      if(getWorld()->getActor(getX(), getY()) == this )
        getWorld()->deUpdate(getX(), getY());
      moveTo(ox, oy);
    }
}

//----------Wall----------
Wall::Wall(int x, int y, StudentWorld* ptr)
  :Actor(IID_WALL, x, y, ptr), id(IID_WALL){}

//----------Player----------
Player::Player(int x, int y, StudentWorld* ptr)
  :Character(x, y, 20, IID_PLAYER, ptr, right),
   id(IID_PLAYER), ammu(20){}

void Player::push(int x, int y)
{
  Boulder* b = dynamic_cast<Boulder*>( getWorld()->getActor(x, y) );
  if (b != nullptr)
    {
      int n = (getX() - x) + (getY() - y) * 3;
      switch(n)
        {
        case 1:
          b->moveNMark(x-1, y);
          break;
        case -1:
          b->moveNMark(x+1, y);
          break;
        case 3:
          b->moveNMark(x, y-1);
          break;
        case -3:
          b->moveNMark(x, y+1);
          break;
        default:
          break;
        }
    }
}

void Player:: doSomething()
{
  //temp version: can only move
  int ch;
  int ox = getX();
  int oy = getY();
  if( getWorld()->getKey(ch) )
    {
      switch(ch)
        {
        case KEY_PRESS_LEFT:
          {
            push(ox -1, oy);
            moveNMark(ox -1, oy);
            break;
          }
        case KEY_PRESS_RIGHT:
          {
            push(ox +1, oy);
            moveNMark(ox +1, oy);
            break;
          }
        case KEY_PRESS_UP:
          {
            push(ox, oy +1);
            moveNMark(ox, oy +1);
            break;
          }
        case KEY_PRESS_DOWN:
          {
            push(ox, oy -1);
            moveNMark(ox, oy -1);
            break;
          }
        default:
          break;
        }
    }
}

//----------Boulder----------
Boulder::Boulder(int x, int y, StudentWorld* ptr)
  :Character(x, y, 10, IID_BOULDER, ptr),
   id(IID_BOULDER){}
