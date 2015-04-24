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
#include <string.h>

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

void CAi::OnMessage(int MsgType, void *pRawMsg) {
    if (MsgType == NETMSGTYPE_SV_CHAT) {
        CNetMsg_Sv_Chat *pMsg = (CNetMsg_Sv_Chat *) pRawMsg;
        char *clientName = m_gameClient->m_aClients[m_gameClient->m_Snap.m_LocalClientID].m_aName;
        if (strncmp(pMsg->m_pMessage, clientName, strlen(clientName)) == 0) {
            // + 2 since people talk to us like "Name: COMMAND"
            char const *cmd = pMsg->m_pMessage + strlen(clientName) + 2;
            if (strcmp(cmd, "follow me") == 0 && this->m_followClientId == -1) {
                this->m_followClientId = pMsg->m_ClientID;
            } else if (strcmp(cmd, "stop follow me") == 0 && this->m_followClientId == pMsg->m_ClientID) {
                this->m_followClientId = -1;
            }
            dbg_msg("ai", cmd);
        }
        //AddLine(pMsg->m_ClientID, pMsg->m_Team, pMsg->m_pMessage);
    }
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
    m_gameClient->m_pControls->m_InputData.m_Jump = 0;
    //m_gameClient->m_pControls->m_InputData.m_Fire = !m_gameClient->m_pControls->m_InputData.m_Fire;
//    m_gameClient->m_pControls->m_MousePos = vec2(100, 0);
    if (this->m_followClientId == -1) {
        return;
    }
    CGameClient::CClientData otherClient = m_gameClient->m_aClients[this->m_followClientId];
    if (!otherClient.m_Active) {
        this->m_followClientId = -1;
        return;
    }
    CGameClient::CClientData myClient = m_gameClient->m_aClients[m_gameClient->m_Snap.m_LocalClientID];
    vec2 otherPos = otherClient.m_Predicted.m_Pos;
    if (otherPos.x == 0 && otherPos.y == 0) {
        return;
    }
    vec2 myPos = myClient.m_Predicted.m_Pos;
    vec2 newMousePosition = otherPos - myPos;
    vec2 curMousePosition = m_gameClient->m_pControls->m_MousePos;
    m_gameClient->m_pControls->m_MousePos += (newMousePosition - curMousePosition) * 0.05;
    m_gameClient->m_pControls->ClampMousePos();
    if (abs((int) (myPos.y - otherPos.y)) > 800 ||
        abs((int) (myPos.x - otherPos.x)) > 800 ||
        abs((int) (myPos.x - otherPos.x)) < 100) {
        return;
    }
    if (myPos.x > otherPos.x) {
        m_gameClient->m_pControls->m_InputDirectionLeft = 1;
    } else if (myPos.x < otherPos.x) {
        m_gameClient->m_pControls->m_InputDirectionRight = 1;
    } else {
        return;
    }

    CCharacterCore characterCore = m_gameClient->m_PredictedChar;
    float PhysSize = 28.0f;
    // get ground state
    bool Grounded = false;
    if(characterCore.m_pCollision->CheckPoint(characterCore.m_Pos.x+PhysSize/2, characterCore.m_Pos.y+PhysSize/2+5))
        Grounded = true;
    if(characterCore.m_pCollision->CheckPoint(characterCore.m_Pos.x-PhysSize/2, characterCore.m_Pos.y+PhysSize/2+5))
        Grounded = true;
    if (Grounded) {
        if ((otherPos - myPos).y < -20) {
            m_gameClient->m_pControls->m_InputData.m_Jump = 1;
        }
    } else if (characterCore.m_Vel.y > 0) {
        m_gameClient->m_pControls->m_InputData.m_Jump = 1;
    }
}