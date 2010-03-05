#ifndef EVENTMANAGERH_H
#define EVENTMANAGERH_H
#include "Event.h"
#include "EventHandler.h"

#include <queue>
#include <map>
#include <vector>
#include <list>

namespace OUAN
{
	typedef std::priority_queue<EventPtr,std::vector<EventPtr>,EventComparator> TEventQueue;
	typedef std::list<EventHandlerPtr> TEventHandlerList;
	typedef std::map<TEventType,TEventHandlerList> TEventHandlerMap;
	class EventManager
	{
	private:
		TEventQueue mEventQueue;
		TEventHandlerMap mEventHandlers;
	public:
		EventManager();
		virtual ~EventManager();
		void addEvent(EventPtr evt);
		void dispatchEvents();
		EventPtr peekEvent() const;
		EventPtr removeEvent();

		void registerHandler(EventHandlerPtr handler,TEventType evtType);
		//void unregisterHandler(EventHandler,TEventType evtType);
	};
}
#endif