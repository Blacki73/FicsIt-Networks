#include "Computer/FINComputerSubsystem.h"

#include "EnhancedInputComponent.h"
#include "Subsystem/SubsystemActorManager.h"
#include "FGCharacterPlayer.h"
#include "FGInputSettings.h"

AFINComputerSubsystem::AFINComputerSubsystem() {
	Input = CreateDefaultSubobject<UEnhancedInputComponent>("Input");
	const UFGInputSettings* Settings = UFGInputSettings::Get();
	
	Input->BindAction(Settings->GetInputActionForTag(FGameplayTag::RequestGameplayTag(TEXT("Input.PlayerActions.PrimaryFire"))), ETriggerEvent::Started, this, &AFINComputerSubsystem::OnPrimaryFirePressed);
	Input->BindAction(Settings->GetInputActionForTag(FGameplayTag::RequestGameplayTag(TEXT("Input.PlayerActions.PrimaryFire"))), ETriggerEvent::Completed, this, &AFINComputerSubsystem::OnPrimaryFireReleased);
	Input->BindAction(Settings->GetInputActionForTag(FGameplayTag::RequestGameplayTag(TEXT("Input.PlayerActions.SecondaryFire"))), ETriggerEvent::Started, this, &AFINComputerSubsystem::OnSecondaryFirePressed);
	Input->BindAction(Settings->GetInputActionForTag(FGameplayTag::RequestGameplayTag(TEXT("Input.PlayerActions.SecondaryFire"))), ETriggerEvent::Completed, this, &AFINComputerSubsystem::OnSecondaryFireReleased);
	
	SetActorTickEnabled(true);
	PrimaryActorTick.SetTickFunctionEnable(true);
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	SetReplicates(true);
	bAlwaysRelevant = true;
}

void AFINComputerSubsystem::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);
}

void AFINComputerSubsystem::BeginPlay() {
	Super::BeginPlay();

	TArray<AActor*> FoundCharacters;
	UGameplayStatics::GetAllActorsOfClass(this, AFGCharacterPlayer::StaticClass(), FoundCharacters);
	for (AActor* Character : FoundCharacters) AttachWidgetInteractionToPlayer(Cast<AFGCharacterPlayer>(Character));
}

void AFINComputerSubsystem::Tick(float dt) {
	Super::Tick(dt);
	this->GetWorld()->GetFirstPlayerController()->PushInputComponent(Input);
}

bool AFINComputerSubsystem::ShouldSave_Implementation() const {
	return true;
}

void AFINComputerSubsystem::PreSaveGame_Implementation(int32 saveVersion, int32 gameVersion) {
	Version = FINLatestVersion;
}

void AFINComputerSubsystem::OnPrimaryFirePressed() {
	AController* controller = GetWorld()->GetFirstPlayerController();
	if (controller) {
		AFGCharacterPlayer* character = Cast<AFGCharacterPlayer>(controller->GetCharacter());
		if (character) {
			UWidgetInteractionComponent** Comp = ScreenInteraction.Find(character);
			if (Comp && IsValid(*Comp)) (*Comp)->PressPointerKey(EKeys::LeftMouseButton);
		}
	}
}

void AFINComputerSubsystem::OnPrimaryFireReleased() {
	AController* controller = GetWorld()->GetFirstPlayerController();
	if (controller) {
		AFGCharacterPlayer* character = Cast<AFGCharacterPlayer>(controller->GetCharacter());
		if (character) {
			UWidgetInteractionComponent** Comp = ScreenInteraction.Find(character);
			if (Comp && IsValid(*Comp)) (*Comp)->ReleasePointerKey(EKeys::LeftMouseButton);
		}
	}
}

void AFINComputerSubsystem::OnSecondaryFirePressed() {
	AController* controller = GetWorld()->GetFirstPlayerController();
	if (controller) {
		AFGCharacterPlayer* character = Cast<AFGCharacterPlayer>(controller->GetCharacter());
		if (character) {
			UWidgetInteractionComponent** Comp = ScreenInteraction.Find(character);
			if (Comp && IsValid(*Comp)) (*Comp)->PressPointerKey(EKeys::RightMouseButton);
		}
	}
}

void AFINComputerSubsystem::OnSecondaryFireReleased() {
	AController* controller = GetWorld()->GetFirstPlayerController();
	if (controller) {
		AFGCharacterPlayer* character = Cast<AFGCharacterPlayer>(controller->GetCharacter());
		if (character) {
			UWidgetInteractionComponent** Comp = ScreenInteraction.Find(character);
			if (Comp && IsValid(*Comp)) (*Comp)->ReleasePointerKey(EKeys::RightMouseButton);
		}
	}
}

AFINComputerSubsystem* AFINComputerSubsystem::GetComputerSubsystem(UObject* WorldContext) {
#if WITH_EDITOR
	return nullptr;
#endif
	UWorld* WorldObject = GEngine->GetWorldFromContextObjectChecked(WorldContext);
	USubsystemActorManager* SubsystemActorManager = WorldObject->GetSubsystem<USubsystemActorManager>();
	check(SubsystemActorManager);
	return SubsystemActorManager->GetSubsystemActor<AFINComputerSubsystem>();
}

void AFINComputerSubsystem::AttachWidgetInteractionToPlayer(AFGCharacterPlayer* character) {
	if (!IsValid(character) || !character->GetController() || !character->GetController()->IsLocalPlayerController()) return;
	DetachWidgetInteractionToPlayer(character);
	UWidgetInteractionComponent* Comp = NewObject<UWidgetInteractionComponent>(character);
	Comp->InteractionSource = EWidgetInteractionSource::World;
	UCameraComponent* cam = Cast<UCameraComponent>(character->GetComponentByClass(UCameraComponent::StaticClass()));
	Comp->InteractionDistance = 10000.0;
	Comp->VirtualUserIndex = VirtualUserNum++;
	Comp->RegisterComponent();
	Comp->AttachToComponent(cam, FAttachmentTransformRules::KeepRelativeTransform);
	ScreenInteraction.Add(character, Comp);
}

void AFINComputerSubsystem::DetachWidgetInteractionToPlayer(AFGCharacterPlayer* character) {
	if (!IsValid(character)) return;
	UWidgetInteractionComponent** Comp = ScreenInteraction.Find(character);
	if (Comp) {
		(*Comp)->UnregisterComponent();
		ScreenInteraction.Remove(character);
	}
}
