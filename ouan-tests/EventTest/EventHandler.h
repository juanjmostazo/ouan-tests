#ifndef EVENTHANDLERH_H
#define EVENTHANDLERH_H
#include "Event.h"
namespace OUAN
{
	class BaseEventHandler
	{
	public:
		virtual ~BaseEventHandler(){}
		void handleEvent(const EventPtr evt)
		{
			invoke(evt);
		}
	private:
		virtual void invoke(const EventPtr evt)=0;
	};

	template <class T, class TEvent>
	class EventHandler: public BaseEventHandler
	{
	public:
		typedef void (T::*TMemberFunction)(boost::shared_ptr<TEvent>);
		typedef boost::shared_ptr<T> TPtr;
		typedef boost::shared_ptr<TEvent> TEventPtr;
		
		EventHandler(TPtr instance, TMemberFunction memFn) : mInstance(instance), mCallback(memFn) {}

		void invoke(const EventPtr evt)
		{
			(mInstance.get()->*mCallback)(boost::dynamic_pointer_cast<TEvent>(evt));
		}
	private:
		TPtr mInstance;
		TMemberFunction mCallback;
	};
	typedef boost::shared_ptr<BaseEventHandler> EventHandlerPtr;
}

#endif