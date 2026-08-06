#include "NBPadController.h"

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
}

void ANBPadController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsActive);
}

void ANBPadController::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SpawnPads();

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

	OnRep_IsActive();
}

void ANBPadController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	TurnPhaseChangedListenerHandle.Unregister();

	if (HasAuthority() && EndPlayReason == EEndPlayReason::Destroyed)
	{
		for (ANBNumberPad* NumberPad : SpawnedPads)
		{
			if (IsValid(NumberPad))
			{
				NumberPad->Destroy();
			}
		}
	}

	SpawnedPads.Reset();
	Super::EndPlay(EndPlayReason);
}

void ANBPadController::ResetPads()
{
	if (HasAuthority() == false)
	{
		return;
	}

	for (ANBNumberPad* NumberPad : SpawnedPads)
	{
		if (IsValid(NumberPad))
		{
			NumberPad->ResetInput();
		}
	}
}

void ANBPadController::SetPadNumber(int32 PadIndex, int32 Number)
{
	if (HasAuthority() == false || SpawnedPads.IsValidIndex(PadIndex) == false)
	{
		return;
	}

	if (ANBNumberPad* NumberPad = SpawnedPads[PadIndex])
	{
		NumberPad->SetPadNumber(Number);
	}
}

void ANBPadController::SetControllerActive(bool bActive)
{
	if (HasAuthority() == false || bIsActive == bActive)
	{
		return;
	}

	bIsActive = bActive;
	for (ANBNumberPad* NumberPad : SpawnedPads)
	{
		if (IsValid(NumberPad))
		{
			NumberPad->SetPadEnabled(bIsActive);
		}
	}

	OnRep_IsActive();
	ForceNetUpdate();
}

void ANBPadController::SpawnPads()
{
	if (PadClass == nullptr)
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (int32 Index = 0; Index < 9; ++Index)
	{
		ANBNumberPad* NumberPad = GetWorld()->SpawnActor<ANBNumberPad>(
			PadClass,
			GetPadTransform(Index),
			SpawnParameters);
		if (IsValid(NumberPad))
		{
			NumberPad->SetReplicates(true);
			NumberPad->SetReplicateMovement(true);
			NumberPad->SetPadNumber(Index + 1);
			NumberPad->SetPadEnabled(bIsActive);
			NumberPad->AttachToComponent(SceneRoot, FAttachmentTransformRules::KeepWorldTransform);
			SpawnedPads.Add(NumberPad);
		}
	}
}

FTransform ANBPadController::GetPadTransform(int32 PadIndex) const
{
	const int32 Row = PadIndex / 3;
	const int32 Column = PadIndex % 3;
	const FVector LocalLocation(
		(Column - 1) * Padding.X,
		(1 - Row) * Padding.Y,
		0.f);

	return FTransform(FRotator::ZeroRotator, LocalLocation) * GetActorTransform();
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
	OnActiveStateChanged(bIsActive);
}
