// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

AFPSGameMode::AFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();
}

void AFPSGameMode::CompleteMission(APawn* InstigatorPawn, bool bMissionSuccess)
{
	if (InstigatorPawn)
	{
		InstigatorPawn->DisableInput(nullptr);

		if (SideViewer)
		{
			TArray<AActor*> returnedActors;
			UGameplayStatics::GetAllActorsOfClass(this, SideViewer, returnedActors);

			APlayerController* PC = Cast<APlayerController>(InstigatorPawn->GetController());
			if (PC && returnedActors.Num() > 0)
			{
				PC->SetViewTargetWithBlend(returnedActors[0], 0.5f, EViewTargetBlendFunction::VTBlend_Cubic);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No SideViewers found!!!"));
		}
	}

	OnMissionCompleted(InstigatorPawn, bMissionSuccess);
}
