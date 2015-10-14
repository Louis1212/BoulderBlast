#include "Actor.h"
#include "StudentWorld.h"
#include <stdlib.h>
//----------Help----------


//----------Actor----------
Actor::Actor(int id, int x, int y, StudentWorld* ptr, Direction d)
  :GraphObject(id, x, y, d),
   isDead(false), world(ptr) {setVisible(true);}

void Actor::doSomething()
{return;}

bool Actor::isAlive()
{return !isDead;}

void Actor::setDead()
{isDead = true;}

StudentWorld* Actor::getWorld()
{return world;}

Actor::~Actor()
{return;}

//----------Wall----------
Wall::Wall(int x, int y, StudentWorld* ptr, int id)
  :Actor(id, x, y, ptr){}

//----------KleptoBot_Factory----------
KleptoBot_Factory::KleptoBot_Factory(int x, int y,
                                     StudentWorld* ptr, bool angry)
  :Wall(x, y, ptr, IID_ROBOT_FACTORY), isAngry(angry){}

void KleptoBot_Factory::doSomething()
{
  // update location when no KleptoBot are on the Factory
  if(getWorld()->isWalkable(getX(), getY()))
    getWorld()->update(getX(), getY(), this);
  // the 1/50 chance
  if((rand() % 50) != 0 )
    return;
  if(shouldCreate()){
    getWorld()->playSound(SOUND_ROBOT_BORN);
    if(!isAngry)
      getWorld()->addActor( new Normal_KleptoBot(getX(), getY(), getWorld()) );
    else
      getWorld()->addActor( new Angry_KleptoBot(getX(), getY(), getWorld()) );
  }
}

bool KleptoBot_Factory::shouldCreate()
{
  if(getWorld()->isKleptoBot(getX(), getY()))
    return false;
  // get the 7*7 box
  int up = ( (getY() + 3) > 14 ) ? 14 : (getY() + 3);
  int down = ( (getY() - 3) > 0 ) ? (getY() - 3) : 0;
  int right = ( (getX() + 3) > 14 ) ? 14 : (getX() + 3);
  int left = ( (getX() - 3) > 0 ) ? (getX() - 3) : 0;
  // count number of KleptoBots
  int count = 0;
  for(int i = down; i <= up; i++)
    for(int j = left; j <= right; j++)
      if(getWorld()->isKleptoBot(j, i))
        count++;

  return (count < 3);
}

//----------Hole----------
Hole::Hole(int x, int y, StudentWorld* ptr)
  :Actor(IID_HOLE, x, y, ptr){}

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
  // move along the current direction
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
  // check the new square!
    if(getWorld()->isBlocked(getX(), getY())){
    damage(getX(),getY());
    return;
  }
}

//----------Collectable----------
Collectable::Collectable(int x, int y, int id, StudentWorld* ptr)
  :Actor(id, x, y, ptr){}

void Collectable::collected()
{
  setDead();
  getWorld()->playSound(SOUND_GOT_GOODIE);
  getWorld()->deUpdate_collectable(getX(), getY());
}

Collectable::~Collectable()
{return;}

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
  :Collectable( x, y,IID_EXIT, ptr), revealed(false){setVisible(false);}

void Exit::doSomething()
{
  // just check and reveal itself
  if(!revealed){
    if(getWorld()->isComplete()){
      revealed = true;
      getWorld()->playSound(SOUND_REVEAL_EXIT);
      setVisible(true);
    }
  }
}

bool Exit::isRevealed()
{
  return revealed;
}

//----------Character----------
Character::Character(int x, int y, int points, int id,
                     StudentWorld* ptr, Direction d)
  :Actor(id, x, y, ptr, d), hitPoint(points){}


bool Character::moveNMark(int ox, int oy, bool isBoulder)
{
  // in Boulder doesn't have a direction!
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
    // Boulder has more strict blocking rules
    if(isBoulder && !getWorld()->isEmpty(ox, oy))
        return false;
    getWorld()->update(ox, oy, this);
    // TODO - might not need this anymore
    if(getWorld()->getActor(getX(), getY()) == this )
      getWorld()->deUpdate_character(getX(), getY());
    moveTo(ox, oy);
    return true;
  }
  return false;
}

void Character::attacked(int sound)
{
  hitPoint -= 2;
  if(hitPoint <= 0){
    this->setDead();
    getWorld()->deUpdate_character(getX(), getY());
  }
  else
    getWorld()->playSound(sound);
}

void Character::fire(int sound)
{
  int x = getX();
  int y = getY();
  Direction d = getDirection();
  switch(d){
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
  // Robots and Player have different sounds
  getWorld()->playSound(sound);
  Bullet* tmp = new Bullet(x, y, getWorld(), d);
  getWorld()->addActor(tmp, false); // don't update location
}

Character::~Character()
{return;}

void Character::setHealth(int p)
{hitPoint = p;}

int Character::getHealth()
{return hitPoint;}

//----------Player----------
Player::Player(int x, int y, StudentWorld* ptr)
  :Character(x, y, 20, IID_PLAYER, ptr, right),
   ammu(20){}

void Player::doSomething()
{
  if(!isAlive())
    return;
  int ch;
  int ox = getX();
  int oy = getY();
  pickUp(ox, oy);
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
    } // If hit the direction key
    if(ox != getX() || oy != getY()){
      push(ox, oy); // check if Boulder
      moveNMark(ox, oy);
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

void Player::push(int x, int y)
{
  Boulder* b = dynamic_cast<Boulder*>( getWorld()->getActor(x, y) );
  if (b != nullptr){
    // get the direction of Boulder about Player
    int n = (getX() - x) + (getY() - y) * 3;
    switch(n){
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


void Player::pickUp(int x, int y)
{
  Jewel* j = dynamic_cast<Jewel*>
    (getWorld()->getCollectable(x, y));
  Ammo* a = dynamic_cast<Ammo*>
    (getWorld()->getCollectable(x, y));
  Extra_life* l = dynamic_cast<Extra_life*>
    (getWorld()->getCollectable(x, y));
  Restore_health* h = dynamic_cast<Restore_health*>
    (getWorld()->getCollectable(x, y));
  Exit* e = dynamic_cast<Exit*>
    (getWorld()->getCollectable(x, y));

  if(j != nullptr){
    buff('J');
    j->collected();
   } else if(a != nullptr){
    buff('A');
    a->collected();
   } else if(l != nullptr){
    buff('L');
    l->collected();
   } else if(h != nullptr){
    buff('H');
    h->collected();
   } else if(e != nullptr && e->isRevealed()){
    buff('E');
    e->setDead();
    getWorld()->finish();
   }
}
void Player::buff(char c)
{
  switch(c){
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
    getWorld()->deUpdate_character(x,y);
    getWorld()->deUpdate_character(getX(), getY());
    return true;
  }
  return false;
}

//----------Robot----------

Robot::Robot(int x, int y, StudentWorld* ptr, int id, Direction d)
  :Character(x, y, 10, id, ptr, d)
{
  // calculate the speed according to the level number
  speed_recip = ( ( (28 - ptr->getLevel()) / 4 ) > 3 )
    ? ((28 - ptr->getLevel()) /4 ): 3;
}

bool Robot::shouldMove(int tick)
{
  // move every "speed" ticks
  return (tick % speed_recip == 0);
}

bool Robot::shouldFire(Player* p1)
{
  int h = p1->getX() - this->getX();
  int v = p1->getY() - this->getY();
  bool sf = false;
  Direction d = getDirection();
  // same col
  if(h == 0){
    if(v > 0 && d == up){
      sf = true;
      // if blocked
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
  }// same row
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

void Robot::attacked(int blank)
{
  Character::attacked(SOUND_ROBOT_IMPACT);
}

void Robot::fire(int sound)
{
  Character::fire(sound);
}

Robot::~Robot()
{
  // if it's still alive-> the level finished, don't play sound.
  if(!isAlive())
    getWorld()->playSound(SOUND_ROBOT_DIE);
}

//----------SnarlBot----------

SnarlBot::SnarlBot(int x, int y, StudentWorld* ptr, Direction d)
  :Robot(x, y, ptr, IID_SNARLBOT, d) {}

void SnarlBot::doSomething()
{
  if(!isAlive())
    return;
  if(!shouldMove(getWorld()->getTick()))
    return;
  if(shouldFire(getWorld()->getPlayer())){
    fire();
    return;
  }
  // move along current direction
  switch(getDirection()){
  case up:
    if( !moveNMark(getX(), getY()+1) )
      // if blocked, turn to opposite direction
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

SnarlBot::~SnarlBot()
{
  // if still alive-> level finished, don't add socre
  if(!isAlive())
    getWorld()->increaseScore(100);
}

//----------KleptoBot----------

KleptoBot::KleptoBot(int x, int y, StudentWorld* ptr, int id)
  :Robot(x, y, ptr, id, right), turnUntil((rand() % 5) + 1),
   picked(nullptr){}

void KleptoBot::doSomething()
{
  if(!isAlive())
    return;
  if(turnUntil == 0){
    randDirection();
    turnUntil = (rand() % 5) + 1;
  }
  // if picked something up
  if(pickUp(getX(), getY()))
    return;

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
  if(getWorld()->isWalkable(ox, oy)){
    moveNMark(ox, oy);
    turnUntil--;
  } // if blocked
  else{
    randDirection();
    ox = getX();
    oy = getY();
    turnUntil = (rand() % 5) + 1;
    if(isEnclosed(ox, oy))
      return;
    else{
      moveNMark(ox, oy);
       turnUntil--;
    }
  }
}

void KleptoBot::randDirection()
{
  switch(rand() % 4){
  case 0:
    setDirection(up);
    break;
  case 1:
    setDirection(down);
    break;
  case 2:
    setDirection(left);
    break;
  case(3):
    setDirection(right);
    break;
  }
}

bool KleptoBot::isEnclosed(int& x, int& y)
{
  if(getWorld()->isWalkable(getX()+1, getY())){
    x++;
    return false;
  }
  else if(getWorld()->isWalkable(getX()-1, getY())){
    x--;
    return false;
  }
  else if(getWorld()->isWalkable(getX(), getY()+1)){
    y++;
    return false;
  }
  else if(getWorld()->isWalkable(getX(), getY()-1)){
    y--;
    return false;
  }
  else
    return true;
}

bool KleptoBot::pickUp(int x, int y)
{
  if(rand() % 10 != 0 || picked != nullptr)
    return false;
  Ammo* a = dynamic_cast<Ammo*>
    (getWorld()->getCollectable(x, y));
  Extra_life* l = dynamic_cast<Extra_life*>
    (getWorld()->getCollectable(x, y));
  Restore_health* h = dynamic_cast<Restore_health*>
    (getWorld()->getCollectable(x, y));

  if(a != nullptr){
    a->setDead();
    getWorld()->playSound(SOUND_ROBOT_MUNCH);
    getWorld()->deUpdate_collectable(x, y);
    picked = new Ammo(x, y, getWorld());
    picked->setVisible(false);
    return true;
  }
  else if(l != nullptr){
    l->setDead();
    getWorld()->playSound(SOUND_ROBOT_MUNCH);
    getWorld()->deUpdate_collectable(x, y);
    picked = new Extra_life(x, y, getWorld());
    picked->setVisible(false);
    return true;
  }
  else if(h != nullptr){
    h->setDead();
    getWorld()->playSound(SOUND_ROBOT_MUNCH);
    getWorld()->deUpdate_collectable(x, y);
    picked = new Restore_health(x, y, getWorld());
    picked->setVisible(false);
    return true;
  }
  return false;
}

KleptoBot::~KleptoBot()
{
  if(picked != nullptr){
    if(isAlive())
      delete picked;
    else{
      if(getWorld()->isEmpty(getX(), getY())){
        picked->moveTo(getX(), getY());
        getWorld()->addActor(picked);
        picked->setVisible(true);
      }
      else
        delete picked;
    }
  }
}

//----------Normal_KleptoBot----------

Normal_KleptoBot::Normal_KleptoBot(int x, int y, StudentWorld* ptr)
  :KleptoBot(x, y, ptr, IID_KLEPTOBOT){ setHealth(5); }

void Normal_KleptoBot::doSomething()
{
  if(!isAlive())
    return;
  if(!shouldMove(getWorld()->getTick()))
    return;
  KleptoBot::doSomething();
}

Normal_KleptoBot::~Normal_KleptoBot()
{
  if(!isAlive())
    getWorld()->increaseScore(10);
}

//----------Angry_KleptoBot----------

Angry_KleptoBot::Angry_KleptoBot(int x, int y, StudentWorld* ptr)
  :KleptoBot(x, y, ptr, IID_ANGRY_KLEPTOBOT) { setHealth(8); }

void Angry_KleptoBot::doSomething()
{
  if(!isAlive())
    return;
  if(!shouldMove(getWorld()->getTick()))
    return;
  if(shouldFire(getWorld()->getPlayer())){
    fire();
    return;
  }
  KleptoBot::doSomething();
}

Angry_KleptoBot::~Angry_KleptoBot()
{
  if(!isAlive())
    getWorld()->increaseScore(20);
}
