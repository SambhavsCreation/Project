// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/PlayFabError.h"
#include "Core/PlayFabClientDataModels.h"
#include "PlayFab.h"
#include "PlayFabMultiplayerDataModels.h"
#include "GameFramework/Character.h"

#include "MyProject3Character.generated.h"

using namespace PlayFab;
using namespace ClientModels;

DECLARE_LOG_CATEGORY_EXTERN(LogPlayerOnline, Warning, All)

USTRUCT()
struct FTestStruct
{
	GENERATED_BODY()

	FString Test = TEXT("xyz");
};


UCLASS(config=Game)
class AMyProject3Character : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AMyProject3Character();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface


public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(Exec, Category="Commands")
	void Login(FString CustomID);
	


protected:
	virtual void BeginPlay() override;
	
	PlayFabClientPtr ClientAPI = nullptr;
	PlayFabMatchmakerPtr MatchmakerAPI = nullptr;
	PlayFabMultiplayerPtr MultiplayerAPI = nullptr;
// Login functions	
protected:
	// Login Delegates
	void OnLoginSuccess(const FLoginResult& Result);
	void OnLoginError(const FPlayFabCppError& ErrorResult);

	// Matchmaking Delegates
	UFUNCTION(Exec, Category="Commands")
	void CreateMatchmakingTicket(FString LobbyId);
	void OnMatchmakingTicketCreationSuccessful(const MultiplayerModels::FCreateMatchmakingTicketResult& MatchmakingTicketResult);
	void OnMatchmakingFailed(const FPlayFabCppError& Error);
	void OnMatchmakingTicketFound(const MultiplayerModels::FGetMatchmakingTicketResult&);
	FTimerHandle MultiplayerTimer;
	UFUNCTION()
	void Trigger_FindMatchmakingTicket();
	MultiplayerModels::FCreateMatchmakingTicketResult MatchmakingTicket;

	// Match Delegates
	void OnMatchFoundSuccessful(const MultiplayerModels::FGetMatchResult&);

	// Lobby creation Delegates
	// UFUNCTION(Exec, Category="Commands")
	// void CreateLobby();
	// void OnLobbyCreationSuccessful(const MultiplayerModels::FCreateLobbyResult& Result);
	// void OnLobbyCreationError(const FPlayFabCppError& Error);

	// Lobby join Delegates
	// UFUNCTION(Exec, Category="Commands")
	// void JoinLobby(FString ConnectionString);
	// void OnJoinLobbySuccessful(const MultiplayerModels::FJoinLobbyResult& JoinLobbyResult);
	// void OnJoinLobbyError(const FPlayFabCppError& Error);

	// Utils
	// void LobbyFound(const MultiplayerModels::FGetLobbyResult& LobbyResult);

	

private:
	FLoginResult PlayerLoginResult;
	
	
};

