// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProject3Character.h"

#include "JsonObjectConverter.h"
#include "LoginActor.h"
#include "PlayFabAuthenticationDataModels.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Core/PlayFabClientAPI.h"
#include "Core/PlayFabMultiplayerAPI.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Windows/WindowsPlatformApplicationMisc.h"

// PlayFab imports



DEFINE_LOG_CATEGORY(LogPlayerOnline)


AMyProject3Character::AMyProject3Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	UE_LOG(LogTemp,  Warning, TEXT("HOLAAAA"))

}

//////////////////////////////////////////////////////////////////////////
// Input

void AMyProject3Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AMyProject3Character::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AMyProject3Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AMyProject3Character::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AMyProject3Character::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMyProject3Character::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMyProject3Character::TouchStopped);
}

void AMyProject3Character::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AMyProject3Character::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AMyProject3Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AMyProject3Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AMyProject3Character::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMyProject3Character::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AMyProject3Character::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMyProject3Character::Login(FString CustomID)
{
	ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	
	FLoginWithCustomIDRequest request;
	request.CustomId = CustomID;
	request.CreateAccount = true;
	
	ClientAPI->LoginWithCustomID(
		request,
		UPlayFabClientAPI::FLoginWithCustomIDDelegate::CreateUObject(this, &AMyProject3Character::OnLoginSuccess),
		FPlayFabErrorDelegate::CreateUObject(this, &AMyProject3Character::OnLoginError)
	);
	
	UE_LOG(LogTemp, Warning, TEXT("Game Instance"))
}

void AMyProject3Character::OnLoginSuccess(const FLoginResult& Result)
{
	GEngine->AddOnScreenDebugMessage(1, 15.f, FColor::Green, TEXT("Login Successful"));

	MatchmakerAPI = IPlayFabModuleInterface::Get().GetMatchmakerAPI();
	MultiplayerAPI = IPlayFabModuleInterface::Get().GetMultiplayerAPI();

	PlayerLoginResult = Result;
}

void AMyProject3Character::OnLoginError(const PlayFab::FPlayFabCppError& ErrorResult)
{
	GEngine->AddOnScreenDebugMessage(1, 15, FColor::Red, TEXT("Login Failed"));
}

void AMyProject3Character::CreateMatchmakingTicket(FString LobbyId)
{
	TSharedPtr<FJsonObject> Attributes = MakeShareable(new FJsonObject());
	TSharedPtr<FJsonObject> LatencyPtr = MakeShareable(new FJsonObject());
	TSharedPtr<FJsonObject> LatencyItemPtr = MakeShareable(new FJsonObject());
	LatencyItemPtr->SetStringField("region", "EastUs");
	LatencyItemPtr->SetStringField("latency", "150");
	TArray<TSharedPtr<FJsonValue>> LatencyList;
	LatencyList.Add(MakeShareable(new FJsonValueObject(LatencyItemPtr)) );
	Attributes->SetObjectField("Attributes", LatencyPtr);
	
	
	LatencyPtr->SetArrayField("Latencies", LatencyList);
	
	MultiplayerModels::FCreateMatchmakingTicketRequest MatchmakingTicketRequest;
	MatchmakingTicketRequest.Creator.Entity.Id = PlayerLoginResult.EntityToken->Entity->Id;
	MatchmakingTicketRequest.Creator.Entity.Type = PlayerLoginResult.EntityToken->Entity->Type;
	MatchmakingTicketRequest.QueueName = TEXT("TestQueue");
	MatchmakingTicketRequest.GiveUpAfterSeconds = 120;
	FJsonKeeper x(Attributes);

	MultiplayerModels::FMatchmakingPlayerAttributes PlayerAttributes;
	PlayerAttributes.DataObject = x;
	MatchmakingTicketRequest.Creator.Attributes = MakeShareable(new MultiplayerModels::FMatchmakingPlayerAttributes(PlayerAttributes));
	UE_LOG(LogTemp, Warning, TEXT("%s"), *MatchmakingTicketRequest.Creator.Attributes->DataObject.toJSONString());
	
	MultiplayerAPI->CreateMatchmakingTicket(
		MatchmakingTicketRequest,
		UPlayFabMultiplayerAPI::FCreateMatchmakingTicketDelegate::CreateUObject(this, &AMyProject3Character::OnMatchmakingTicketCreationSuccessful),
		FPlayFabErrorDelegate::CreateUObject(this, &AMyProject3Character::OnMatchmakingFailed)
		);
	
}

void AMyProject3Character::OnMatchmakingTicketCreationSuccessful(const MultiplayerModels::FCreateMatchmakingTicketResult& MatchmakingTicketResult)
{
	GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Green, TEXT("Matchmaking ticket Successful"));
	MatchmakingTicket = MatchmakingTicketResult;
	GetWorld()->GetTimerManager().SetTimer(MultiplayerTimer, this, &AMyProject3Character::Trigger_FindMatchmakingTicket, 8.f, true);
	
}

void AMyProject3Character::OnMatchmakingFailed(const FPlayFabCppError& Error)
{
	GEngine->AddOnScreenDebugMessage(1, 15.f, FColor::Red, TEXT("Matchmaking ticket Unsuccessful- " + Error.ErrorMessage));
	UE_LOG(LogTemp, Warning, TEXT("Error- %s"), *Error.ErrorMessage)
}

void AMyProject3Character::OnMatchmakingTicketFound(const MultiplayerModels::FGetMatchmakingTicketResult& Result)
{
	GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, TEXT("Matchmaking ticket status- " + Result.Status));

	if (Result.Status.Equals(TEXT("Matched")))
	{
		MultiplayerModels::FGetMatchRequest MatchRequest;
		MatchRequest.MatchId = Result.MatchId;
		MatchRequest.QueueName = TEXT("TestQueue");
		MultiplayerAPI->GetMatch(
			MatchRequest,
			UPlayFabMultiplayerAPI::FGetMatchDelegate::CreateUObject(this, &AMyProject3Character::OnMatchFoundSuccessful),
			FPlayFabErrorDelegate::CreateUObject(this, &AMyProject3Character::OnMatchmakingFailed)
			);	
	}
}

void AMyProject3Character::Trigger_FindMatchmakingTicket()
{
	MultiplayerModels::FGetMatchmakingTicketRequest MatchmakingTicketRequest;
	MatchmakingTicketRequest.QueueName = FString(TEXT("TestQueue"));
	MatchmakingTicketRequest.TicketId = MatchmakingTicket.TicketId;
	MultiplayerAPI->GetMatchmakingTicket(
		MatchmakingTicketRequest,
		UPlayFabMultiplayerAPI::FGetMatchmakingTicketDelegate::CreateUObject(this, &AMyProject3Character::OnMatchmakingTicketFound),
		FPlayFabErrorDelegate::CreateUObject(this, &AMyProject3Character::OnMatchmakingFailed)
		);
}

void AMyProject3Character::OnMatchFoundSuccessful(const MultiplayerModels::FGetMatchResult& Result)
{
	GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, TEXT("Matchmaking successful!!! "));
	FPlatformApplicationMisc::ClipboardCopy(*Result.pfServerDetails->IPV4Address);
	FPlatformApplicationMisc::ClipboardCopy(*FString::FromInt(Result.pfServerDetails->Ports[0].Num));
	FString Server = Result.pfServerDetails->IPV4Address + ":" + FString::FromInt(Result.pfServerDetails->Ports[0].Num);
	UGameplayStatics::OpenLevel(GetWorld(), FName(*Server));
}




 

