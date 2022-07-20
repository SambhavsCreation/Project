// Fill out your copyright notice in the Description page of Project Settings.


#include "TestGameInstance.h"

#include "GSDKUtils.h"
#include "MyProject3Character.h"
#include "PlayfabGSDK.h"
#include "PlayFabMultiplayerModels.h"

UTestGameInstance::UTestGameInstance()
{
	Http = &FHttpModule::Get();
}


void UTestGameInstance::Init()
{
	Super::Init();
	StartGame();

	FOnGSDKShutdown_Dyn OnGSDKShutdown;
	OnGSDKShutdown.BindDynamic(this, &UTestGameInstance::OnGSDKShutdown);
	FOnGSDKHealthCheck_Dyn OnGSDKHealthCheck;
	OnGSDKHealthCheck.BindDynamic(this, &UTestGameInstance::OnGSDKHealthCheck);
	FOnGSDKServerActive_Dyn OnGSDKServerActive;
	OnGSDKServerActive.BindDynamic(this, &UTestGameInstance::OnGSDKServerActive);
	FOnGSDKReadyForPlayers_Dyn OnGSDKReadyForPlayers;
	OnGSDKReadyForPlayers.BindDynamic(this, &UTestGameInstance::OnGSDKReadyForPlayers);

	UGSDKUtils::RegisterGSDKShutdownDelegate(OnGSDKShutdown);
	UGSDKUtils::RegisterGSDKHealthCheckDelegate(OnGSDKHealthCheck);
	UGSDKUtils::RegisterGSDKServerActiveDelegate(OnGSDKServerActive);
	UGSDKUtils::RegisterGSDKReadyForPlayers(OnGSDKReadyForPlayers);
#if UE_SERVER
	UGSDKUtils::SetDefaultServerHostPort();
#endif

	
}


void UTestGameInstance::StartGame()
{
	GetMutableDefault<UPlayFabRuntimeSettings>()->TitleId = TEXT("E268D");
	
	
}

void UTestGameInstance::OnGSDKShutdown()
{
	UE_LOG(LogTemp, Warning, TEXT("Shutdown!"));
	FPlatformMisc::RequestExit(false);
}

bool UTestGameInstance::OnGSDKHealthCheck()
{
	UE_LOG(LogTemp, Warning, TEXT("Healthy!"));

	return true;
}

void UTestGameInstance::OnGSDKServerActive()
{
	/**
	 * Server is transitioning to an active state.
	 * Optional: Add in the implementation any code that is needed for the game server when
	 * this transition occurs.
	 */
	UE_LOG(LogTemp, Warning, TEXT("Active!"));
}

void UTestGameInstance::OnGSDKReadyForPlayers()
{
	/**
	 * Server is transitioning to a StandBy state. Game initialization is complete and the game
	 * is ready to accept players.
	 * Optional: Add in the implementation any code that is needed for the game server before
	 * initialization completes.
	 */
	UE_LOG(LogTemp, Warning, TEXT("Finished Initialization - Moving to StandBy!"));
}

void UTestGameInstance::OnStart()
{
	UE_LOG(LogTemp, Warning, TEXT("Reached onStart!"));
	UGSDKUtils::ReadyForPlayers();
}


