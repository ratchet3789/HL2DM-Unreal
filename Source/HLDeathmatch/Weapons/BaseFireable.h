#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponDataAsset.h"
#include "BaseFireable.generated.h"

UCLASS()
class HLDEATHMATCH_API ABaseFireable : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WeaponStats",meta=(AllowPrivateAccess=true))
	UWeaponDataAsset* WeaponData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WeaponStats",meta=(AllowPrivateAccess=true))
	TObjectPtr<class USkeletalMeshComponent> WeaponMdl;
	
	int Ammo;
	int AltAmmo;

public:	
	// Sets default values for this actor's properties
	ABaseFireable();

	UFUNCTION(Server, Unreliable)
	void S_SetWeaponData(UWeaponDataAsset* NewWeaponData);

	UFUNCTION(NetMulticast, Unreliable)
	void MC_SetWeaponData(USkeletalMesh* NewWeaponMdl);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category="Weapon", meta=(DisplayName="Fire"))
	void Fire(class UCameraComponent* PlayerCamera);

	UFUNCTION(BlueprintCallable, Category="Weapon", meta=(DisplayName="Fire"))
	void AltFire(class UCameraComponent* PlayerCamera);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void S_Fire(class UCameraComponent* PlayerCamera);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void S_AltFire(class UCameraComponent* PlayerCamera);

	UFUNCTION(NetMulticast, Reliable)
	void MC_FireDebug(FVector Start, FVector End);

	void Drop();
};
