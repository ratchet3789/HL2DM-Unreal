// Note: Still VERY WIP.

#include "BaseFireable.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/EngineTypes.h"
#include "Kismet/KismetMathLibrary.h"

ABaseFireable::ABaseFireable()
{
	PrimaryActorTick.bCanEverTick = false; // Optimisation. But also we're running most things on timers.

	WeaponMdl = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMdl"));
	WeaponMdl->SetIsReplicated(true);
	RootComponent = WeaponMdl;

	bReplicates = true;
}

// Called when the game starts or when spawned
void ABaseFireable::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseFireable::S_SetWeaponData_Implementation(UWeaponDataAsset* NewWeaponData)
{
	WeaponData = NewWeaponData;
	WeaponMdl->SetSkeletalMeshAsset(WeaponData->WeaponMdl);

	MC_SetWeaponData(NewWeaponData->WeaponMdl);
};

void ABaseFireable::MC_SetWeaponData_Implementation(USkeletalMesh* NewWeaponMdl)
{
	WeaponMdl->SetSkeletalMeshAsset(NewWeaponMdl);
}

void ABaseFireable::Fire(UCameraComponent* PlayerCamera)
{
	S_Fire(PlayerCamera);
}

bool ABaseFireable::S_Fire_Validate(UCameraComponent* PlayerCamera)
{
	// What did you do?!
	if (PlayerCamera == nullptr)
		return false;

	// Make sure we have ammo and that we don't have more than the max ammo
	return (Ammo >= 0 && Ammo <= WeaponData->MaxActiveAmmo) || WeaponData->bIsMelee;
}

void ABaseFireable::S_Fire_Implementation(UCameraComponent* PlayerCamera)
{
	FVector CamStart = PlayerCamera->GetComponentLocation();
	FVector ForwardVector = UKismetMathLibrary::GetForwardVector(PlayerCamera->GetComponentRotation());
	FVector CamEnd = CamStart + (ForwardVector * WeaponData->MaxFireDistance);

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, CamStart, CamEnd, ECollisionChannel::ECC_MAX);

	// Debug the trace end, which should return even if you don't hit anything.
	MC_FireDebug(CamStart, HitResult.TraceEnd);
}

void ABaseFireable::AltFire(UCameraComponent* PlayerCamera)
{
	S_AltFire(PlayerCamera);
}

bool ABaseFireable::S_AltFire_Validate(UCameraComponent* PlayerCamera)
{
	// What did you do?!
	if (PlayerCamera == nullptr)
		return false;

	// Note: if our alt ammo is -1 then we don't have an alt fire.
	return ((WeaponData->MaxCarriedAltAmmo != -1 && AltAmmo > 0) && AltAmmo <= WeaponData->MaxCarriedAltAmmo) || WeaponData->bIsMelee;
}

void ABaseFireable::S_AltFire_Implementation(UCameraComponent* PlayerCamera)
{
}

void ABaseFireable::MC_FireDebug_Implementation(FVector Start, FVector End)
{
	DrawDebugLine(GetWorld(), Start, End, FColor::Magenta, false, 5.0f, 0, 1.0f);
}