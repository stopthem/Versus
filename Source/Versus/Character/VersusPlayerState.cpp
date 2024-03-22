// Fill out your copyright notice in the Description page of Project Settings.


#include "VersusPlayerState.h"
#include "Kismet/GameplayStatics.h"

AVersusPlayerState::AVersusPlayerState()
{
	bReplicates = true;
	NetUpdateFrequency = 5.0f;
}

void AVersusPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	//Cast<UVersusHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD())->UpdateScores();
}

void AVersusPlayerState::ND_IncreaseScore(float IncreaseAmount)
{
	IncreaseScore(IncreaseAmount);

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
		ForceNetUpdate();
	}
}
