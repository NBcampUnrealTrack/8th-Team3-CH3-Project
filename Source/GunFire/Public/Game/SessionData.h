#pragma once

#include "CoreMinimal.h"
#include "CombatStat.h"
#include "Item/ItemSystemTypes.h"
#include "Weapon/WeaponComponent.h"
#include "SessionData.generated.h"

struct FGF_PassiveItemData;
struct FGF_ActiveItemData;

// 한 세션에서 저장할 플레이어 스탯 정보
USTRUCT(BlueprintType)
struct FPlayerSessionData
{
    GENERATED_BODY()

    // 플레이어 기본 스탯
    UPROPERTY(BlueprintReadWrite)
    FCombatStat BaseStats;

    // 현재 체력
    UPROPERTY(BlueprintReadWrite)
    float CurrentHealth = 100.f;
};

// 한 세션에서 저장할 총기 전용 정보
USTRUCT(BlueprintType)
struct FGunSessionData
{
    GENERATED_BODY()

    // 현재 장전된 총알
    UPROPERTY(BlueprintReadWrite)
    int32 CurrentAmmo = 0;

    // 남은 총알
    UPROPERTY(BlueprintReadWrite)
    int32 RemainAmmo = 0;
};

// 한 세션에서 저장할 장착한 무기 정보
USTRUCT(BlueprintType)
struct FEquippedWeaponSessionData
{
    GENERATED_BODY()

    // 장착된 슬롯
    UPROPERTY(BlueprintReadWrite)
    EWeaponSlot Slot = EWeaponSlot::RightHand;

    // 장착한 무기 클래스
    UPROPERTY(BlueprintReadWrite)
    TSubclassOf<AWeaponBase> WeaponClass = nullptr;

    // 총기 정보인지 확인
    UPROPERTY(BlueprintReadWrite)
    bool bHasGunData = false;

    // 총기 정보
    UPROPERTY(BlueprintReadWrite)
    FGunSessionData GunData;
};

// 한 세션에서 저장할 인벤토리 정보
USTRUCT(BlueprintType)
struct FInventorySessionData
{
    GENERATED_BODY()

    // 보유한 패시브 아이템 배열
    UPROPERTY(BlueprintReadWrite)
    TArray<FGF_PassiveItemData> OwnedPassives;

    // 보유한 액티브 아이템 배열
    UPROPERTY(BlueprintReadWrite)
    TArray<FGF_ActiveItemData> OwnedActives;

    // 보유한 재료 아이템 배열
    UPROPERTY(BlueprintReadWrite)
    TArray<FGF_PassiveItemData> OwnedMaterials;
};
