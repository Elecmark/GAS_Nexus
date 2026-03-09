// Fill out your copyright notice in the Description page of Project Settings.


#include "NexusCharacterBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS_Nexus/GameplayAbilitySystem/AbilitySystemComponent/NexusAbilitySystemComponent.h"
#include "GAS_Nexus/GameplayAbilitySystem/Ability/NexusGameplayAbility.h"

// Sets default values
ANexusCharacterBase::ANexusCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Create Ability System Component
	AbilitySystemComponent = CreateDefaultSubobject<UNexusAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
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
		GrantAbilities(StartingAbilities);
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
	
	// 如果能力系统组件无效，我们就返回一个空数组
	if (!AbilitySystemComponent || !HasAuthority())
	{
		return TArray<FGameplayAbilitySpecHandle>();
	}

	// 第二步，创建一个类型为FGameplayAbilitySpecHandle的空TArray，这样我们就能返回这个TArray了。
	TArray<FGameplayAbilitySpecHandle> AbilityHandles;
	
	for (TSubclassOf<UGameplayAbility> Ability : AbilitiesToGrant)
	{
		int32 InputID = -1;
		// if (const UNexusGameplayAbility* NexusAbilityCDO = GetDefault<UNexusGameplayAbility>(Ability))
		// {
		// 	InputID = static_cast<int32>(NexusAbilityCDO->AbilityInputID);
		// }
		// 首先遍历我们赋予这个函数的所有能力，并逐一授予它们。
		FGameplayAbilitySpecHandle SpecHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(
			Ability, 1, InputID, this));

		AbilityHandles.Add(SpecHandle);
	}
	// 在取消所有权限后，我们会在返回前通知用户界面。
	SendAbilitiesChangedEvent();
	return AbilityHandles;
}

void ANexusCharacterBase::RemoveAbilities(TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove)
{
	if (!AbilitySystemComponent || !HasAuthority())
	{
		return;
	}

	// ToRemove 我们只需遍历所有能力句柄并将其移除
	for (FGameplayAbilitySpecHandle AbilityHandle : AbilitiesToRemove)
	{
		AbilitySystemComponent->ClearAbility(AbilityHandle);
	}
	// 移除所有功能后，我们告知了用户界面。
	
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
 	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventData.EventTag, EventData);
}

UAbilitySystemComponent* ANexusCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}