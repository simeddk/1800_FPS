#include "FP_FirstPersonGameMode.h"
#include "Global.h"
#include "EngineUtils.h"
#include "FP_FirstPersonHUD.h"
#include "FP_FirstPersonCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Game/CGameState.h"
#include "Game/CPlayerState.h"
#include "Game/CSpawnPoint.h"


AFP_FirstPersonGameMode::AFP_FirstPersonGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	//Default Pawn Class
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	//HUD Class
	HUDClass = AFP_FirstPersonHUD::StaticClass();

	//GameState Class
	GameStateClass = ACGameState::StaticClass();

	//PlayerState Class
	PlayerStateClass = ACPlayerState::StaticClass();
}

void AFP_FirstPersonGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	//Get Player State & Get Player Pawn
	ACPlayerState* playerState = Cast<ACPlayerState>(NewPlayer->PlayerState);
	CheckNull(playerState);

	AFP_FirstPersonCharacter* playerPawn = Cast<AFP_FirstPersonCharacter>(NewPlayer->GetPawn());
	CheckNull(playerPawn);

	playerPawn->SetPlayerState(playerState);

	//팀 분배
	if (RedTeamPlayers.Num() > BlueTeamPlayers.Num())
	{
		BlueTeamPlayers.Add(playerPawn);
		playerState->Team = ETeamType::Blue;
	}
	else
	{
		RedTeamPlayers.Add(playerPawn);
		playerState->Team = ETeamType::Red;
	}

	//팀 세팅
	playerPawn->CurrentTeam = playerState->Team;
	playerPawn->SetTeamColor(playerState->Team);

	UWorld* world = GetWorld();
	CheckNull(world);

	SpawnHost(world);

	MoveToSpawnPoint(playerPawn);
}

void AFP_FirstPersonGameMode::Tick(float DeltaSeconds)
{
}

void AFP_FirstPersonGameMode::SpawnHost(UWorld* world)
{
	APlayerController* hostController = world->GetFirstPlayerController();
	CheckNull(hostController);

	AFP_FirstPersonCharacter* hostPawn = Cast<AFP_FirstPersonCharacter>(hostController->GetPawn());
	CheckNull(hostPawn);

	MoveToSpawnPoint(hostPawn);
}

void AFP_FirstPersonGameMode::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = GetWorld();
	CheckNull(world);

	for (TActorIterator<ACSpawnPoint> it(world); it; ++it)
	{
		if (it->GetTeam() == ETeamType::Red)
			RedTeamSpawnPoints.Add(*it);
		else
			BlueTeamSpawnPoints.Add(*it);
	}
	
}

void AFP_FirstPersonGameMode::MoveToSpawnPoint(AFP_FirstPersonCharacter* InPlayer)
{
	TArray<ACSpawnPoint*>* targetPoints;

	if (InPlayer->CurrentTeam == ETeamType::Red)
		targetPoints = &RedTeamSpawnPoints;
	else
		targetPoints = &BlueTeamSpawnPoints;

	for (const auto& target : *targetPoints)
	{
		//스폰포인트가 블락되어 있지 않은 경우
		if (target->IsBlocked() == false)
		{
			InPlayer->SetActorLocation(target->GetActorLocation());
			target->UpdateOverlaps();

			return;
		}

	}

	
	//스폰포인트가 블락되어 있는 경우
	if (WaitingPlayers.Find(InPlayer) < 0)
		WaitingPlayers.Add(InPlayer);
}
