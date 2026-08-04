#include "NBPadController.h"

#include "Components/ChildActorComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "GameAnimationSample/NumberBaseball/Core/NBGameState.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBGameplayMessages.h"
#include "NBNumberPad.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogNBPadController, Log, All);

ANBPadController::ANBPadController()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	for (int32 Index = 0; Index < 9; ++Index)
	{
		const FName ComponentName(*FString::Printf(TEXT("Pad%d"), Index + 1));
		UChildActorComponent* PadComponent = CreateDefaultSubobject<UChildActorComponent>(ComponentName);
		PadComponent->SetupAttachment(SceneRoot);
		PadComponents.Add(PadComponent);
	}
}

void ANBPadController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsActive);
}

void ANBPadController::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	for (int32 Index = 0; Index < PadComponents.Num(); ++Index)
	{
		UChildActorComponent* PadComponent = PadComponents[Index];
		if (IsValid(PadComponent) == false)
		{
			continue;
		}

		const int32 Row = Index / 3;
		const int32 Column = Index % 3;
		PadComponent->SetRelativeLocation(FVector(
			(Column - 1) * Padding.X,
			(1 - Row) * Padding.Y,
			0.f));

		if (PadComponent->GetChildActorClass() != PadClass.Get())
		{
			PadComponent->SetChildActorClass(PadClass);
		}

		SetPadNumber(Index, Index + 1);
	}
}

void ANBPadController::BeginPlay()
{
	Super::BeginPlay();

	for (int32 Index = 0; Index < PadComponents.Num(); ++Index)
	{
		SetPadNumber(Index, Index + 1);
	}

	OnRep_IsActive();

	if (HasAuthority())
	{
		TurnPhaseChangedListenerHandle = UGameplayMessageSubsystem::Get(this)
			.RegisterListener<FNBTurnPhaseChangedMessage>(
				NBGameplayMessages::TurnPhaseChanged,
				this,
				&ThisClass::HandleTurnPhaseChanged);

		if (ANBGameState* NBGameState = GetWorld()->GetGameState<ANBGameState>())
		{
			ApplyTurnPhase(NBGameState->GetCurrentTurnPhase());
		}
	}
}

void ANBPadController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	TurnPhaseChangedListenerHandle.Unregister();

	Super::EndPlay(EndPlayReason);
}

void ANBPadController::ResetPads()
{
	for (UChildActorComponent* PadComponent : PadComponents)
	{
		if (IsValid(PadComponent))
		{
			if (ANBNumberPad* NumberPad = Cast<ANBNumberPad>(PadComponent->GetChildActor()))
			{
				NumberPad->ResetInput();
			}
		}
	}
}

void ANBPadController::SetPadNumber(int32 PadIndex, int32 Number)
{
	if (PadComponents.IsValidIndex(PadIndex) == false)
	{
		return;
	}

	UChildActorComponent* PadComponent = PadComponents[PadIndex];
	if (IsValid(PadComponent))
	{
		if (ANBNumberPad* NumberPad = Cast<ANBNumberPad>(PadComponent->GetChildActor()))
		{
			NumberPad->SetPadNumber(Number);
		}
	}
}

void ANBPadController::SetControllerActive(bool bActive)
{
	if (HasAuthority() == false || bIsActive == bActive)
	{
		return;
	}

	bIsActive = bActive;
	OnRep_IsActive();
}

void ANBPadController::HandleTurnPhaseChanged(
	FGameplayTag Channel,
	const FNBTurnPhaseChangedMessage& Message)
{
	ApplyTurnPhase(Message.TurnPhase);
}

void ANBPadController::ApplyTurnPhase(ENBTurnPhase TurnPhase)
{
	UE_LOG(LogNBPadController, Warning, TEXT("turn change %s"), *UEnum::GetValueAsString(TurnPhase));

	const bool bIsUserInputTurn = TurnPhase == ENBTurnPhase::UserInputTurn;
	if (bIsUserInputTurn)
	{
		ResetPads();
	}

	SetControllerActive(bIsUserInputTurn);
}

void ANBPadController::OnRep_IsActive()
{
	for (UChildActorComponent* PadComponent : PadComponents)
	{
		if (IsValid(PadComponent))
		{
			if (ANBNumberPad* NumberPad = Cast<ANBNumberPad>(PadComponent->GetChildActor()))
			{
				NumberPad->SetPadEnabled(bIsActive);
			}
		}
	}

	OnActiveStateChanged(bIsActive);
}
