#pragma once

#include "CoreMinimal.h"
#include "U05_SessionGameMode.h"
#include "CLobbyGameMode.generated.h"

UCLASS()
class U05_SESSION_API ACLobbyGameMode : public AU05_SessionGameMode
{
	GENERATED_BODY()
	
public:
	void PostLogin(APlayerController* NewPlayer) override;
	void Logout(AController* Exiting) override;

private:
	void StartGame();

private:
	uint32 NumberOfPlayers;
};
