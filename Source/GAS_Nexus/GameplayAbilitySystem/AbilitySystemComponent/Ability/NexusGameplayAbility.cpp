// Fill out your copyright notice in the Description page of Project Settings.


#include "NexusGameplayAbility.h"

UNexusGameplayAbility::UNexusGameplayAbility()
{
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("GameplayAbility.Active")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
}

bool UNexusGameplayAbility::HasPC() const
{
	const APawn* Pawn = Cast<APawn>(GetAvatarActorFromActorInfo());

	if (!Pawn)
	{
		return false;
	}
	return Pawn->GetController()->IsA<APlayerController>();
}
