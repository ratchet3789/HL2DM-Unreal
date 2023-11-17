#pragma once

#include "CoreMinimal.h"
#include "HLDeathmatchPickup.h"
#include "../HLDeathmatchCharacter.h"
#include "../Weapons/WeaponDataAsset.h"
#include "HLDeathmatchAmmoPickup.generated.h"

UCLASS()
class HLDEATHMATCH_API AHLDeathmatchAmmoPickup : public AHLDeathmatchPickup
{
	GENERATED_BODY()

	AHLDeathmatchAmmoPickup();

protected:
	void BeginPlay() override;

	// Both functions trigger in editor to set the correct weapon model.
	void OnConstruction(const FTransform& Transform) override;
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	// Also only called in the editor and wrapped in preprocessors for safety
	void EditorUpdateMeshes();

public:
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Pickup")
	int PickupAmmoCount = -1;

	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Pickup")
	bool bAlsoPicksUpWeapon = false;

	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Pickup")
	UWeaponDataAsset* WeaponData = nullptr;

public:
	bool CanPickup(AHLDeathmatchCharacter* PickupActor, UWeaponDataAsset*& PlayerWeaponAsset);
	void Pickup(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

};
