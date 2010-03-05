#include "Event.h"
using namespace OUAN;
bool EventComparator::operator() (EventPtr& evt1, EventPtr& evt2)
{
	if (!evt1.get()) return true;
	if (!evt2.get()) return false;
	// The more or less standard definition for a priority queue
	// assigns more priority to values with lower numbers.
	// However, the STL implementation does the opposite.
	// To stick with the more familiar convention, the comparison operator
	// used for this method is > instead of <, so that the queue ordering 
	// gets reversed.
	return evt1->getPriority()>evt2->getPriority();
}
//-------------------------

Event::Event(int priority, TEventType eventType)
:mEventType(eventType)
,mPriority(priority)
{
}
Event::~Event()
{

}
TEventType Event::getEventType() const
{
	return mEventType;
}
int Event::getPriority() const
{
	return mPriority;
}

//----------------------
ChangeWorldEvent::ChangeWorldEvent(bool dreamWorld)
:Event(EVT_PRIORITY_CHANGEWORLD,EVENT_TYPE_CHANGEWORLD)//TODO: Replace magic number with enum, const or delete it
,mDreamWorld(dreamWorld)
{
};
bool ChangeWorldEvent::isDreamWorld() const
{
	return mDreamWorld;
}
//----------------------
EnemyHitEvent::EnemyHitEvent(std::string enemyName, int damage)
:Event(EVT_PRIORITY_GAME_OVER,EVENT_TYPE_GAMEOVER)
,mEnemyName(enemyName)
,mDamage(damage)
{

}
std::string EnemyHitEvent::getEnemyName() const
{
	return mEnemyName;
}
int EnemyHitEvent::getDamage() const
{
	return mDamage;
}