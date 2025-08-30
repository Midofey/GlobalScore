// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalScorePlayerController.h"

void AGlobalScorePlayerController::Client_ReceiveLeaderboardData_Implementation(
	const TArray<FLeaderboardEntry>& LeaderboardEntries)
{
	RecieveLeaderboardData(LeaderboardEntries);
}
