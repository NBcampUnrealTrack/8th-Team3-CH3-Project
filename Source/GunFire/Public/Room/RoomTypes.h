#pragma once

#include "CoreMinimal.h"
#include "RoomTypes.generated.h"


// 방의 타입 : 정비, 전투, 보스
// 이후에 선택 추가 : 함정, 보물방
UENUM(BlueprintType)
enum class ERoomType : uint8
{
    Start       UMETA(DisplayName = "시작방"),
    Combat      UMETA(DisplayName = "전투방"),
    Random      UMETA(DisplayName = "랜덤방"),
    Boss        UMETA(DisplayName = "보스방"),

    Safe        UMETA(DisplayName = "휴식방")
};

// 방의 상태 : 시작 전 대기, 진행 중, 클리어
UENUM(BlueprintType)
enum class ERoomState : uint8
{
    Waiting     UMETA(DisplayName = "대기"),
    InProgress  UMETA(DisplayName = "진행 중"),
    Cleared     UMETA(DisplayName = "클리어")
};
