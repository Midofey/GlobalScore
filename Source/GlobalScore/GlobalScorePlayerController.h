// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GlobalScorePlayerController.generated.h"

struct FLeaderboardEntry;
/**
 * 
 */
UCLASS()
class GLOBALSCORE_API AGlobalScorePlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	UFUNCTION(Client, Reliable, Category = "Leaderboard")
	void Client_ReceiveLeaderboardData(const TArray<FLeaderboardEntry>& LeaderboardEntries);

	UFUNCTION(BlueprintImplementableEvent, Category = "Leaderboard")
	void RecieveLeaderboardData(const TArray<FLeaderboardEntry>& LeaderboardEntries);
	
};
