#include "WeaponDataFactory.h"
#include "../Weapons/WeaponDataAsset.h"

UWeaponDataFactory::UWeaponDataFactory(const class FObjectInitializer& OBJ)
	: Super(OBJ)
{
	SupportedClass = UWeaponDataAsset::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
	
}

UObject* UWeaponDataFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	check(InClass->IsChildOf(UWeaponDataAsset::StaticClass()));
	UWeaponDataAsset* NewAsset = NewObject<UWeaponDataAsset>(InParent, InClass, InName, Flags);
	return NewAsset;
}