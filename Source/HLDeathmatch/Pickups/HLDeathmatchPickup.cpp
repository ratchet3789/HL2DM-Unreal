#include "HLDeathmatchPickup.h"
#include "Components/SphereComponent.h"

// Sets default values
AHLDeathmatchPickup::AHLDeathmatchPickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshObject = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticObject"));
	SetRootComponent(MeshObject);

	FallbackSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FallbackSkeletonMdl"));
	FallbackSkeletalMesh->SetupAttachment(MeshObject);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	OverlapSphere->SetupAttachment(FallbackSkeletalMesh);

	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AHLDeathmatchPickup::OnOverlap);

	bReplicates = true;
	SetReplicateMovement(true);
	MeshObject->SetIsReplicated(true);
	FallbackSkeletalMesh->SetIsReplicated(true);
	OverlapSphere->SetIsReplicated(true);
}

// Called when the game starts or when spawned
void AHLDeathmatchPickup::BeginPlay()
{
	Super::BeginPlay();
}

void AHLDeathmatchPickup::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Pickup(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AHLDeathmatchPickup::Pickup(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Overlapped Object"));
	Destroy(true, false);
}
