// Creates an item in the Editor that allows us to create/place a new Weapon Data Item, mainly for modding

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "WeaponDataFactory.generated.h"

/**
 * 
 */
UCLASS()
class HLDEATHMATCH_API UWeaponDataFactory : public UFactory
{
	GENERATED_BODY()
	
	UWeaponDataFactory(const class FObjectInitializer& OBJ);

protected:
	virtual bool IsMacroFactory() const { return false; }
	virtual bool CanCreateNew() { return true; }
	virtual FText GetDisplayName() const { FText TmpName = FText::FromString("Weapon Data Factory"); return TmpName;}

public:
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};