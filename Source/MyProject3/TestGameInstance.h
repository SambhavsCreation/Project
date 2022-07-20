// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "Engine/GameInstance.h"

#include "PlayFab.h"

#include "TestGameInstance.generated.h"


UCLASS()
class MYPROJECT3_API UTestGameInstance : public UGameInstance
{
	GENERATED_BODY()

	void StartGame();

	virtual void Init() override;

	UTestGameInstance();



private:
	PlayFabClientPtr clientAPI = nullptr;

protected:

	UFUNCTION()
	void OnGSDKShutdown();

	UFUNCTION()
	bool OnGSDKHealthCheck();

	UFUNCTION()
	void OnGSDKServerActive();

	UFUNCTION()
	void OnGSDKReadyForPlayers();

public:
	virtual void OnStart() override;
	FString PlayerPlayFabID;
	FString SessionTicket;

	void OnPlayerDataCaptured(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

private:
	FHttpModule* Http;
};
