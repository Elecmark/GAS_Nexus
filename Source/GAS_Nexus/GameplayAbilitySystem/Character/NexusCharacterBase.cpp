// Fill out your copyright notice in the Description page of Project Settings.


#include "NexusCharacterBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS_Nexus/GameplayAbilitySystem/AbilitySystemComponent/Ability/NexusGameplayAbility.h"

// Sets default values
ANexusCharacterBase::ANexusCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Create Ability System Component
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(AscReplicationMode);

	// Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(35.f, 90.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking= 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling= 1500.0f;

	// Add the Basic Attribute Set
	BasicAttributeSet = CreateDefaultSubobject<UBasicAttributeSet>(TEXT("BasicAttributeSet"));
}

// Called when the game starts or when spawned
void ANexusCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANexusCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANexusCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANexusCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RefreshAbilityActorInfo();
	}
}

void ANexusCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RefreshAbilityActorInfo();
	}
}

TArray<FGameplayAbilitySpecHandle> ANexusCharacterBase::GrantAbilities(TArray<TSubclassOf<UGameplayAbility>> AbilitiesToGrant)
{
	
	// if the ability system component is not valid we return an empty array 
	if (!AbilitySystemComponent || !HasAuthority())
	{
		return TArray<FGameplayAbilitySpecHandle>();
	}

	//Second Create an empty TArray of type  FGameplayAbilitySpecHandle so we can return the TArray
	TArray<FGameplayAbilitySpecHandle> AbilityHandles;
	
	for (TSubclassOf<UGameplayAbility> Ability : AbilitiesToGrant)
	{
		int32 InputID = -1;
		if (const UNexusGameplayAbility* NexusAbilityCDO = GetDefault<UNexusGameplayAbility>(Ability))
		{
			InputID = static_cast<int32>(NexusAbilityCDO->AbilityInputID);
		}
		//first loop over all the abilities we give this function and grant them one by one.
		FGameplayAbilitySpecHandle SpecHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(
			Ability, 1, InputID, this));

		AbilityHandles.Add(SpecHandle);
	}
	//After Grating all abilities we let UI know before returning.  
	SendAbilitiesChangedEvent();
	return AbilityHandles;
}

void ANexusCharacterBase::RemoveAbilities(TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove)
{
	if (!AbilitySystemComponent || !HasAuthority())
	{
		return;
	}

	//ToRemove we just loop over all abilities handles and remove them 
	for (FGameplayAbilitySpecHandle AbilityHandle : AbilitiesToRemove)
	{
		AbilitySystemComponent->ClearAbility(AbilityHandle);
	}
	//After removing all abilities we let UI know 
	
	SendAbilitiesChangedEvent();
}

void ANexusCharacterBase::SendAbilitiesChangedEvent()
{
	//To Let the UI know that we have Changed abilities by granting them or removing them we need to call this function.
	FGameplayEventData EventData;
	EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("Event.Abilities.Changed"));
	EventData.Instigator = this;
	EventData.Target = this;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventData.EventTag, EventData);
}

void ANexusCharacterBase::Server_SendGameplayEventToSelf_Implementation(FGameplayEventData EventData)
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventData.EventTag, EventData);
}

UAbilitySystemComponent* ANexusCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}