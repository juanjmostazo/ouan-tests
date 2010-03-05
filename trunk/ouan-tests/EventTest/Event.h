#ifndef EVENTH_H
#define EVENTH_H
#include <boost/shared_ptr.hpp>
#include <string>
#include "EventDefs.h"
namespace OUAN
{
	// Forward declaration
	class Event;
	// Alias for smart pointer
	typedef boost::shared_ptr<Event> EventPtr;
	
	///Base class for all events
	class Event
	{
	private:
		/// Event priority: It'll be used for sorting a priority queue
		int mPriority;
		/// Event type
		TEventType mEventType;
	public:
		/// Destructor
		virtual ~Event();
		/// Constructor: it takes a priority and event type
		/// @param priority event priority
		/// @param eventType event type
		Event(int priority,TEventType eventType);
		/// Return event priority
		/// @return event priority
		int getPriority() const;
		/// Return event type
		/// @return event type
		TEventType getEventType() const;
	};
	
	/// Functor used to compare two pointers to event
	/// instances by comparing their priorities
	class EventComparator
	{
	public:
		/// Compare two pointers to event instance
		/// @param evt1 first event to compare
		/// @param evt2 second event to compare
		/// @return true if evt1's priority is higher than evt2's
		bool operator()(EventPtr& evt1, EventPtr& evt2);
	};

	/// Event subclass to represent a change world event.
	class ChangeWorldEvent: public Event
	{
	public:
		///Constructor: It takes a flag stating
		/// if the current world is in dreams or nightmares
		/// @param dreamWorld is true if current world is the dreams one
		ChangeWorldEvent(bool dreamWorld);
		/// Return value of the dream world flag
		/// @return value of the dream world flag
		bool isDreamWorld() const;
	private:
		/// Tell if the world is the dreams one or the nightmare one
		bool mDreamWorld;

	};

	///Event subclass to represent an enemy hit event
	class EnemyHitEvent: public Event
	{
	public:
		/// Constructor: It takes the enemy name and the
		/// damage it received
		/// @param enemyName	name of the enemy
		/// @param damage		damage received by the enemy
		EnemyHitEvent(std::string enemyName,int damage);
		/// Return enemy name
		/// @return enemy name
		std::string getEnemyName() const;
		/// Return enemy damage
		/// @return enemy damage
		int getDamage() const;
	private:
		/// enemy name
		std::string mEnemyName;
		/// damage received
		int mDamage;

	};

	/// Aliases for smart pointers
	typedef boost::shared_ptr<ChangeWorldEvent> ChangeWorldEventPtr;
	typedef boost::shared_ptr<EnemyHitEvent> EnemyHitEventPtr;
}
#endif