#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Room/RoomTypes.h"
#include "GunFireGameInstance.generated.h"


// 어떤 상태로 결과창에 진입할지
UENUM(BlueprintType)
enum class ESessionResult : uint8
{
    None        UMETA(DisplayName = "진행 중"),
    Victory     UMETA(DisplayName = "승리"),
    Death       UMETA(DisplayName = "사망"),
    Quit        UMETA(DisplayName = "종료")
};

// 한 세션에서 저장할 플레이어 데이터
USTRUCT(BlueprintType)
struct FPlayerSessionData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    float CurrentHealth = 100.f;

    UPROPERTY(BlueprintReadWrite)
    float MaxHealth = 100.f;

    UPROPERTY(BlueprintReadWrite)
    int32 Gold = 0;

    UPROPERTY(BlueprintReadWrite)
    TArray<FName> RelicIDs;
};

// 방 정보 구조체
USTRUCT(BlueprintType)
struct FRoomData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    int32 Floor = 1;

    UPROPERTY(BlueprintReadWrite)
    FName RoomID = NAME_None;

    UPROPERTY(BlueprintReadWrite)
    ERoomType RoomType = ERoomType::Safe;
};


UCLASS()
class GUNFIRE_API UGunFireGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
    UGunFireGameInstance();

    // 세션 시작
    UFUNCTION(BlueprintCallable, Category = "Session")
    void StartNewSession();

    // 세션 종료
    UFUNCTION(BlueprintCallable, Category = "Session")
    void FinishSession(ESessionResult Result);

    // 세션정보 초기화
    UFUNCTION(BlueprintCallable, Category = "Session")
    void ResetSessionData();

    // 지나온 방의 정보 기록
    UFUNCTION(BlueprintCallable, Category = "Session")
    void AddRoomData(const FRoomData& Data);

    // 처치한 적 수 기록
    UFUNCTION(BlueprintCallable, Category = "Session")
    void AddKilledEnemyCount(int32 Count);

    UFUNCTION(BlueprintCallable, Category = "Session")
    void MoveNextFloor();


    // 게터
    UFUNCTION(BlueprintPure, Category = "Session")
    const FPlayerSessionData& GetPlayerSessionData() const;

    UFUNCTION(BlueprintPure, Category = "Session")
    int32 GetCurrentFloor() const;

    UFUNCTION(BlueprintPure, Category = "Session")
    ESessionResult GetSessionResult() const;


    // 세터
    UFUNCTION(BlueprintCallable, Category = "Session")
    void SetPlayerSessionData(const FPlayerSessionData& SessionData);

    UFUNCTION(BlueprintCallable, Category = "Session")
    void SetCurrentFloor(int32 Floor);


protected:
    // 저장할 플레이어 데이터
    UPROPERTY(BlueprintReadOnly, Category = "Session")
    FPlayerSessionData PlayerSessionData;

    // 현재 층
    UPROPERTY(BlueprintReadOnly, Category = "Session")
    int32 CurrentFloor;

    // 세션 상태, 결과창에 반영할 상태
    UPROPERTY(BlueprintReadOnly, Category = "Session")
    ESessionResult SessionResult;

    // 지나온 방들의 정보
    UPROPERTY(BlueprintReadOnly, Category = "Session")
    TArray<FRoomData> RoomData;

    // 클리어한 방 갯수
    UPROPERTY(BlueprintReadOnly, Category = "Session")
    int32 ClearedRoomCount;

    // 처치한 적의 수
    UPROPERTY(BlueprintReadOnly, Category = "Session")
    int32 KilledEnemyCount;
};
