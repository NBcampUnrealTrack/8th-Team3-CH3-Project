#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyBase.generated.h"

// 델리게이트에서 EnemyBase 포인터를 넘기기 위한 전방선언
class AEnemyBase;
class UStaticMeshComponent;

// Delegate/Event 방식으로 몬스터 사망시 Room 에 알리기 위함
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDied, AEnemyBase*, DeadEnemy);

// 작동 확인용 임시 클래스
UCLASS()
class GUNFIRE_API AEnemyBase : public AActor
{
	GENERATED_BODY()

public:
	AEnemyBase();

    // 델리게이트 타입의 변수 생성
    UPROPERTY(BlueprintAssignable, Category = "Enemy")
    FOnEnemyDied OnEnemyDead;

    UFUNCTION()
    void Die();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Component")
    TObjectPtr<UStaticMeshComponent> StaticMesh;

    bool bDead;
};
