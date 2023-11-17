// Copyright Epic Games, Inc. All Rights Reserved.

#include "HLDeathmatchGameMode.h"
#include "HLDeathmatchCharacter.h"
#include "UObject/ConstructorHelpers.h"

AHLDeathmatchGameMode::AHLDeathmatchGameMode()
	: Super()
{
	// Hard coded awfulness. #Todo: Fix
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
