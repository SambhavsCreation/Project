// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginActor.h"

// PlayFab imports
#include "Core/PlayFabClientAPI.h"

// Sets default values
ALoginActor::ALoginActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALoginActor::BeginPlay()
{
	Super::BeginPlay();
	GetMutableDefault<UPlayFabRuntimeSettings>()->TitleId = TEXT("E268D");
	clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();

	PlayFab::ClientModels::FLoginWithCustomIDRequest request;
	request.CustomId = TEXT("XYZ");
	request.CreateAccount = true;

	clientAPI->LoginWithCustomID(
		request,
		PlayFab::UPlayFabClientAPI::FLoginWithCustomIDDelegate::CreateUObject(this, &ALoginActor::onSuccess),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &ALoginActor::onError)
	);
}

void ALoginActor::onSuccess(const PlayFab::ClientModels::FLoginResult& Result)
{
	UE_LOG(LogTemp, Warning, TEXT("Login successful"));
}

void ALoginActor::onError(const PlayFab::FPlayFabCppError& ErrorResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Login Failed"))
}



// Called every frame
void ALoginActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

