#ifndef EVENTDEFSH_H
#define EVENTDEFSH_H
namespace OUAN
{
	// Priority values for all event types: 0 is highest
	const int EVT_PRIORITY_GAME_OVER=0;
	const int EVT_PRIORITY_ENEMY_HIT=1;
	const int EVT_PRIORITY_ONY_DEATH=2;	//Ony loses 1 live
	const int EVT_PRIORITY_CHANGEWORLD=3;
	
	// Event types
	typedef enum{
		EVENT_TYPE_CHANGEWORLD,
		EVENT_TYPE_GAMEOVER,
		EVENT_TYPE_ONY_DEATH,
		EVENT_TYPE_ENEMY_HIT
	} TEventType;
}

#endif