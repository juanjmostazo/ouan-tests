#include "EventManager.h"
#include <iostream>
#include <stdlib.h>
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/enable_shared_from_this.hpp>

using namespace OUAN;
EventManager mgr;

class Base
{
public:
	virtual void registerHandlers()=0;
	template<class T, class EventT,TEventType eventType>
	EventHandlerPtr registerEventHandler(boost::shared_ptr<T> instance,void (T::*callback)(boost::shared_ptr<EventT>),EventManager& evtManager)
	{
		EventHandlerPtr eh = EventHandlerPtr(new EventHandler<T,EventT>(instance,callback));
		evtManager.registerHandler(eh,eventType);
		return eh;
	}
};

class Janderklander: public Base, public boost::enable_shared_from_this<Janderklander>
{
public:
	void registerHandlers()
	{
		boost::shared_ptr<Janderklander> _this= shared_from_this();
		registerEventHandler<Janderklander,ChangeWorldEvent,EVENT_TYPE_CHANGEWORLD>(_this,&Janderklander::processChangeWorld,mgr);
		// do not pay attention to the event type...I was feeling too lazy to define an EVENT_TYPE_ENEMYHIT 
		// and pasted the values from OUAN to the test app -_-U
		registerEventHandler<Janderklander,EnemyHitEvent,EVENT_TYPE_GAMEOVER>(_this,&Janderklander::processEnemyHit,mgr);
	}
	void processChangeWorld(ChangeWorldEventPtr p)
	{
		if (p->isDreamWorld())
			std::cout<<"World change: to dreams"<<std::endl;
		else
			std::cout<<"World change: to nightmares"<<std::endl;
	}
	void processEnemyHit(EnemyHitEventPtr e)
	{
		std::cout<<"Enemy "<<e->getEnemyName()<<" was hit, receiving "<<e->getDamage()<<" damage points."<<std::endl;
	}
};

class Kurchakovio: public Base, public boost::enable_shared_from_this<Kurchakovio>
{
private:
	int mId;
public:
	Kurchakovio(int id):mId(id){}
	void registerHandlers()
	{
		boost::shared_ptr<Kurchakovio> _this=shared_from_this();
		registerEventHandler<Kurchakovio,EnemyHitEvent,EVENT_TYPE_GAMEOVER>(_this,&Kurchakovio::doEnemyHit,mgr);
	}
	void doEnemyHit(EnemyHitEventPtr e)
	{
		std::cout<<"KURCHAKOVIO "<<mId<<" SPEAKING: "<<e->getEnemyName()<<" has been pwned with "<<e->getDamage()<<" damage. Fuck yeah!!"<<std::endl;
	}

};

int main()
{
boost::shared_ptr<Kurchakovio> k1=boost::shared_ptr<Kurchakovio>(new Kurchakovio(42));
k1->registerHandlers();
boost::shared_ptr<Kurchakovio> k2=boost::shared_ptr<Kurchakovio>(new Kurchakovio(100));
k2->registerHandlers();
boost::shared_ptr<Janderklander> j=boost::shared_ptr<Janderklander>(new Janderklander());
j->registerHandlers();

EventPtr p1(new EnemyHitEvent("Zombiajo",20));
mgr.addEvent(p1);
EventPtr p2(new EnemyHitEvent("Eskeletorr",20));
mgr.addEvent(p2);
EventPtr p3(new ChangeWorldEvent(true));
mgr.addEvent(p3);
mgr.dispatchEvents();


EventPtr p4(new ChangeWorldEvent(false));
mgr.addEvent(p4);
EventPtr p5(new EnemyHitEvent("Rorcuato",34));
mgr.addEvent(p5);
mgr.dispatchEvents();
int i;
std::cin>>i;
}