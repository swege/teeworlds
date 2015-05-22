/* See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_CLIENT_COMPONENTS_AI_H
#define GAME_CLIENT_COMPONENTS_AI_H
#include <base/vmath.h>
#include <game/client/component.h>
#include "controls.h"
#include "../../../base/vmath.h"

class CAi : public CComponent
{
	enum Strategy {
		SEARCH,
		ATTACK,
		ESCAPE,
	};

	Strategy currentStrategy;
	bool walkableTile(int tileId);
	bool walkableTile(vector2_base<float>);
	bool aiActive;
	void strategySearch();
	void strategyAttack();
	void strategyEscape();

public:

	CAi();

	virtual void OnReset();
	virtual void OnRelease();
	virtual void OnRender();
	virtual void OnMessage(int MsgType, void *pRawMsg);
	virtual bool OnMouseMove(float x, float y);
	virtual bool OnInput(IInput::CEvent e);
	virtual void OnConsoleInit();
	virtual void OnPlayerDeath();
	virtual void Tick();
	virtual void disableAi();

	CGameClient *m_gameClient;
	int m_followClientId;

	void jump();

	void walkLeft();

	void walkRight();

	void walkStop();
};
#endif