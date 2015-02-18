#include "Actor.h"
#include "StudentWorld.h"

//----------Actor----------
Actor::Actor(int id, int x, int y, StudentWorld* ptr, GraphObject::Direction d)
  :GraphObject(id, x, y, d),
   isDead(false), world(ptr) { setVisible(true);}

bool Actor::isAlive()
{ return !isDead;}

StudentWorld* Actor::getWorld()
{ return world;}

//----------Character----------
Character::Character(int x, int y, Direction d,
                     int points, int id, StudentWorld* ptr)
  :Actor(id, x, y, ptr, d), hitPoint(points){}

//----------Wall----------
Wall::Wall(int x, int y, StudentWorld* ptr)
  :Actor(IID_WALL, x, y, ptr), id(IID_WALL){}

void Wall::doSomething()
{return;}

//----------Player----------
Player::Player(int x, int y, StudentWorld* ptr)
  :Character(x, y, right, 20, IID_PLAYER, ptr),
   id(IID_PLAYER), ammu(20){}

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
            if(getWorld()->isEmpty(ox-1, oy))
              {
                getWorld()->deUpdate(ox, oy);
                moveTo(ox-1, oy);
                getWorld()->update(ox -1, oy, '@');
              }
            break;
          }
        case KEY_PRESS_RIGHT:
          {
            if(getWorld()->isEmpty(ox+1, oy))
              {
                getWorld()->deUpdate(ox, oy);
                moveTo(ox+1, oy);
                getWorld()->update(ox+1, oy, '@');
              }
            break;
          }
        case KEY_PRESS_UP:
          {
            if(getWorld()->isEmpty(ox, oy+1))
              {
                getWorld()->deUpdate(ox, oy);
                moveTo(ox, oy+1);
                getWorld()->update(ox, oy+1, '@');
              }
            break;
          }
        case KEY_PRESS_DOWN:
          {
            if(getWorld()->isEmpty(ox, oy-1))
              {
                getWorld()->deUpdate(ox, oy);
                moveTo(ox, oy-1);
                getWorld()->update(ox, oy-1, '@');
              }
            break;
          }
        default:
          break;
        }
    }
}
