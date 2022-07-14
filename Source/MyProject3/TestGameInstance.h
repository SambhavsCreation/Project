// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "PlayFab.h"
#include "Core/PlayFabError.h"
#include "Core/PlayFabClientDataModels.h"

#include "TestGameInstance.generated.h"

/**
 * 
 */

UCLASS()
class MYPROJECT3_API UTestGameInstance : public UGameInstance
{
	GENERATED_BODY()

	void StartGame();

	virtual void Init() override;

protected:
	void onSuccess(const PlayFab::ClientModels::FLoginResult& Result);
	void onError(const PlayFab::FPlayFabCppError& ErrorResult);

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
};
