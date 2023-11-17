#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HLDeathmatchPickup.generated.h"

UCLASS()
class HLDEATHMATCH_API AHLDeathmatchPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHLDeathmatchPickup();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<class USphereComponent> OverlapSphere;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<class UStaticMeshComponent> MeshObject;

	UPROPERTY(BlueprintReadOnly, EditInstanceOnly)
	// This is the fallback if the weapon mesh static is null.
	TObjectPtr<class USkeletalMeshComponent> FallbackSkeletalMesh;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Called from OnOverlap. Allows for override on child classes, because Unreal doesn't allow UFUNCTION overrides for dynamically bound delegates :(
	virtual void Pickup(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};