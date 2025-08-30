// Copyright Epic Games, Inc. All Rights Reserved.

#include "GlobalScoreGameMode.h"
#include "GlobalScoreCharacter.h"
#include "GlobalScorePlayerController.h"
#include "UObject/ConstructorHelpers.h"

 
#include "MidofeySQLSubsystem.h"
 

AGlobalScoreGameMode::AGlobalScoreGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}



void AGlobalScoreGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
 
	// Cast to your custom PlayerController class
	AGlobalScorePlayerController* NewPC = Cast<AGlobalScorePlayerController>(NewPlayer);
	if (!NewPC) return;

	// Get the world to access all PlayerControllers
	UWorld* World = GetWorld();
	if (!World) return;

	// Loop through all connected PlayerControllers
	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		AGlobalScorePlayerController* PC = Cast<AGlobalScorePlayerController>(It->Get());
		if (PC == NewPC)
		{
			// Only send leaderboard data to the newly joined player
			PC->Client_ReceiveLeaderboardData(CachedLeaderboard);
			break; // Stop iterating — we found our target
		}
	}
 
}




void AGlobalScoreGameMode::BeginPlay()
{
	Super::BeginPlay();
		FTimerHandle TimerHandle;
		UMidofeySQLSubsystem* MidofeySQLSubsystem = GetGameInstance()->GetSubsystem<UMidofeySQLSubsystem>();
		MidofeySQLSubsystem->SetConnectionProperties(
			TEXT("127.0.0.1"),
			TEXT("root"),
			TEXT("YOUR_PASSWORD_HERE"),
			TEXT("leaderboard"),
			3306,
			FConnectionSslConfig());
		if (MidofeySQLSubsystem->EstablishConnection())
		{
			RefreshLeaderboard();
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &AGlobalScoreGameMode::RefreshLeaderboard), 30.0f, true);
		}
}

void AGlobalScoreGameMode::SubmitScore(FLeaderboardEntry LeaderboardEntry)
{
 
	UMidofeySQLSubsystem* MidofeySQLSubsystem = GetGameInstance()->GetSubsystem<UMidofeySQLSubsystem>();
	FQueryData QueryData;
	QueryData.Parameters.Add(FQueryValue::FromString(LeaderboardEntry.Name));
	QueryData.Parameters.Add(FQueryValue::FromInt(LeaderboardEntry.Score));
	MidofeySQLSubsystem->ExecuteQueryAsync(TEXT("INSERT INTO leaderboard (player_name, score) VALUES (?, ?) ON DUPLICATE KEY UPDATE score = VALUES(score)"), QueryData, [this, LeaderboardEntry](const FQueryResult& Result)
		{
			const int32 Index = CachedLeaderboard.Find(LeaderboardEntry);
			if (Index != INDEX_NONE)
			{
				CachedLeaderboard[Index].Score = LeaderboardEntry.Score;
			}
			else
			{
				CachedLeaderboard.Add(LeaderboardEntry);
				UE_LOG(LogTemp, Warning, TEXT("Added Leaderboard Entry for %s"), *LeaderboardEntry.Name)
			}
			CachedLeaderboard.Sort([](const FLeaderboardEntry& A, const FLeaderboardEntry& B)
				{
					return A.Score > B.Score;
				});

			SyncLeaderboardData();
		});
 
}

void AGlobalScoreGameMode::RemoveLeaderboardEntry(FLeaderboardEntry Entry)
{
 
	UMidofeySQLSubsystem* MidofeySQLSubsystem = GetGameInstance()->GetSubsystem<UMidofeySQLSubsystem>();
	FQueryData QueryData;
	QueryData.Parameters.Add(FQueryValue::FromString(Entry.Name));
	MidofeySQLSubsystem->ExecuteQueryAsync(TEXT("DELETE FROM leaderboard WHERE player_name = ?"), QueryData, [this, Entry](const FQueryResult& Result)
		{
			if (Result.RowCount >= 1)
			{
				const int32 Index = CachedLeaderboard.Find(Entry);
				if (Index != INDEX_NONE)
				{
					CachedLeaderboard.RemoveAt(Index);
					CachedLeaderboard.Sort([](const FLeaderboardEntry& A, const FLeaderboardEntry& B)
						{
							return A.Score > B.Score;
						});
					SyncLeaderboardData();
				}
			}
		});
 
}

void AGlobalScoreGameMode::RefreshLeaderboard()
{
 
	UMidofeySQLSubsystem* MidofeySQLSubsystem = GetGameInstance()->GetSubsystem<UMidofeySQLSubsystem>();
	FQueryData QueryData;
	MidofeySQLSubsystem->ExecuteQueryAsync(TEXT("SELECT * FROM leaderboard ORDER BY score DESC LIMIT 100"), QueryData, [this](const FQueryResult& Result)
		{
			CachedLeaderboard.Empty();
			for (const auto& Row : Result.Rows)
			{
				FLeaderboardEntry LeaderboardEntry;
				LeaderboardEntry.Name = Row.Get("player_name");;
				LeaderboardEntry.Score = Row.Get("score");
				CachedLeaderboard.Add(LeaderboardEntry);
			}
			SyncLeaderboardData();
		});
 
}

void AGlobalScoreGameMode::SyncLeaderboardData()
{
 
	UWorld* World = GetWorld();
	if (!World) return;

	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		AGlobalScorePlayerController* PC = Cast<AGlobalScorePlayerController>(It->Get());
		if (PC)
		{
			PC->Client_ReceiveLeaderboardData(CachedLeaderboard);
		}
	}
 
}

