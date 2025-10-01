// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GlobalScoreGameMode.generated.h"


USTRUCT(BlueprintType)
struct FLeaderboardEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Leaderboard")
	FString Name;

	UPROPERTY(BlueprintReadWrite, Category = "Leaderboard")
	int32 Score;

	bool operator==(const FLeaderboardEntry& Other) const
	{
		return Name == Other.Name;
	}

};

UCLASS(minimalapi)
class AGlobalScoreGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGlobalScoreGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	UPROPERTY(BlueprintReadOnly, Category = "Leaderboard", meta = (AllowPrivateAccess = true))
	TArray<FLeaderboardEntry> CachedLeaderboard;

	UFUNCTION(BlueprintCallable, Category = "Leaderboard")
	void SubmitScore(FLeaderboardEntry LeaderboardEntry);

	UFUNCTION(BlueprintCallable, Category = "Leaderboard")
	void RemoveLeaderboardEntry(FLeaderboardEntry Entry);

	UFUNCTION(BlueprintCallable, Category = "Leaderboard")
	void RefreshLeaderboard();

	void SyncLeaderboardData();

};



