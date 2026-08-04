#include "NBNumberPad.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBGameplayMessages.h"

ANBNumberPad::ANBNumberPad()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bAlwaysRelevant = true;

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

void ANBNumberPad::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bInputSubmitted);
	DOREPLIFETIME(ThisClass, HavingNumber);
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
}

void ANBNumberPad::ResetInput()
{
	if (HasAuthority() == false || bInputSubmitted == false)
	{
		return;
	}

	bInputSubmitted = false;
	OnRep_InputSubmitted();
}

void ANBNumberPad::SetPadEnabled(bool bEnabled)
{
	if (bIsPadEnabled == bEnabled)
	{
		return;
	}

	bIsPadEnabled = bEnabled;
	if (bIsPadEnabled)
	{
		OnPadActivated();
	}

	Trigger->SetCollisionEnabled(bEnabled ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void ANBNumberPad::SetPadNumber(int32 Number)
{
	if (HavingNumber == Number)
	{
		return;
	}

	HavingNumber = Number;
	OnRep_HavingNumber();
}

void ANBNumberPad::OnRep_InputSubmitted()
{
	OnInputSubmittedChanged(bInputSubmitted);
}

void ANBNumberPad::OnRep_HavingNumber()
{
	OnPadNumberChanged(HavingNumber);
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

	OnPressStarted(Pawn);
	OnRep_InputSubmitted();
}

void ANBNumberPad::HandleEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex)
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

	OnPressReleased(Pawn);
	OnRep_InputSubmitted();
}

//TODO 플레이어 폰인지 체크하는 함수 만들자
