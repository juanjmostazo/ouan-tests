#ifndef EVENTDEFSH_H
#define EVENTDEFSH_H
namespace OUAN
{
	const int EVT_PRIORITY_GAME_OVER=0;
	const int EVT_PRIORITY_ONY_DEATH=1;	//Ony loses 1 live
	const int EVT_PRIORITY_CHANGEWORLD=2;
	//
	typedef enum{
		EVENT_TYPE_CHANGEWORLD,
		EVENT_TYPE_GAMEOVER,
		EVENT_TYPE_ONY_DEATH,
	} TEventType;
}

#endif