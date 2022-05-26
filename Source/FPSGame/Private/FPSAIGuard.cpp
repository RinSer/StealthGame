// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSAIGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "FPSGameMode.h"

// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnHearNoise);
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();
	
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
}

void AFPSAIGuard::OnHearNoise(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	if (NoiseInstigator == nullptr)
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
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

