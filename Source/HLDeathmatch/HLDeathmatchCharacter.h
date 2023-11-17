#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapons/WeaponDataAsset.h"
#include "Weapons/BaseFireable.h"
#include "Net/UnrealNetwork.h"
#include "Components/ChildActorComponent.h"
#include "HLDeathmatchCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class AHLDeathmatchCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintStartAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintStopAction;

	// Swap Weapon Input Interactions (RIP)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapWeaponOne;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapWeaponTwo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapWeaponThree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapWeaponFour;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapWeaponFive;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapWeaponSix;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapWeaponSeven;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapWeaponEight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapWeaponNine;

	// Controller support
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapWeaponLeft;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapWeaponRight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapWeaponUp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapWeaponDown;

#pragma region Client Vars
	const float WalkSpeed = 250.0f;
	const float NormSpeed = 600.0f;
	const float SprintSpeed = 1200.0f;
	const float SprintAuxDecay = 0.2f;
	const float AuxRegenerationAmount = 0.1f;
	const float AuxDecayTimerRate = 0.1f;
	const float AuxRegenerationTimerRate = 0.05f;
	const float AuxRegenerationIdleTime = 5.0f;

	// #Todo
	// Readable Inventory positions for weapons as HL2DMs weapon system is a 2d array
	int InventoryRow = 0;
	int InventoryColumn = 0;

public:
	// Blueprint Getters just in case.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aux|Client")
	FORCEINLINE float GetAuxRegenerationAmount() { return AuxRegenerationAmount; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aux|Client")
	FORCEINLINE float GetAuxDecayTimerRate() { return AuxDecayTimerRate; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aux|Client")
	FORCEINLINE float GetAuxRegenerationTimerRate() { return AuxRegenerationTimerRate; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aux|Client")
	// Time to wait after Aux has stopped draining before refilling
	FORCEINLINE float GetAuxEmptyRegenerationIdleTime() { return AuxRegenerationIdleTime; }
#pragma endregion

// Note: Server Variables exist on both, but should only be modified by the server.
// This enforces server verification.
#pragma region Server Vars
private:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Aux|Server", meta = (AllowPrivateAccess = "true"))
	float AuxPower = 100.0f;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Aux|Server", meta = (AllowPrivateAccess = "true"))
	float AuxUsage = 0.0f;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Player|Inventory|Server", meta = (AllowPrivateAccess = "true"))
	// Map containing the Ammo Type and Count ("PistolAmmo", 40)
	TArray<UWeaponDataAsset*> AmmoStored;

	FTimerHandle AuxDecayTimer;
	FTimerHandle AuxRegenerationTimer;

public:
	FORCEINLINE TArray<UWeaponDataAsset*> GetAmmoStorage() {return AmmoStored;}

	void AddAmmoToStorage(UWeaponDataAsset* Ammo, int& Index) { Index = AmmoStored.Add(Ammo);}
#pragma endregion

public:
	AHLDeathmatchCharacter();

protected:
	virtual void BeginPlay();

#pragma warning(disable : 4263) // Member function does not override any base class virtual member function (Aka, Get outta here C++)
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps);
#pragma warning(default : 4263)

public:
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	TArray<TArray<ABaseFireable*>> Inventory; // Key -> 1 - 9 on Keyboard. // Pair - Inventory Item Slot

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	// Client Functions
	UFUNCTION()
	void StartSprint();

	UFUNCTION()
	void StopSprint();

	UFUNCTION()
	void StartWalk();

	UFUNCTION()
	void StopWalk();

	UFUNCTION(BlueprintCallable)
	void SwapWeapon(int Position);

	// Client -> Server RPCs
	UFUNCTION(Server, Unreliable, WithValidation)
	void S_StartSprint();

	UFUNCTION(Server, Unreliable)
	void S_StopSprint();

	UFUNCTION(Server, Unreliable)
	void S_StartWalk();

	UFUNCTION(Server, Unreliable)
	void S_StopWalk();

	UFUNCTION(Server, Unreliable)
	void S_AddWeapon(UWeaponDataAsset* WeaponData, AHLDeathmatchCharacter* OverlappedCharacter);

	// Server Exclusive
	UFUNCTION()
	void S_AuxDecayTimer();

	UFUNCTION()
	void S_AuxRegenTimer();

	// Server -> Everyone
	UFUNCTION(NetMulticast, Unreliable)
	void MC_AddWeapon(ABaseFireable* NewWeapon);

	// Server -> Owning Client
	// Note: The client is not a fan of the server setting the players move speed without also setting it for the server.
	UFUNCTION(Client, Unreliable)
	void OC_SetSprint();
	
	UFUNCTION(Client, Unreliable)
	void OC_StopSprint();
	
	UFUNCTION(Client, Unreliable)
	void OC_SetWalk();
	
	UFUNCTION(Client, Unreliable)
	void OC_StopWalk();
	
	// Debug Values (Only used here #Todo Delete)
	FColor SrvColor = FColor::Green;
	FColor ClientColor = FColor::Blue;
	FColor MCColor = FColor::Magenta;

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetCamera() const { return FirstPersonCameraComponent; }
};