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
#include "../../../engine/client.h"
#include "../../mapitems.h"

CAi::CAi() {
    this->currentStrategy = SEARCH;
    this->m_followClientId = -1;
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
//    return false;
    return true;
}

bool CAi::OnMouseMove(float x, float y) {
    // manual mouse input will not be processed
//    return false;
    return true;
}

void CAi::Tick() {
    if (Client()->State() != IClient::STATE_ONLINE) {
        return;
    }
    switch (this->currentStrategy) {
        case SEARCH:
            this->strategySearch();
            break;
        case ATTACK:
            this->strategyAttack();
            break;
        case ESCAPE:
            this->strategyEscape();
            break;
    }
    /*
//    return;
    static int64 LastHookTime = 0;
    m_gameClient->m_pControls->m_InputDirectionLeft = 0;
    m_gameClient->m_pControls->m_InputDirectionRight = 0;
    m_gameClient->m_pControls->m_InputData.m_Jump = 0;
    //m_gameClient->m_pControls->m_InputData.m_Fire = !m_gameClient->m_pControls->m_InputData.m_Fire;
//    m_gameClient->m_pControls->m_InputData.m_Hook = 1;
    if (time_get() > LastHookTime + 1000000) {
        LastHookTime = time_get();
        dbg_msg("ai", "no hook");
        m_gameClient->m_pControls->m_InputData.m_Hook = 0;
    }

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
    vec2 distance = otherPos - myPos;
    if (abs((int) (myPos.y - otherPos.y)) > 800 ||
        abs((int) (myPos.x - otherPos.x)) > 800 ||
        abs((int) (distance.x * distance.x + distance.y * distance.y)) < 50*50) {
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
    if (m_gameClient->m_pControls->m_InputDirectionRight) {
        if (characterCore.m_pCollision->CheckPoint(characterCore.m_Pos.x+25, characterCore.m_Pos.y)) {
            m_gameClient->m_pControls->m_InputData.m_Jump = 1;
        }
    } else if (m_gameClient->m_pControls->m_InputDirectionLeft) {
        if (characterCore.m_pCollision->CheckPoint(characterCore.m_Pos.x-25, characterCore.m_Pos.y)) {
            m_gameClient->m_pControls->m_InputData.m_Jump = 1;
        }
    }
     */
}

void CAi::strategySearch() {
    static int walkDirection = -1;
    CMapItemLayerTilemap *gameLayer = (CMapItemLayerTilemap *) m_gameClient->Layers()->GameLayer();
    CTile *pTiles = (CTile *) m_gameClient->Layers()->Map()->GetData(gameLayer->m_Data);
    for (int i = 0; i < gameLayer->m_Width * gameLayer->m_Height; i++) {
        if (i < gameLayer->m_Width) {
            continue;
        }
        CTile aboveTile = pTiles[i - gameLayer->m_Width];
        bool walkableTile = (pTiles[i].m_Index == TILE_SOLID || pTiles[i].m_Index == TILE_NOHOOK || pTiles[i].m_Index == 5) &&
                            (aboveTile.m_Index == TILE_AIR || (aboveTile.m_Index >= 192 && aboveTile.m_Index <= 200));
        // 5 seems to be the actual nohook tile
        // 192 .. 200 seem to be the entity tiles (health, armor, weapons...). Dont know why ENTITIY_... doesnt work.
        if (walkableTile) {

        }
    }
    if (walkDirection == -1) {
//        m_gameClient->m_PredictedChar
        m_gameClient->m_pControls->m_InputDirectionLeft = 1;
        m_gameClient->m_pControls->m_InputDirectionRight = 0;
    } else if (walkDirection == 1) {
        m_gameClient->m_pControls->m_InputDirectionLeft = 0;
        m_gameClient->m_pControls->m_InputDirectionRight = 1;
    }
    else {
        m_gameClient->m_pControls->m_InputDirectionLeft = 0;
        m_gameClient->m_pControls->m_InputDirectionRight = 0;
    }
}

void CAi::strategyAttack() {
}

void CAi::strategyEscape() {
}

//void CAi::walkable(int ti)