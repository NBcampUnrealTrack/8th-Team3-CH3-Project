#pragma once

#include "CoreMinimal.h"
#include "RoomTypes.generated.h"


// 방의 타입 : 전투, 함정, 보물방
UENUM(BlueprintType)
enum class ERoomType : uint8
{
    Combat      UMETA(DisplayName = "전투방"),
    Trap        UMETA(DisplayName = "함정방"),
    Treasure    UMETA(DisplayName = "보물방")
};

// 방의 상태 : 시작 전 대기, 진행 중, 클리어
UENUM(BlueprintType)
enum class ERoomState : uint8
{
    Waiting     UMETA(DisplayName = "대기"),
    InProgress  UMETA(DisplayName = "진행 중"),
    Cleared     UMETA(DisplayName = "클리어")
};
