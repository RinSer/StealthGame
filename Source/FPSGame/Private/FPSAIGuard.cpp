// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSAIGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "FPSGameMode.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnHearNoise);

	GuardState = EAIState::Idle;
}

void AFPSAIGuard::OnRep_GuardState()
{
	OnStateChanged(GuardState);
}

void AFPSAIGuard::SetGuardState(EAIState NewState)
{
	if (GuardState == NewState)
		return;

	GuardState = NewState;
	OnRep_GuardState();
}

void AFPSAIGuard::ResetOrientation()
{
	if (GuardState == EAIState::Alerted)
		return;
	
	SetActorRotation(OriginalRotation);

	SetGuardState(EAIState::Idle);
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();

	OriginalRotation = GetActorRotation();
}

void AFPSAIGuard::OnPawnSeen(APawn* Pawn)
{
	if (Pawn == nullptr)
	{
		return;
	}
	
	DrawDebugSphere(GetWorld(), Pawn->GetActorLocation(), 32.0f, 12, FColor::Red, false, 12.0f);

	AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->CompleteMission(Pawn, false);
	}
	
	SetGuardState(EAIState::Alerted);
}

void AFPSAIGuard::OnHearNoise(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	if (NoiseInstigator == nullptr || GuardState == EAIState::Alerted)
	{
		return;
	}

	DrawDebugSphere(GetWorld(), NoiseInstigator->GetActorLocation(), 32.0f, 12, FColor::Yellow, false, 12.0f);

	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();

	FRotator newLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	newLookAt.Pitch = 0.0f;
	newLookAt.Roll = 0.0f;
	SetActorRotation(newLookAt);

	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAIGuard::ResetOrientation, 2.5f);

	SetGuardState(EAIState::Suspicious);
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFPSAIGuard::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSAIGuard, GuardState);
}
