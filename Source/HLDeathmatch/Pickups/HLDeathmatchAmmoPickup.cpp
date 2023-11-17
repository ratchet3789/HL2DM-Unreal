#include "HLDeathmatchAmmoPickup.h"
#include "Components/SkeletalMeshComponent.h"

AHLDeathmatchAmmoPickup::AHLDeathmatchAmmoPickup()
{
}

void AHLDeathmatchAmmoPickup::BeginPlay()
{
	Super::BeginPlay();

	if (WeaponData == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Blue, TEXT("Weapon Data for this item is invalid. Drawing arrow above object."));
		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation() + GetActorLocation().UpVector * 500.0f, GetActorLocation(), 500.0f, FColor::Red, true, 10.0f, SDPG_Foreground, 2.0f);
		return;
	}

	if (PickupAmmoCount <= 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Blue, TEXT("Weapon Ammo is -1."));
		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation() + GetActorLocation().UpVector * 500.0f, GetActorLocation(), 500.0f, FColor::Blue, true, 10.0f, SDPG_Foreground, 2.0f);
	}
}

void AHLDeathmatchAmmoPickup::EditorUpdateMeshes()
{
#if WITH_EDITOR
	if (WeaponData == nullptr)
	{
		return;
	}

	if (bAlsoPicksUpWeapon)
	{
		if (WeaponData->WeaponMdlStatic != nullptr)
		{
			MeshObject->SetStaticMesh(WeaponData->WeaponMdlStatic);
			FallbackSkeletalMesh->SetSkeletalMeshAsset(NULL);
		}
		else if (WeaponData->WeaponMdl != nullptr)
		{
			FallbackSkeletalMesh->SetSkeletalMeshAsset(WeaponData->WeaponMdl);
			MeshObject->SetStaticMesh(NULL);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Blue, TEXT("No Weapon Model, defaulting to Cube"));
		}
	}
	else
	{
		if (WeaponData->AmmoMdl != nullptr)
		{
			MeshObject->SetStaticMesh(WeaponData->AmmoMdl);
			FallbackSkeletalMesh->SetSkeletalMeshAsset(NULL);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Blue, TEXT("No Ammo Model, defaulting to Cube"));
		}
	}
#endif // WITH_EDITOR
}

void AHLDeathmatchAmmoPickup::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	EditorUpdateMeshes();
}

void AHLDeathmatchAmmoPickup::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	EditorUpdateMeshes();
}

bool AHLDeathmatchAmmoPickup::CanPickup(AHLDeathmatchCharacter* PickupActor, UWeaponDataAsset*& PlayerWeaponAsset)
{
	int ItemIndex = PickupActor->GetAmmoStorage().Find(WeaponData);

	// We didn't find an item, lets add it!
	if (ItemIndex <= INDEX_NONE)
	{
		PickupActor->AddAmmoToStorage(WeaponData, ItemIndex);
		// Should always be the last but I don't want to risk it
		PlayerWeaponAsset = PickupActor->GetAmmoStorage()[ItemIndex];
		return true;
	}

	UWeaponDataAsset* TempWeaponData = PickupActor->GetAmmoStorage()[ItemIndex];

	// We already have max ammo!
	if (TempWeaponData->IsAmmoFull())
	{
		// We don't need our Weapon Asset here as we're not doing anything with it. So we're playing it safe.
		PlayerWeaponAsset = nullptr;
		return false;
	}

	// We need more ammo
	PlayerWeaponAsset = PickupActor->GetAmmoStorage()[ItemIndex];

	return true;
}

void AHLDeathmatchAmmoPickup::Pickup(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (WeaponData == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Blue, TEXT("Weapon Data for this item is invalid. Drawing arrow above object."));
		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation() + GetActorLocation().UpVector * 500.0f, GetActorLocation(), 500.0f, FColor::Red, true, 10.0f, SDPG_Foreground, 2.0f);
		return;
	}

	AHLDeathmatchCharacter* OverlappingCharacter = Cast<AHLDeathmatchCharacter>(OtherActor);
	if (OverlappingCharacter == nullptr)
		return;

	if (!OverlappingCharacter->IsLocallyControlled())
		return;

	UWeaponDataAsset* InventoryWeapon;
	if (!CanPickup(OverlappingCharacter, InventoryWeapon))
		return;

	InventoryWeapon->AddAmmo(PickupAmmoCount);

	if (bAlsoPicksUpWeapon)
	{
		OverlappingCharacter->S_AddWeapon(InventoryWeapon, OverlappingCharacter);
	}

	Destroy(true);
}