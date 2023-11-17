// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../HLDeathmatchCharacter.h"
#include "DBG_NetClient.generated.h"

/**
 *
 */
UCLASS()
class HLDEATHMATCH_API UDBG_NetClient : public UUserWidget
{
	GENERATED_BODY()

	TObjectPtr<AHLDeathmatchCharacter> OwningHLChar = nullptr;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = DebugUI)
	AHLDeathmatchCharacter* GetOwningPawn()
	{
		if (OwningHLChar == nullptr)
		{
			APlayerController* OPC = GetOwningPlayer();
			APawn* OPN = GetOwningPlayer()->GetPawn();
			OwningHLChar = Cast<AHLDeathmatchCharacter>(OPN);
			if (OwningHLChar == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to get Owning Character for this widget"));
				return nullptr;
			}
		}
		return OwningHLChar;
	}

public:
};
