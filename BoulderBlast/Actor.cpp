#include "Actor.h"
#include "StudentWorld.h"

//----------Help----------


//----------Actor----------
Actor::Actor(int id, int x, int y, StudentWorld* ptr, Direction d)
  :GraphObject(id, x, y, d),
   isDead(false), world(ptr) {setVisible(true);}

bool Actor::isAlive()
{return !isDead;}

void Actor::setDead()
{isDead = true;}

StudentWorld* Actor::getWorld()
{return world;}

Actor::~Actor()
{return;}

//----------Object----------
Object::Object(int x, int y, int id, StudentWorld* ptr)
  :Actor(id, x, y, ptr){}

void Object::doSomething()
{return;}

Object::~Object()
{return;}

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

void Character::attacked()
{
  hitPoint -= 2;
  if(hitPoint <= 0)
    {
      this->setDead();
      getWorld()->deUpdate(getX(), getY());
    }
}

Character::~Character()
{return;}

//----------Wall----------
Wall::Wall(int x, int y, StudentWorld* ptr)
  :Object(x, y, IID_WALL, ptr){}

//----------Player----------
Player::Player(int x, int y, StudentWorld* ptr)
  :Character(x, y, 20, IID_PLAYER, ptr, right),
   ammu(20){}

void Player::push(int x, int y)
{
  Boulder* b = dynamic_cast<Boulder*>( getWorld()->getActor(x, y) );
  if (b != nullptr)
    {
      int n = (getX() - x) + (getY() - y) * 3;
      switch(n)
        {
        case 1:
          if( !b->fill(x-1, y) )
            b->moveNMark(x-1, y);
          break;
        case -1:
          if( !b->fill(x+1, y) )
            b->moveNMark(x+1, y);
          break;
        case 3:
          if( !b->fill(x, y-1) )
            b->moveNMark(x, y-1);
          break;
        case -3:
          if( !b->fill(x, y+1) )
            b->moveNMark(x, y+1);
          break;
        default:
          break;
        }
    }
}

void Player::fire()
{
  int x = getX();
  int y = getY();
  Direction d = getDirection();
  switch(d)
    {
    case up:
      y += 1;
      break;
    case down:
      y -= 1;
      break;
    case right:
      x += 1;
      break;
    case left:
      x -= 1;
      break;
    default:
      break;
    }

  Bullet* tmp = new Bullet(x, y, getWorld(), d);
  getWorld()->addActor(tmp, false);
}

void Player::doSomething()
{
  //temp version: can only move
  if(!isAlive())
    return;
  int ch;
  int ox = getX();
  int oy = getY();
  if( getWorld()->getKey(ch) )
    {
      switch(ch)
        {
        case KEY_PRESS_LEFT:
          push(ox -1, oy);
          moveNMark(ox -1, oy);
          break;
        case KEY_PRESS_RIGHT:
          push(ox +1, oy);
          moveNMark(ox +1, oy);
          break;
        case KEY_PRESS_UP:
          push(ox, oy +1);
          moveNMark(ox, oy +1);
          break;
        case KEY_PRESS_DOWN:
          push(ox, oy -1);
          moveNMark(ox, oy -1);
          break;
        case KEY_PRESS_SPACE:
          fire();
          break;
        default:
          break;
        }
    }
}

//----------Boulder----------
Boulder::Boulder(int x, int y, StudentWorld* ptr)
  :Character(x, y, 10, IID_BOULDER, ptr){}

bool Boulder::fill(int x, int y)
{
  Hole* h = dynamic_cast<Hole*>(getWorld()->getActor(x,y));
  if(h != nullptr)
    {
      //make them disappear first, and the dead body will be clean up
      // after a tick;
      h->setDead();
      this->setDead();
      getWorld()->deUpdate(x,y);
      getWorld()->deUpdate(getX(), getY());
      return true;
    }
  return false;
}

void Boulder::doSomething()
{return;}

//----------Hole----------
Hole::Hole(int x, int y, StudentWorld* ptr)
  :Object(x, y, IID_HOLE, ptr){}

//----------Bullete----------
Bullet::Bullet(int x, int y, StudentWorld* ptr, Direction d)
  :Actor(IID_BULLET, x, y, ptr, d){}

void Bullet::damage(int x, int y)
{
  Character* p = dynamic_cast<Character*>(getWorld()->getActor(x,y));
  p->attacked();
  setDead();
}

void Bullet::doSomething()
{
  //pass through goodies, holes,
  //damage robots, player, boulders
  //stop at Factories, Walls,
  if(!isAlive())
    return;

  if(!getWorld()->isEmpty(getX(), getY()))
    {
      Character* c = dynamic_cast<Character*>
        (getWorld()->getActor(getX(), getY()));
      Wall* w = dynamic_cast<Wall*>
        (getWorld()->getActor(getX(), getY()));
      if(w != nullptr)
        {
          setDead();
          return;
        }
      else if(c != nullptr)
        {
          damage(getX(),getY());
          return;
        }
    }
  switch(getDirection())
    {
    case up:
      moveTo(getX(), getY()+1);
      break;
    case down:
      moveTo(getX(), getY()-1);
      break;
    case right:
      moveTo(getX()+1, getY());
      break;
    case left:
      moveTo(getX()-1, getY());
      break;
    default:
      break;
    }
}
