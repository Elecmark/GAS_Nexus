// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "NexusGameplayAbility.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EAbilityInputID : uint8
{
	None UMETA(DisplayName = "None"), // 0
	PrimaryAbility UMETA(DisplayName = "Primary Ability"), // 1
	SecondaryAbility UMETA(DisplayName = "Secondary Ability"), // 2
	DefensiveAbility UMETA(DisplayName = "Defensive Ability"), // 3 
	MovementAbility UMETA(DisplayName = "Movement Ability") // 4
};
UCLASS()
class GAS_NEXUS_API UNexusGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()


public:
	UNexusGameplayAbility();
	/*
	 * Variables Delcarations
	 */

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbilitiesProperties")
	bool bShouldShowInAbilitiesBar = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	EAbilityInputID AbilityInputID = EAbilityInputID::None;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities")
	bool HasPC() const;
};
