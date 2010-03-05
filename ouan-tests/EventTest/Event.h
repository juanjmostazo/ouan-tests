#ifndef EVENTH_H
#define EVENTH_H
#include <boost/shared_ptr.hpp>
#include <string>
#include "EventDefs.h"
namespace OUAN
{
	class Event;
	typedef boost::shared_ptr<Event> EventPtr;
	
	class Event
	{
	private:
		int mPriority;
		TEventType mEventType; //Replace with enum
	public:
		virtual ~Event();
		Event(int priority,TEventType eventType);
		int getPriority() const;
		TEventType getEventType() const;
	};
	
	class EventComparator
	{
	public:
		bool operator()(EventPtr& evt1, EventPtr& evt2);
	};

	class ChangeWorldEvent: public Event
	{
	public:
		ChangeWorldEvent(bool mDreamWorld);
		bool isDreamWorld() const;
	private:
		bool mDreamWorld;

	};

	class EnemyHitEvent: public Event
	{
	public:
		EnemyHitEvent(std::string enemyName,int damage);
		std::string getEnemyName() const;
		int getDamage() const;
	private:
		std::string mEnemyName;
		int mDamage;

	};

	typedef boost::shared_ptr<ChangeWorldEvent> ChangeWorldEventPtr;
	typedef boost::shared_ptr<EnemyHitEvent> EnemyHitEventPtr;
}
#endif