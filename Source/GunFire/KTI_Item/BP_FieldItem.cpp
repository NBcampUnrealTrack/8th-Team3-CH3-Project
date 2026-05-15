// Fill out your copyright notice in the Description page of Project Settings.


#include "BP_FieldItem.h"

// Sets default values
ABP_FieldItem::ABP_FieldItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABP_FieldItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABP_FieldItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

