// Copyright Epic Games, Inc. All Rights Reserved.

#include "GunFireGameMode.h"
#include "GunFireCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "PlayerCharacter.h"
#include "GunFirePlayerController.h"

AGunFireGameMode::AGunFireGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	//DefaultPawnClass = PlayerPawnClassFinder.Class;
    DefaultPawnClass = APlayerCharacter::StaticClass();
    DefaultPawnClass = AGunFirePlayerController::StaticClass();

}
