// Copyright Epic Games, Inc. All Rights Reserved.

#include "HLDeathmatchCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Weapons/BaseFireable.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AHLDeathmatchCharacter

AHLDeathmatchCharacter::AHLDeathmatchCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.0f, 96.0f);

	// Create a CameraComponent
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.0f, 0.0f, 60.0f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	// Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.0f, 0.0f, -150.0f));
}

void AHLDeathmatchCharacter::BeginPlay()
{
	// Call the base class
	Super::BeginPlay();
	if (!IsLocallyControlled() || GetLocalRole() == ROLE_Authority)
	{
		// Initialize server Inventory
		TArray<ABaseFireable*> EmptyChild;
		for (int i = 0; i < 9; i++)
		{
			Inventory.Push(EmptyChild);
		}
		return;
	}

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AHLDeathmatchCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHLDeathmatchCharacter, AuxPower);
	DOREPLIFETIME(AHLDeathmatchCharacter, AuxUsage);
}

//////////////////////////////////////////////////////////////////////////// Input

void AHLDeathmatchCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHLDeathmatchCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AHLDeathmatchCharacter::Look);

		EnhancedInputComponent->BindAction(SprintStartAction, ETriggerEvent::Triggered, this, &AHLDeathmatchCharacter::StartSprint);
		EnhancedInputComponent->BindAction(SprintStopAction, ETriggerEvent::Triggered, this, &AHLDeathmatchCharacter::StopSprint);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AHLDeathmatchCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AHLDeathmatchCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

// Client
void AHLDeathmatchCharacter::StartSprint()
{
	S_StartSprint();
}

void AHLDeathmatchCharacter::StopSprint()
{
	S_StopSprint();
}

void AHLDeathmatchCharacter::StartWalk()
{
	S_StartWalk();
}

void AHLDeathmatchCharacter::StopWalk()
{
	S_StopWalk();
}

// Server
bool AHLDeathmatchCharacter::S_StartSprint_Validate()
{
	return AuxPower <= 100.0f && AuxPower >= 0.0f;
}

void AHLDeathmatchCharacter::S_StartSprint_Implementation()
{
	if (GetCharacterMovement()->IsFalling())
	{
		return;
	}

	if (AuxPower > 0.0f)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		OC_SetSprint();

		AuxUsage += SprintAuxDecay;
		AuxUsage = FMath::Clamp(AuxUsage, 0.0f, 10.0f);

		if (GetWorldTimerManager().IsTimerActive(AuxRegenerationTimer))
		{
			GetWorldTimerManager().ClearTimer(AuxRegenerationTimer);
		}

		if (!GetWorldTimerManager().IsTimerActive(AuxDecayTimer))
		{
			GetWorldTimerManager().SetTimer(AuxDecayTimer, this, &AHLDeathmatchCharacter::S_AuxDecayTimer, AuxDecayTimerRate, false);
		}
	}
}

void AHLDeathmatchCharacter::S_StopSprint_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = NormSpeed;
	OC_StopSprint();

	AuxUsage -= SprintAuxDecay;
	AuxUsage = FMath::Clamp(AuxUsage, 0.0f, 10.0f);

	if (AuxPower < 100.0f)
	{
		if (GetWorldTimerManager().IsTimerActive(AuxDecayTimer))
		{
			GetWorldTimerManager().ClearTimer(AuxDecayTimer);
		}

		if (!GetWorldTimerManager().IsTimerActive(AuxRegenerationTimer))
		{
			GetWorldTimerManager().SetTimer(AuxRegenerationTimer, this, &AHLDeathmatchCharacter::S_AuxRegenTimer, AuxRegenerationTimerRate, false, AuxRegenerationIdleTime);
		}
	}
}

void AHLDeathmatchCharacter::S_StartWalk_Implementation()
{
}

void AHLDeathmatchCharacter::S_StopWalk_Implementation()
{
}

void AHLDeathmatchCharacter::S_AuxDecayTimer()
{
	if (AuxPower == 100.0f)
	{
		AuxPower -= AuxUsage;
	}

	if (AuxPower > 0.0f && AuxPower < 100.0f)
	{
		if (!GetWorldTimerManager().IsTimerActive(AuxRegenerationTimer))
		{
			// If we're not moving, do not decay Aux Power
			if (GetCharacterMovement()->Velocity != FVector::Zero() && !GetCharacterMovement()->IsFalling())
			{
				AuxPower -= AuxUsage;
				AuxPower = FMath::Clamp(AuxPower, 0.0f, 100.0f);
			}

			FString DebugMsg = "Aux Power = " + FString::SanitizeFloat(AuxPower, 2);
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, SrvColor, DebugMsg);

			GetWorldTimerManager().SetTimer(AuxDecayTimer, this, &AHLDeathmatchCharacter::S_AuxDecayTimer, AuxDecayTimerRate, false);
		}
	}
}

void AHLDeathmatchCharacter::S_AuxRegenTimer()
{
	if (AuxPower < 100.0f)
	{
		if (!GetWorldTimerManager().IsTimerActive(AuxDecayTimer))
		{
			AuxPower += AuxRegenerationAmount;
			AuxPower = FMath::Clamp(AuxPower, 0.0f, 100.0f);

			GetWorldTimerManager().SetTimer(AuxRegenerationTimer, this, &AHLDeathmatchCharacter::S_AuxRegenTimer, AuxRegenerationTimerRate, false);

			FString DebugMsg = "Aux Power = " + FString::SanitizeFloat(AuxPower, 2);
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, SrvColor, DebugMsg);
		}
	}
}

// Owning Controllers (Server to Client)
void AHLDeathmatchCharacter::OC_SetSprint_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AHLDeathmatchCharacter::OC_StopSprint_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = NormSpeed;
}

void AHLDeathmatchCharacter::OC_SetWalk_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AHLDeathmatchCharacter::OC_StopWalk_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = NormSpeed;
}

// #TODO
void AHLDeathmatchCharacter::SwapWeapon(int Position)
{
	if (Inventory.Num() == 0)
	{
		return;
	}

	while (Inventory[InventoryRow].Num() == 0)
	{
		InventoryRow++;
	}
}

void AHLDeathmatchCharacter::S_AddWeapon_Implementation(UWeaponDataAsset* WeaponData, AHLDeathmatchCharacter* OverlappedCharacter)
{
	if (this != OverlappedCharacter)
		return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = this;

	FTransform SpawnOrigins;
	SpawnOrigins.SetLocation(FVector::ZeroVector);
	SpawnOrigins.SetRotation(FQuat::MakeFromEuler(FVector::ZeroVector));
	SpawnOrigins.SetScale3D(FVector::OneVector);

	ABaseFireable*			  Weapon = GetWorld()->SpawnActor<ABaseFireable>(ABaseFireable::StaticClass(), SpawnOrigins, SpawnParams);
	AActor*					  LocalPlayer = GetOwner();
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
	Weapon->AttachToComponent(FirstPersonCameraComponent, AttachRules);

	Weapon->SetActorRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	Weapon->AddActorWorldOffset((UKismetMathLibrary::GetForwardVector(FirstPersonCameraComponent->GetComponentRotation()) * 50.0f) + (UKismetMathLibrary::GetRightVector(FirstPersonCameraComponent->GetComponentRotation()) * 25.0f) + (UKismetMathLibrary::GetUpVector(FirstPersonCameraComponent->GetComponentRotation()) * -25.0f));

	Weapon->S_SetWeaponData(WeaponData);
	Inventory[WeaponData->InventorySlot].Push(Weapon);
	Weapon->SetOwner(this);
	MC_AddWeapon(Weapon);
}

//#Todo Fix this. Initially Owner = In camera, Others: In stomach. Currently in camera for everyone.
void AHLDeathmatchCharacter::MC_AddWeapon_Implementation(ABaseFireable* NewWeapon)
{
	if (IsLocallyControlled())
		return;

	ENetRole LclRole = this->GetLocalRole();
	ENetRole WpRole = NewWeapon->GetLocalRole();

	FString Name = GetName();
	switch (WpRole)
	{
		case ROLE_None:
			GEngine->AddOnScreenDebugMessage(-1, 100.0f, MCColor, Name + "None");
			break;
		case ROLE_SimulatedProxy:
			GEngine->AddOnScreenDebugMessage(-1, 100.0f, MCColor, Name + "Sim Proxy");
			break;
		case ROLE_AutonomousProxy:
			GEngine->AddOnScreenDebugMessage(-1, 100.0f, MCColor, Name + "Auto Proxy");
			break;
		case ROLE_Authority:
			GEngine->AddOnScreenDebugMessage(-1, 100.0f, MCColor, Name + "Authority");
			break;
		case ROLE_MAX:
			GEngine->AddOnScreenDebugMessage(-1, 100.0f, MCColor, Name + "Max");
			break;
		default:
			break;
	}

	if (!HasAuthority())
	{
		FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
		NewWeapon->AttachToComponent(RootComponent, AttachRules);
		NewWeapon->AddActorWorldRotation(FRotator(0.0f, -90.0f, 0.0f));
		NewWeapon->AddActorWorldOffset(FVector(-50.0f, 0.0f, 0.0f));
	}
}