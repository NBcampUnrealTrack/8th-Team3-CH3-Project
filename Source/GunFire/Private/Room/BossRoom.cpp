#include "Room/BossRoom.h"

#include "GunFire/GunFireGameMode.h"

ABossRoom::ABossRoom()
{
    RoomType = ERoomType::Boss;
    Initialize();
}
