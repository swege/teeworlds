/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <base/math.h>

#include <engine/shared/config.h>

#include <game/collision.h>
#include <game/client/gameclient.h>
#include <game/client/component.h>
#include <game/client/components/chat.h>
#include <game/client/components/menus.h>
#include <game/client/components/scoreboard.h>
#include <stdlib.h>

#include "ai.h"
#include "../../../engine/input.h"
#include "../../generated/protocol.h"
#include "../../../base/vmath.h"
#include "../../../engine/keys.h"
#include "../gameclient.h"
#include "../../gamecore.h"
#include "../../../engine/shared/protocol.h"

CAi::CAi() {
}

void CAi::OnReset() {

}

void CAi::OnRelease() {
}

void CAi::OnPlayerDeath() {
}

static void ConKeyInputState(IConsole::IResult *pResult, void *pUserData) {
}

static void ConKeyInputCounter(IConsole::IResult *pResult, void *pUserData) {

}

struct CInputSet {

};

static void ConKeyInputSet(IConsole::IResult *pResult, void *pUserData) {

}

static void ConKeyInputNextPrevWeapon(IConsole::IResult *pResult, void *pUserData) {

}

void CAi::OnConsoleInit() {

}

void CAi::OnMessage(int Msg, void *pRawMsg) {

}


void CAi::OnRender() {

}

bool CAi::OnInput(IInput::CEvent e) {

    // manual keyboard input will not be processed
    return true;
}

bool CAi::OnMouseMove(float x, float y) {
    // manual mouse input will not be processed
    return true;
}

void CAi::Tick() {
    m_gameClient->m_pControls->m_InputDirectionLeft = 0;
    m_gameClient->m_pControls->m_InputDirectionRight = 0;
    //m_gameClient->m_pControls->m_InputData.m_Fire = !m_gameClient->m_pControls->m_InputData.m_Fire;
//    m_gameClient->m_pControls->m_MousePos = vec2(100, 0);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        CGameClient::CClientData client = m_gameClient->m_aClients[i];
        if (i == m_gameClient->m_Snap.m_LocalClientID) {
            continue;
        }
        if (client.m_Active) {
            vec2 otherPosition = client.m_Predicted.m_Pos;
            if (otherPosition.x == 0 && otherPosition.y == 0) {
                continue;
            }
            vec2 myPosition = m_gameClient->m_aClients[m_gameClient->m_Snap.m_LocalClientID].m_Predicted.m_Pos;
            vec2 newMousePosition = otherPosition - myPosition;
            vec2 curMousePosition = m_gameClient->m_pControls->m_MousePos;
            m_gameClient->m_pControls->m_MousePos += (newMousePosition - curMousePosition) * 0.05;
            m_gameClient->m_pControls->ClampMousePos();
            if (abs((int) (myPosition.y - otherPosition.y)) > 800 ||
                abs((int) (myPosition.x - otherPosition.x)) > 800 ||
                abs((int) (myPosition.x - otherPosition.x)) < 100) {
                break;
            }
            if (myPosition.x > otherPosition.x) {
                m_gameClient->m_pControls->m_InputDirectionLeft = 1;
            } else if (myPosition.x < otherPosition.x) {
                m_gameClient->m_pControls->m_InputDirectionRight = 1;
            }
            break;
        }
    }
}