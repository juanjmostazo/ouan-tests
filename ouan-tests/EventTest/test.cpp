#include "EventManager.h"
#include <iostream>
#include <stdlib.h>
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/enable_shared_from_this.hpp>

using namespace OUAN;
EventManager mgr;

//Mock class to be used for the examples. Its equivalent in OUAN would be GameObject (and, perhaps, some subsystems or the world manager)
class Base
{
public:
	virtual void registerHandlers()=0;
	virtual void unregisterHandlers()=0;
	template<class T, class EventT,TEventType eventType>
	void registerEventHandler(boost::shared_ptr<T> instance,void (T::*callback)(boost::shared_ptr<EventT>),EventManager& evtManager)
	{
		EventHandlerPtr eh = EventHandlerPtr(new EventHandler<T,EventT>(instance,callback));
		evtManager.registerHandler(eh,eventType);
	}
	template<class T, class EventT,TEventType eventType>
	void unregisterEventHandler(boost::shared_ptr<T> instance,void (T::*callback)(boost::shared_ptr<EventT>),EventManager& evtManager)
	{
		EventHandlerPtr eh = EventHandlerPtr(new EventHandler<T,EventT>(instance,callback));
		evtManager.unregisterHandler(eh,eventType);
		std::cout<<"Successfully unregistered handler"<<std::endl;
	}
};
// Mock subclass 1: It'll react to changeworld and gameover events
class Janderklander: public Base, public boost::enable_shared_from_this<Janderklander>
{
public:
	/// Unregister all handlers
	void unregisterHandlers()
	{
		boost::shared_ptr<Janderklander> _this= shared_from_this();
		unregisterEventHandler<Janderklander,ChangeWorldEvent,EVENT_TYPE_CHANGEWORLD>(_this,&Janderklander::processChangeWorld,mgr);
		unregisterEventHandler<Janderklander,EnemyHitEvent,EVENT_TYPE_ENEMY_HIT>(_this,&Janderklander::processEnemyHit,mgr);
	}
	/// Register all handlers
	void registerHandlers()
	{
		boost::shared_ptr<Janderklander> _this= shared_from_this();
		registerEventHandler<Janderklander,ChangeWorldEvent,EVENT_TYPE_CHANGEWORLD>(_this,&Janderklander::processChangeWorld,mgr);
		// do not pay attention to the event type...I was feeling too lazy to define an EVENT_TYPE_ENEMYHIT 
		// and pasted the values from OUAN to the test app -_-U
		registerEventHandler<Janderklander,EnemyHitEvent,EVENT_TYPE_ENEMY_HIT>(_this,&Janderklander::processEnemyHit,mgr);
	}
	/// Process a game world change event
	void processChangeWorld(ChangeWorldEventPtr p)
	{
		if (p->isDreamWorld())
			std::cout<<"World change: to dreams"<<std::endl;
		else
			std::cout<<"World change: to nightmares"<<std::endl;
	}
	/// Process an enemy hit event
	void processEnemyHit(EnemyHitEventPtr e)
	{
		std::cout<<"Enemy "<<e->getEnemyName()<<" was hit, receiving "<<e->getDamage()<<" damage points."<<std::endl;
	}
};
/// Mock subclass 2: It'll only react to gameover events
class Kurchakovio: public Base, public boost::enable_shared_from_this<Kurchakovio>
{
private:
	int mId;
public:
	Kurchakovio(int id):mId(id){}
	///Unregister all handlers
	void unregisterHandlers()
	{
		boost::shared_ptr<Kurchakovio> _this=shared_from_this();
		unregisterEventHandler<Kurchakovio,EnemyHitEvent,EVENT_TYPE_ENEMY_HIT>(_this,&Kurchakovio::doEnemyHit,mgr);
	}
	///Register all handlers
	void registerHandlers()
	{
		boost::shared_ptr<Kurchakovio> _this=shared_from_this();
		registerEventHandler<Kurchakovio,EnemyHitEvent,EVENT_TYPE_ENEMY_HIT>(_this,&Kurchakovio::doEnemyHit,mgr);
	}
	/// Process an enemy hit event
	void doEnemyHit(EnemyHitEventPtr e)
	{
		std::cout<<"KURCHAKOVIO "<<mId<<" SPEAKING: "<<e->getEnemyName()<<" has been pwned with "<<e->getDamage()<<" damage. Fuck yeah!!"<<std::endl;
	}

};

int main()
{
//Create some objets, and make them subscribe to whatever events they want to handle
boost::shared_ptr<Kurchakovio> k1=boost::shared_ptr<Kurchakovio>(new Kurchakovio(42));
k1->registerHandlers();
boost::shared_ptr<Kurchakovio> k2=boost::shared_ptr<Kurchakovio>(new Kurchakovio(100));
k2->registerHandlers();
boost::shared_ptr<Janderklander> j=boost::shared_ptr<Janderklander>(new Janderklander());
j->registerHandlers();

//Enqueue events
EventPtr p1(new EnemyHitEvent("Zombiajo",20));
mgr.addEvent(p1);
EventPtr p2(new EnemyHitEvent("Eskeletorr",20));
mgr.addEvent(p2);
EventPtr p3(new ChangeWorldEvent(true));
mgr.addEvent(p3);
//Process p1, p2 and p3
mgr.dispatchEvents();

//Unsubscribe k2 from handling enemy hit events
k2->unregisterEventHandler<Kurchakovio,EnemyHitEvent,EVENT_TYPE_GAMEOVER>(k2,&Kurchakovio::doEnemyHit,mgr);

//Enqueue some more events
EventPtr p4(new ChangeWorldEvent(false));
mgr.addEvent(p4);
EventPtr p5(new EnemyHitEvent("Rorcuato",34));
mgr.addEvent(p5);

//...and dispatch them
mgr.dispatchEvents();
j->unregisterHandlers();
k1->unregisterHandlers();
k2->unregisterHandlers();
int i;
std::cin>>i;
}