// Fill out your copyright notice in the Description page of Project Settings.


#include "VersusPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Versus/HUD/VersusHUD.h"

AVersusPlayerState::AVersusPlayerState()
{
	bReplicates = true;

	// We will only update score, no need greater values
	NetUpdateFrequency = 5.0f;

	NetDormancy = DORM_DormantAll;
}

void AVersusPlayerState::BeginPlay()
{
	Super::BeginPlay();

	// We use a timer because player name is not set when this funciton is called
	FTimerHandle ServerUpdateScoresTimerHandle;
	GetWorldTimerManager().SetTimer(ServerUpdateScoresTimerHandle, FTimerDelegate::CreateLambda([&]
		{
			// Update scores in UI
			Cast<AVersusHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD())->UpdateScores();
		}), 1.0f, false);
}

void AVersusPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Cast<AVersusHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD())->UpdateScores();
}

void AVersusPlayerState::ND_IncreaseScore(float IncreaseAmount)
{
	// Make sure local gets it asap
	IncreaseScore(IncreaseAmount);

	// If client, tell server
	if (!HasAuthority())
	{
		SVR_IncreaseScore(IncreaseAmount);
	}
}

bool AVersusPlayerState::SVR_IncreaseScore_Validate(float IncreaseAmount)
{
	return true;
}

void AVersusPlayerState::SVR_IncreaseScore_Implementation(float IncreaseAmount)
{
	IncreaseScore(IncreaseAmount);
}

void AVersusPlayerState::IncreaseScore(float IncreaseAmount)
{
	SetScore(GetScore() + IncreaseAmount);

	OnRep_Score();

	if (HasAuthority())
	{
		// Our update frequency is low, force a update so clients get it asap
		ForceNetUpdate();
	}
}
