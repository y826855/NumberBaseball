#include "NBNumberPad.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/Pawn.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBGameplayMessages.h"
#include "Net/UnrealNetwork.h"

ANBNumberPad::ANBNumberPad()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(SceneRoot);
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Trigger->SetGenerateOverlapEvents(true);
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandleBeginOverlap);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &ThisClass::HandleEndOverlap);
}

void ANBNumberPad::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() != NM_DedicatedServer)
	{
		OnPadNumberChanged(HavingNumber);
	}
}

void ANBNumberPad::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bInputSubmitted);
	DOREPLIFETIME(ThisClass, HavingNumber);
	DOREPLIFETIME(ThisClass, bIsPressed);
	DOREPLIFETIME(ThisClass, bIsPadEnabled);
}

void ANBNumberPad::CompletePress()
{
	if (HasAuthority() == false || bIsPadEnabled == false || bInputSubmitted)
	{
		return;
	}

	FNBInputMessage Message;
	Message.Value = HavingNumber;
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		NBGameplayMessages::InputSubmitted,
		Message);

	bInputSubmitted = true;
	OnRep_InputSubmitted();
	ForceNetUpdate();
}

void ANBNumberPad::ResetInput()
{
	if (HasAuthority() == false || bInputSubmitted == false)
	{
		return;
	}

	bInputSubmitted = false;
	OnRep_InputSubmitted();
	ForceNetUpdate();
}

void ANBNumberPad::SetPadEnabled(bool bEnabled)
{
	if (bIsPadEnabled == bEnabled)
	{
		return;
	}

	bIsPadEnabled = bEnabled;
	if (bIsPadEnabled == false)
	{
		SetPressed(false);
	}
	OnRep_IsPadEnabled();
	ForceNetUpdate();
}

void ANBNumberPad::SetPadNumber(int32 Number)
{
	if (HavingNumber == Number)
	{
		return;
	}

	HavingNumber = Number;
	OnRep_HavingNumber();
	ForceNetUpdate();
}

void ANBNumberPad::OnRep_InputSubmitted()
{
	OnInputSubmittedChanged(bInputSubmitted);
}

void ANBNumberPad::OnRep_HavingNumber()
{
	if (GetNetMode() != NM_DedicatedServer)
	{
		OnPadNumberChanged(HavingNumber);
	}
}

void ANBNumberPad::OnRep_IsPadEnabled()
{
	if (bIsPadEnabled)
	{
		OnPadActivated();
	}
}

void ANBNumberPad::SetPressed(bool bPressed)
{
	if (HasAuthority() == false || bIsPressed == bPressed)
	{
		return;
	}

	bIsPressed = bPressed;
	OnRep_IsPressed();
	ForceNetUpdate();
}

void ANBNumberPad::OnRep_IsPressed()
{
	if (bIsPressed)
	{
		OnPressStarted();
	}
	else
	{
		OnPressReleased();
	}
}

void ANBNumberPad::HandleBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (HasAuthority() == false || bIsPadEnabled == false || bInputSubmitted)
	{
		return;
	}

	APawn* Pawn = Cast<APawn>(OtherActor);
	if (IsValid(Pawn) == false || OtherComponent != Pawn->GetRootComponent())
	{
		return;
	}

	SetPressed(true);
}

void ANBNumberPad::HandleEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex)
{
	if (HasAuthority() == false || bIsPadEnabled == false)
	{
		return;
	}

	APawn* Pawn = Cast<APawn>(OtherActor);
	if (IsValid(Pawn) == false || OtherComponent != Pawn->GetRootComponent())
	{
		return;
	}

	SetPressed(false);
}

