#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Texture2D.h"
#include "WeaponDataAsset.generated.h"

// Stores weapon data for each weapon.
// #Todo: Weapon Timers for firing and reloading.
UCLASS()
class HLDEATHMATCH_API UWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "WeaponStats")
	// The printed title of the weapon (Weapon Name)
	FText WeaponName;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "WeaponStats")
	// Is this a melee weapon (ignores Ammo values)
	bool bIsMelee;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "WeaponStats")
	FString MeleeReadme = "If melee is true ammo and magazine data is ignored. You can set it to -1 for safety";

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "WeaponStats")
	// The amount of damage applied if hit anywhere but the head
	int BodyDamage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "WeaponStats")
	// The amount of damage applied if hit the head
	int HeadDamage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "WeaponStats")
	// The maximum ammo that can be in a magazine (Eg 1 for the crossbow)
	int MaxActiveAmmo;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "WeaponStats")
	// The maximum distance of a raycast/linetrace/hitscan
	int MaxFireDistance;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "WeaponStats")
	int InventorySlot;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "WeaponStats")
	// The actual model
	USkeletalMesh* WeaponMdl;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "WeaponStats")
	UStaticMesh* AmmoMdl;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "WeaponStats")
	UStaticMesh* WeaponMdlStatic;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Ammo")
	// The image that shows up when the player overlaps the item
	UTexture2D* PickupImage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Ammo")
	// The Ammo name, this is stored in the players.
	FName AmmoDisplayName;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Ammo")
	// The Ammo name, this can be unique or a duplicate (untested)
	FName AltAmmoDisplayName;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Ammo")
	// Maximum amount of ammo that can be stored
	int MaxCarriedAmmo;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Ammo")
	// Maximum amount of alt-item ammo that can be stored
	int MaxCarriedAltAmmo;

private:
	int ActiveAmmo = 0;
	int AltAmmo = 0;

	int CarriedAmmo = 0;
	int CarriedAltAmmo = 0;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ammo")
	int GetActiveAmmo() { return ActiveAmmo; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ammo")
	int GetAltAmmo() { return AltAmmo; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ammo")
	int GetCarriedAmmo() { return CarriedAmmo; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ammo")
	int GetCarriedAltAmmo() { return CarriedAltAmmo; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ammo")
	int GetMaxCarriedAmmo() { return MaxCarriedAmmo; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ammo")
	int GetMaxCarriedAltAmmo() { return MaxCarriedAltAmmo; }

	void AddAmmo(int AmmoToAdd)
	{
		if (CarriedAmmo + AmmoToAdd > MaxCarriedAmmo)
		{
			CarriedAmmo = MaxCarriedAmmo;
			return;
		}

		CarriedAmmo += AmmoToAdd;
	}
	void AddAltAmmo(int AmmoToAdd)
	{
		if (CarriedAltAmmo + AmmoToAdd > MaxCarriedAltAmmo)
		{
			CarriedAltAmmo = MaxCarriedAltAmmo;
			return;
		}

		CarriedAltAmmo += AmmoToAdd;
	}
	bool IsAmmoFull() { return CarriedAmmo == MaxCarriedAmmo; }
	bool IsAltAmmoFull() { return CarriedAltAmmo == MaxCarriedAltAmmo; }
};
