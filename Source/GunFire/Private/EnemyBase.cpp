#include "EnemyBase.h"

#include "Components/StaticMeshComponent.h"

AEnemyBase::AEnemyBase()
{
    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    SetRootComponent(StaticMesh);

    bDead = false;
}

void AEnemyBase::Die()
{
    // 중복 호출 방지용
    if (bDead) return;

    bDead = true;

    UE_LOG(LogTemp, Warning, TEXT("Enemy Died!"));

    // 바인딩한 이벤트들에게 Broadcast
    // 바인딩한 함수들을 처리하고 뒤의 Destroy 호출함
    OnEnemyDead.Broadcast(this);
    Destroy();
}
