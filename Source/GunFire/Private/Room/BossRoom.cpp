#include "Room/BossRoom.h"

#include "Interactables/Portal.h"

ABossRoom::ABossRoom()
{
    RoomType = ERoomType::Boss;
    Initialize();
}

void ABossRoom::ActivateResultPortal()
{
    if (IsValid(ResultPortal))
    {
        ResultPortal->SetActive(true);
    }
}
