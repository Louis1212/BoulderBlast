#include "Actor.h"
#include "StudentWorld.h"
#include <stdlib.h>
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


bool Character::moveNMark(int ox, int oy, bool isBoulder)
{
  if(getDirection() != none){
    int n = (getX() - ox) + (getY() - oy) * 3;
    switch(n){
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
  if(getWorld()->isWalkable(ox, oy)){
    if(isBoulder &&
       (!getWorld()->isEmpty(ox, oy) || getWorld()->isExit(ox, oy) ) )
        return false;
    getWorld()->update(ox, oy, this);
    if(getWorld()->getActor(getX(), getY()) == this )
      getWorld()->deUpdate(getX(), getY());
    moveTo(ox, oy);
    return true;
  }
  return false;
}

void Character::attacked(int sound)
{
  hitPoint -= 2;
  if(hitPoint <= 0)
    {
      this->setDead();
      getWorld()->deUpdate(getX(), getY());
    }
  else
    getWorld()->playSound(sound);
}

void Character::setHealth(int p)
{hitPoint = p;}

int Character::getHealth()
{return hitPoint;}

void Character::fire(int sound)
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

  getWorld()->playSound(sound);
  Bullet* tmp = new Bullet(x, y, getWorld(), d);
  getWorld()->addActor(tmp, false);
}

Character::~Character()
{return;}

//----------Wall----------
Wall::Wall(int x, int y, StudentWorld* ptr, int id)
  :Object(x, y, id, ptr){}

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
            b->moveNMark(x-1, y, true);
          break;
        case -1:
          if( !b->fill(x+1, y) )
            b->moveNMark(x+1, y, true);
          break;
        case 3:
          if( !b->fill(x, y-1) )
            b->moveNMark(x, y-1, true);
          break;
        case -3:
          if( !b->fill(x, y+1) )
            b->moveNMark(x, y+1, true);
          break;
        default:
          break;
        }
    }
}

void Player::fire(int sound)
{
  if(ammu <= 0)
    return;
  ammu--;
  Character::fire(sound);
}

void Player::attacked(int blank)
{
  Character::attacked(SOUND_PLAYER_IMPACT);
}

void Player::pickUp(int x, int y)
{
  Jewel* j = dynamic_cast<Jewel*>
    (getWorld()->getActor(x, y));
  Ammo* a = dynamic_cast<Ammo*>
    (getWorld()->getActor(x, y));
  Extra_life* l = dynamic_cast<Extra_life*>
    (getWorld()->getActor(x, y));
  Restore_health* h = dynamic_cast<Restore_health*>
    (getWorld()->getActor(x, y));

  if(j != nullptr){
    buff('J');
    j->collected();
    getWorld()->deUpdate(x, y);
  } else if(a != nullptr){
    buff('A');
    a->collected();
    getWorld()->deUpdate(x, y);
  } else if(l != nullptr){
    buff('L');
    l->collected();
    getWorld()->deUpdate(x, y);
  } else if(h != nullptr){
    buff('H');
    h->collected();
    getWorld()->deUpdate(x, y);
  } else if(getWorld()->isExit(x, y) &&
            getWorld()->getExit()->isRevealed()){
    buff('E');
    getWorld()->getExit()->setDead();
  }
}
void Player::buff(char c)
{
  switch(c)
    {
    case 'J':
      getWorld()->increaseScore(50);
      break;
    case 'A':
      ammu += 20;
      getWorld()->increaseScore(100);
      break;
    case 'L':
      getWorld()->incLives();
      getWorld()->increaseScore(1000);
      break;
    case 'H':
      setHealth(20);
      getWorld()->increaseScore(500);
      break;
    case 'E':
      getWorld()->increaseScore(2000);
      break;
    default:
      break;
    }

}

void Player::doSomething()
{
  //temp version: can only move
  if(!isAlive())
    return;
  int ch;
  int ox = getX();
  int oy = getY();
  if( getWorld()->getKey(ch) ){
    switch(ch){
    case KEY_PRESS_LEFT:
      ox -= 1;
      break;
    case KEY_PRESS_RIGHT:
      ox += 1;
      break;
    case KEY_PRESS_UP:
      oy += 1;
      break;
    case KEY_PRESS_DOWN:
      oy -= 1;
      break;
    case KEY_PRESS_SPACE:
      fire();
      break;
    case KEY_PRESS_ESCAPE:
      setDead();
    default:
      break;
    }
    if(ox != getX() || oy != getY()){
      pickUp(ox, oy);
      push(ox, oy);
      moveNMark(ox, oy);
    }
  }
}

int Player::getAmmo()
{return ammu;}

//----------Boulder----------
Boulder::Boulder(int x, int y, StudentWorld* ptr)
  :Character(x, y, 10, IID_BOULDER, ptr){}

bool Boulder::fill(int x, int y)
{
  Hole* h = dynamic_cast<Hole*>(getWorld()->getActor(x,y));
  if(h != nullptr){
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
  if(p != nullptr)
    p->attacked(0);
  setDead();
}

void Bullet::doSomething()
{
  //pass through goodies, holes,
  //damage robots, player, boulders
  //stop at Factories, Walls,
  if(!isAlive())
    return;

  if(getWorld()->isBlocked(getX(), getY())){
    damage(getX(),getY());
    return;
  }

  switch(getDirection()){
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

//----------Collectable----------
Collectable::Collectable(int x, int y, int id, StudentWorld* ptr)
  :Object(x, y, id, ptr){}

void Collectable::collected()
{
  setDead();
  getWorld()->playSound(SOUND_GOT_GOODIE);
}

void Collectable::doSomething()
{
  if(getWorld()->isWalkable(getX(), getY()))
    getWorld()->update(getX(), getY(), this);
}

//----------Jewel----------
Jewel::Jewel(int x, int y, StudentWorld* ptr)
  :Collectable(x, y, IID_JEWEL, ptr){}

//----------Ammo----------
Ammo::Ammo(int x, int y, StudentWorld* ptr)
  :Collectable(x, y, IID_AMMO, ptr){}

//----------Extra life----------
Extra_life::Extra_life(int x, int y, StudentWorld* ptr)
  :Collectable(x, y, IID_EXTRA_LIFE, ptr){}

//----------Restore health----------
Restore_health::Restore_health(int x, int y, StudentWorld* ptr)
  :Collectable(x, y, IID_RESTORE_HEALTH, ptr){}

//----------Exit----------
Exit::Exit(int x, int y, StudentWorld* ptr)
  :Actor(IID_EXIT, x, y, ptr), revealed(false){setVisible(false);}

bool Exit::isRevealed()
{
  return revealed;
}

void Exit::doSomething()
{
  if(!revealed){
    if(getWorld()->isComplete()){
      revealed = true;
      getWorld()->playSound(SOUND_REVEAL_EXIT);
      setVisible(true);
    }
  }
}

//----------Robot----------

Robot::Robot(int x, int y, StudentWorld* ptr, int id, Direction d, int sr)
  :Character(x, y, 10, id, ptr, d), speed_recip(sr){}

void Robot::die()
{
  getWorld()->playSound(SOUND_ROBOT_DIE);
}

Robot::~Robot()
{return;}

void Robot::doSomething(int tick)
{
  switch(getDirection()){
  case up:
    if( !moveNMark(getX(), getY()+1) )
      setDirection(down);
    break;
  case down:
    if( !moveNMark(getX(), getY()-1) )
      setDirection(up);
    break;
  case right:
    if( !moveNMark(getX()+1, getY()) )
      setDirection(left);
    break;
  case left:
    if(!moveNMark(getX()-1, getY()) )
      setDirection(right);
    break;
  default:
    break;
  }
}

bool Robot::shouldMove(int tick)
{
  return (tick % speed_recip == 0);
}

void Robot::fire(int sound)
{
  Character::fire(sound);
}

bool Robot::shouldFire(Player* p1)
{
  int h = p1->getX() - this->getX();
  int v = p1->getY() - this->getY();
  bool sf = false;
  Direction d = getDirection();

  if(h == 0){
    if(v > 0 && d == up){
      sf = true;
      for(int i = 1; i < v; i++)
        if(getWorld()->isBlocked(getX(), getY() + i))
          sf = false;
    }
    else if(v < 0 && d == down){
      sf = true;
      for(int i = 1; i < (-v); i++)
        if(getWorld()->isBlocked(getX(), getY() - i))
          sf = false;
    }
  }
  else if(v == 0){
    if(h > 0 && d == right){
      sf = true;
      for(int i = 1; i < h; i++)
        if(getWorld()->isBlocked(getX() + i, getY()))
          sf = false;
    }
    else if(h < 0 && d == left){
      sf = true;
      for(int i = 1; i < (-h); i++)
        if(getWorld()->isBlocked(getX() - i, getY()))
          sf = false;
    }
  }
  return sf;
}

//----------SnarlBot----------

SnarlBot::SnarlBot(int x, int y, StudentWorld* ptr, int sr, Direction d)
  :Robot(x, y, ptr, IID_SNARLBOT, d, sr) {}

void SnarlBot::doSomething()
{return;}

void SnarlBot::attacked(int blank)
{
  Character::attacked(SOUND_ROBOT_IMPACT);
}

void SnarlBot::die()
{
  Robot::die();
  getWorld()->increaseScore(100);
}

void SnarlBot::doSomething(int tick)
{
  if(!shouldMove(tick))
    return;

  if(shouldFire(getWorld()->getPlayer())){
    fire();
    return;
  }

  Robot::doSomething(tick);
}

//----------KleptoBot----------

KleptoBot::KleptoBot(int x, int y, StudentWorld* ptr, int sr, int id)
  :Robot(x, y, ptr, id, right, sr), turnUntil((rand() % 5) + 1),
   picked(nullptr){}

KleptoBot::~KleptoBot()
{
  if(isAlive())
    if(picked != nullptr)
      delete picked;
}

void KleptoBot::randDirection()
{
  switch(rand() % 4){
  case 0:
    setDirection(up);
    break;
  case 1:
    setDirection(down);
  case 2:
    setDirection(left);
  case(3):
    setDirection(right);
  }
}

bool KleptoBot::isEnclosed(int& x, int& y)
{
  if(getWorld()->isWalkable(getX()+1, getY())){
    x++;
    return true;
  }
  else if(getWorld()->isWalkable(getX()-1, getY())){
    x--;
    return true;
  }
  else if(getWorld()->isWalkable(getX(), getY()+1)){
    y++;
    return true;
  }
  else if(getWorld()->isWalkable(getX(), getY()-1)){
    y--;
    return true;
  }
  else
    return false;
}

bool KleptoBot::pickUp(int x, int y)
{
  if(rand() % 10 != 0 || picked != nullptr)
    return false;
  Ammo* a = dynamic_cast<Ammo*>
    (getWorld()->getActor(x, y));
  Extra_life* e = dynamic_cast<Extra_life*>
    (getWorld()->getActor(x, y));
  Restore_health* h = dynamic_cast<Restore_health*>
    (getWorld()->getActor(x, y));

  if(a != nullptr){
    a->setDead();
    getWorld()->playSound(SOUND_ROBOT_MUNCH);
    picked = new Ammo(x, y, getWorld());
    picked->setVisible(false);
    return true;
  }
  else if(e != nullptr){
    e->setDead();
    getWorld()->playSound(SOUND_ROBOT_MUNCH);
    picked = new Extra_life(x, y, getWorld());
    picked->setVisible(false);
    return true;
  }
  else if(h != nullptr){
    h->setDead();
    getWorld()->playSound(SOUND_ROBOT_MUNCH);
    picked = new Restore_health(x, y, getWorld());
    picked->setVisible(false);
    return true;
  }
  return false;
}

void KleptoBot::die()
{
  Robot::die();
  if(picked != nullptr)
    getWorld()->addActor(picked);
}

void KleptoBot::doSomething()
{
  if(pickUp(getX(), getY()))
    return;

  if(turnUntil == 0){
    randDirection();
    turnUntil = (rand() % 5) + 1;
  }
  turnUntil--;

  int ox = getX();
  int oy = getY();
  Direction d0 = getDirection();
  switch(d0){
  case up:
    oy++;
    break;
  case down:
    oy--;
    break;
  case left:
    ox--;
    break;
  case right:
    ox++;
    break;
  default:
    break;
  }
  if(getWorld()->isWalkable(ox, oy))
    moveNMark(ox, oy);
  else{
    randDirection();
    turnUntil = (rand() % 5) + 1;
    if(isEnclosed(ox, oy))
      return;
    else
      moveNMark(ox, oy);
  }
}

//----------Normal_KleptoBot----------
Normal_KleptoBot::Normal_KleptoBot(int x, int y, StudentWorld* ptr, int sr)
  :KleptoBot(x, y, ptr, sr, IID_KLEPTOBOT){ setHealth(5); }

void Normal_KleptoBot::doSomething(int tick)
{
  if(!shouldMove(tick))
    return;
  KleptoBot::doSomething();
}

void Normal_KleptoBot::die()
{
  KleptoBot::die();
  getWorld()->increaseScore(10);
}

//----------Angry_KleptoBot----------

Angry_KleptoBot::Angry_KleptoBot(int x, int y, StudentWorld* ptr, int sr)
  :KleptoBot(x, y, ptr, sr, IID_ANGRY_KLEPTOBOT) { setHealth(8); }

void Angry_KleptoBot::doSomething(int tick)
{
  if(!shouldMove(tick))
    return;
  if(shouldFire(getWorld()->getPlayer())){
    fire();
    return;
  }
  KleptoBot::doSomething();
}

void Angry_KleptoBot::die()
{
  KleptoBot::die();
  getWorld()->increaseScore(20);
}

//----------KleptoBot_Factory----------
KleptoBot_Factory::KleptoBot_Factory(int x, int y, StudentWorld* ptr,
                                     int sr, bool isAngry)
  :Wall(x, y, ptr, IID_ROBOT_FACTORY), speed(sr), isAngry(angry){}

void KleptoBot_Factory::doSomething()
{
  if(!getWorld()->isBlocked(getX(), getY()))
    getWorld()->update(getX(), getY(), this);

  if(rand() % 50 != 0 )
    return;
  if(shouldCreate()){
    getWorld()->playSound(SOUND_ROBOT_BORN);
    if(!isAngry)
      getWorld()->addActor( new Normal_KleptoBot(getX(), getY(), getWorld(), speed) );
    else
      getWorld()->addActor( new Angry_KleptoBot(getX(), getY(), getWorld(), speed) );
  }
}

bool KleptoBot_Factory::shouldCreate()
{
  int up = ( (getY() + 3) > 14 ) ? 14 : (getY() + 3);
  int down = ( (getY() - 3) > 0 ) ? (getY() - 3) : 0;
  int right = ( (getX() + 3) > 14 ) ? 14 : (getY() + 3);
  int left = ( (getX() - 3) > 0 ) ? (getY()  3) : 0;

  int count = 0;
  for(int i = down; i <= up; i++)
    for(int j = left; j <= right; j++)
      if(getWorld()->isKleptoBot(i, j))
        count++;

  return (count < 3);
}
