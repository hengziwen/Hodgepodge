#include "CodexText/HodgeSurvivor.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "Animation/AnimationAsset.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeSurvivor)

AHodgeSurvivorHero::AHodgeSurvivorHero()
{
    PrimaryActorTick.bCanEverTick = true;
    GetCapsuleComponent()->InitCapsuleSize(35, 90);
    GetMesh()->SetRelativeLocationAndRotation(FVector(0,0,-90), FRotator(0,-90,0));
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> HeroMeshAsset(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple"));
    static ConstructorHelpers::FObjectFinder<UAnimationAsset> IdleAsset(TEXT("/Game/Characters/Mannequins/Animations/Manny/MM_Idle"));
    static ConstructorHelpers::FObjectFinder<UAnimationAsset> RunAsset(TEXT("/Game/Characters/Mannequins/Animations/Manny/MM_Run_Fwd"));
    GetMesh()->SetSkeletalMesh(HeroMeshAsset.Object);
    Idle = IdleAsset.Object; Run = RunAsset.Object;
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    auto* Arm = CreateDefaultSubobject<USpringArmComponent>(TEXT("ArenaCameraArm"));
    Arm->SetupAttachment(RootComponent);
    Arm->TargetArmLength = 1800;
    Arm->SetUsingAbsoluteRotation(true);
    Arm->SetRelativeRotation(FRotator(-60,0,0));
    Arm->bDoCollisionTest = false;
    auto* Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("ArenaCamera"));
    Camera->SetupAttachment(Arm);
    Camera->FieldOfView = 65;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0,720,0);
    GetCharacterMovement()->MaxWalkSpeed = 480;
    GetCharacterMovement()->BrakingDecelerationWalking = 2400;
}

void AHodgeSurvivorHero::SetupPlayerInputComponent(UInputComponent* Input)
{
    Super::SetupPlayerInputComponent(Input);
    auto* Enhanced = Cast<UEnhancedInputComponent>(Input);
    auto* PC = Cast<APlayerController>(Controller);
    if (!Enhanced || !PC || !PC->GetLocalPlayer()) return;
    Mapping = NewObject<UInputMappingContext>(this);
    ForwardAction = NewObject<UInputAction>(this);
    RightAction = NewObject<UInputAction>(this);
    ForwardAction->ValueType = RightAction->ValueType = EInputActionValueType::Axis1D;
    ForwardAction->AccumulationBehavior = RightAction->AccumulationBehavior = EInputActionAccumulationBehavior::Cumulative;
    Mapping->MapKey(ForwardAction, EKeys::W);
    Mapping->MapKey(ForwardAction, EKeys::S).Modifiers.Add(NewObject<UInputModifierNegate>(Mapping));
    Mapping->MapKey(RightAction, EKeys::D);
    Mapping->MapKey(RightAction, EKeys::A).Modifiers.Add(NewObject<UInputModifierNegate>(Mapping));
    PC->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->AddMappingContext(Mapping, 0);
    Enhanced->BindAction(ForwardAction, ETriggerEvent::Triggered, this, &ThisClass::Forward);
    Enhanced->BindAction(RightAction, ETriggerEvent::Triggered, this, &ThisClass::Right);
    Input->BindKey(EKeys::SpaceBar, IE_Pressed, this, &ThisClass::Dash);
}

void AHodgeSurvivorHero::Forward(const FInputActionValue& Value)
{
    auto* Mode = GetWorld()->GetAuthGameMode<AHodgeSurvivorMode>();
    if (Mode && Mode->State == EHodgeSurvivorState::Playing) AddMovementInput(FVector::ForwardVector, Value.Get<float>());
}
void AHodgeSurvivorHero::Right(const FInputActionValue& Value)
{
    auto* Mode = GetWorld()->GetAuthGameMode<AHodgeSurvivorMode>();
    if (Mode && Mode->State == EHodgeSurvivorState::Playing) AddMovementInput(FVector::RightVector, Value.Get<float>());
}
void AHodgeSurvivorHero::Dash()
{
    auto* Mode = GetWorld()->GetAuthGameMode<AHodgeSurvivorMode>();
    if (!Mode || Mode->State != EHodgeSurvivorState::Playing || DashCooldown > 0) return;
    FVector Direction = GetVelocity().GetSafeNormal2D();
    if (Direction.IsNearlyZero()) Direction = GetActorForwardVector();
    LaunchCharacter(Direction*1500, true, false);
    DashCooldown = 3; Invulnerability = .45f;
}
void AHodgeSurvivorHero::Tick(float Dt)
{
    Super::Tick(Dt);
    auto* Mode = GetWorld()->GetAuthGameMode<AHodgeSurvivorMode>();
    if (!Mode || Mode->State != EHodgeSurvivorState::Playing) return;
    DashCooldown = FMath::Max(0.f, DashCooldown-Dt);
    Invulnerability = FMath::Max(0.f, Invulnerability-Dt);
    const bool Moving = GetVelocity().SizeSquared2D() > 100;
    if (Moving != bRunning || !GetMesh()->IsPlaying())
    {
        bRunning = Moving;
        GetMesh()->PlayAnimation(Moving ? Run : Idle, true);
    }
    FVector P = GetActorLocation();
    const FVector Clamped(FMath::Clamp(P.X,-2450.f,2450.f), FMath::Clamp(P.Y,-2450.f,2450.f), P.Z);
    if (!P.Equals(Clamped)) SetActorLocation(Clamped);
}
void AHodgeSurvivorHero::EndPlay(const EEndPlayReason::Type Reason)
{
    if (auto* PC = Cast<APlayerController>(Controller))
        if (PC->GetLocalPlayer() && Mapping)
            PC->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->RemoveMappingContext(Mapping);
    Super::EndPlay(Reason);
}

AHodgeSurvivorMode::AHodgeSurvivorMode()
{
    PrimaryActorTick.bCanEverTick = true;
    DefaultPawnClass = AHodgeSurvivorHero::StaticClass();
    PlayerControllerClass = APlayerController::StaticClass();
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Shape(TEXT("/Engine/BasicShapes/Sphere"));
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> Mesh(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple"));
    static ConstructorHelpers::FObjectFinder<UAnimationAsset> Walk(TEXT("/Game/Characters/Mannequins/Animations/Manny/MM_Walk_InPlace"));
    Sphere = Shape.Object; EnemyMesh = Mesh.Object; EnemyRun = Walk.Object;
}
void AHodgeSurvivorMode::StartPlay()
{
    Super::StartPlay();
    Hero = Cast<AHodgeSurvivorHero>(UGameplayStatics::GetPlayerCharacter(this,0));
    APlayerController* PC = UGameplayStatics::GetPlayerController(this,0);
    if (PC && InterfaceClass)
    {
        Interface = CreateWidget<UUserWidget>(PC, InterfaceClass);
        if (Interface) Interface->AddToViewport();
    }
    SetRunState(EHodgeSurvivorState::Playing);
    for (int32 I=0; I<4; ++I) SpawnEnemy();
}
AActor* AHodgeSurvivorMode::MakeShape(FVector Location, FVector Scale, UMaterialInterface* Material, float Life)
{
    auto* Actor = GetWorld()->SpawnActor<AStaticMeshActor>(Location, FRotator::ZeroRotator);
    if (!Actor) return nullptr;
    auto* Mesh = Actor->GetStaticMeshComponent();
    Mesh->SetMobility(EComponentMobility::Movable);
    Mesh->SetStaticMesh(Sphere);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Mesh->SetCastShadow(false);
    if (Material) Mesh->SetMaterial(0,Material);
    Actor->SetActorScale3D(Scale);
    if (Life>0) Actor->SetLifeSpan(Life);
    return Actor;
}
void AHodgeSurvivorMode::SpawnEnemy(bool Elite)
{
    if (!Hero || Enemies.Num()>=85 || State!=EHodgeSurvivorState::Playing) return;
    const float Angle = FMath::FRand()*2*PI;
    FVector P = Hero->GetActorLocation()+FVector(FMath::Cos(Angle),FMath::Sin(Angle),0)*FMath::FRandRange(1050.f,1500.f);
    P.X=FMath::Clamp(P.X,-2600.f,2600.f); P.Y=FMath::Clamp(P.Y,-2600.f,2600.f); P.Z=4;
    AActor* Actor = GetWorld()->SpawnActor<AActor>(P,FRotator::ZeroRotator);
    if (!Actor) return;
    auto* Mesh = NewObject<USkeletalMeshComponent>(Actor);
    Actor->SetRootComponent(Mesh); Actor->AddInstanceComponent(Mesh);
    Mesh->SetSkeletalMesh(EnemyMesh); Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Mesh->RegisterComponent(); Actor->SetActorLocation(P);
    Actor->SetActorScale3D(FVector(Elite?1.65f:.9f));
    for (int32 I=0; I<Mesh->GetNumMaterials(); ++I)
        if (UMaterialInterface* Material = Elite?EliteMaterial:EnemyMaterial) Mesh->SetMaterial(I, Material);
    Mesh->PlayAnimation(EnemyRun,true);
    FHodgeSurvivorEnemy Entry;
    Entry.Actor=Actor; Entry.Health=(Elite?170.f:30.f)+Elapsed*(Elite?.9f:.22f);
    Entry.Speed=(Elite?115.f:145.f)+Elapsed*.45f+FMath::FRandRange(-15.f,20.f); Entry.bElite=Elite;
    Enemies.Add(Entry);
}
void AHodgeSurvivorMode::HitEnemy(int32 Index, float Amount)
{
    if (!Enemies.IsValidIndex(Index)) return;
    auto& Enemy = Enemies[Index];
    Enemy.Health-=Amount;
    if (Enemy.Health>0 || !Enemy.Actor.IsValid()) return;
    const FVector P=Enemy.Actor->GetActorLocation();
    if (Orbs.Num()<220)
    {
        FHodgeSurvivorOrb Orb;
        Orb.Value=Enemy.bElite?8:1;
        Orb.Actor=MakeShape(P+FVector(0,0,25),FVector(Enemy.bElite?.35f:.16f),EnergyMaterial);
        Orbs.Add(Orb);
    }
    ++Kills; Enemy.Actor->Destroy(); Enemies.RemoveAtSwap(Index);
}
void AHodgeSurvivorMode::DamageHero(float Amount)
{
    if (State!=EHodgeSurvivorState::Playing || Amount<=0 || !Hero || Hero->Invulnerability>0 || HurtClock>0) return;
    Health=FMath::Max(0.f,Health-Amount); HurtClock=.7f;
    MakeShape(Hero->GetActorLocation(),FVector(1.3f,1.3f,.06f),EliteMaterial,.15f);
    if (Health<=0) SetRunState(EHodgeSurvivorState::Defeat);
}
void AHodgeSurvivorMode::GrantExperience(int32 Amount)
{
    if (Amount<0 || State!=EHodgeSurvivorState::Playing) return;
    Experience+=Amount;
    if (Experience<NextLevelXP) return;
    Experience-=NextLevelXP; ++Level; NextLevelXP=5+Level*3;
    Health=FMath::Min(MaxHealth,Health+12);
    Choices={0,1,2,3,4,5};
    for (int32 I=Choices.Num()-1;I>0;--I) Choices.Swap(I,FMath::RandRange(0,I));
    Choices.SetNum(3);
    SetRunState(EHodgeSurvivorState::Upgrade);
}
FString AHodgeSurvivorMode::GetUpgradeText(int32 Slot) const
{
    static const TCHAR* Labels[]={TEXT("过载弹芯  ·  伤害 +35%"),TEXT("疾速扳机  ·  攻速 +20%"),TEXT("轻盈步伐  ·  移速 +12%"),TEXT("生命强化  ·  上限 +25 / 回复 40"),TEXT("磁力回收  ·  拾取范围 +100"),TEXT("电磁脉冲  ·  解锁 / 强化范围攻击")};
    return Choices.IsValidIndex(Slot)?Labels[Choices[Slot]]:TEXT("");
}
void AHodgeSurvivorMode::ChooseUpgrade(int32 Slot)
{
    if (State!=EHodgeSurvivorState::Upgrade || !Choices.IsValidIndex(Slot) || !Hero) return;
    switch (Choices[Slot])
    {
    case 0: Damage*=1.35f; break;
    case 1: AttackInterval=FMath::Max(.16f,AttackInterval*.8f); break;
    case 2: Hero->GetCharacterMovement()->MaxWalkSpeed=FMath::Min(780.f,Hero->GetCharacterMovement()->MaxWalkSpeed*1.12f); break;
    case 3: MaxHealth+=25; Health=FMath::Min(MaxHealth,Health+40); break;
    case 4: PickupRadius+=100; break;
    case 5: ++PulseLevel; PulseClock=0; break;
    }
    Choices.Reset(); SetRunState(EHodgeSurvivorState::Playing); GrantExperience(0);
}
void AHodgeSurvivorMode::SetRunState(EHodgeSurvivorState NewState)
{
    State=NewState;
    const bool Active=State==EHodgeSurvivorState::Playing;
    if (Hero)
    {
        Hero->GetCharacterMovement()->StopMovementImmediately();
        Hero->GetMesh()->GlobalAnimRateScale=Active?1.f:0.f;
    }
    for (auto& Enemy:Enemies)
        if (Enemy.Actor.IsValid())
            if (auto* Mesh=Enemy.Actor->FindComponentByClass<USkeletalMeshComponent>()) Mesh->GlobalAnimRateScale=Active?1.f:0.f;
    if (auto* PC=UGameplayStatics::GetPlayerController(this,0))
    {
        FInputModeGameAndUI Input; Input.SetHideCursorDuringCapture(false);
        Input.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(Input); PC->SetShowMouseCursor(true);
    }
    if (auto* UI=Cast<UHodgeSurvivorHUD>(Interface)) UI->Refresh(this);
}
void AHodgeSurvivorMode::TogglePause()
{
    if (State==EHodgeSurvivorState::Playing) SetRunState(EHodgeSurvivorState::Paused);
    else if (State==EHodgeSurvivorState::Paused) SetRunState(EHodgeSurvivorState::Playing);
}
void AHodgeSurvivorMode::RestartRun()
{
    UGameplayStatics::OpenLevel(this,TEXT("/Game/CodexText/Survivor/L_Survivor"));
}
void AHodgeSurvivorMode::ReturnToMenu()
{
    UGameplayStatics::OpenLevel(this,TEXT("/Game/CodexText/L_MainMenu"));
}
void AHodgeSurvivorMode::Tick(float Dt)
{
    Super::Tick(Dt);
    if (!Hero) return;
    auto* PC=UGameplayStatics::GetPlayerController(this,0);
    if (PC && PC->WasInputKeyJustPressed(EKeys::P)) TogglePause();
    if (PC && State==EHodgeSurvivorState::Upgrade)
    {
        if (PC->WasInputKeyJustPressed(EKeys::One)) ChooseUpgrade(0);
        else if (PC->WasInputKeyJustPressed(EKeys::Two)) ChooseUpgrade(1);
        else if (PC->WasInputKeyJustPressed(EKeys::Three)) ChooseUpgrade(2);
    }
    if (State!=EHodgeSurvivorState::Playing) return;
    Dt=FMath::Min(Dt,.1f);
    Elapsed+=Dt; HurtClock=FMath::Max(0.f,HurtClock-Dt);
    if (Elapsed>=RoundDuration) { SetRunState(EHodgeSurvivorState::Victory); return; }
    SpawnClock-=Dt; AttackClock-=Dt; PulseClock-=Dt;
    if (SpawnClock<=0)
    {
        SpawnClock=FMath::Max(.25f,SpawnInterval-Elapsed*.004f);
        for (int32 I=0;I<1+int32(Elapsed/55);++I) SpawnEnemy();
    }
    if (Elapsed>=NextElite) { NextElite+=35; SpawnEnemy(true); }
    FVector Player=Hero->GetActorLocation();
    for (int32 I=Enemies.Num()-1;I>=0;--I)
    {
        auto& Enemy=Enemies[I];
        if (!Enemy.Actor.IsValid()) { Enemies.RemoveAtSwap(I); continue; }
        FVector P=Enemy.Actor->GetActorLocation();
        FVector Delta=Player-P; Delta.Z=0;
        const float Distance=Delta.Size();
        const FVector Direction=Delta.GetSafeNormal();
        Enemy.Actor->SetActorRotation(FRotator(0,Direction.Rotation().Yaw-90,0));
        if (Distance>65) Enemy.Actor->SetActorLocation(P+Direction*FMath::Min(Enemy.Speed*Dt,Distance-65));
        if (Distance<(Enemy.bElite?115:80)) DamageHero(Enemy.bElite?22:11);
    }
    if (State!=EHodgeSurvivorState::Playing) return;
    if (AttackClock<=0)
    {
        int32 Target=INDEX_NONE; float Best=950*950;
        for (int32 I=0;I<Enemies.Num();++I)
            if (Enemies[I].Actor.IsValid())
            {
                const float D=FVector::DistSquared2D(Player,Enemies[I].Actor->GetActorLocation());
                if (D<Best) { Best=D; Target=I; }
            }
        if (Target!=INDEX_NONE)
        {
            AttackClock=AttackInterval;
            FVector End=Enemies[Target].Actor->GetActorLocation()+FVector(0,0,85);
            AActor* Beam=MakeShape((Player+End)*.5f,FVector(.065f,.065f,FVector::Distance(Player,End)/100),EnergyMaterial,.10f);
            if (Beam) Beam->SetActorRotation(FRotationMatrix::MakeFromZ(End-Player).Rotator());
            HitEnemy(Target,Damage);
        }
    }
    if (PulseLevel>0 && PulseClock<=0)
    {
        PulseClock=4;
        const float Radius=280+PulseLevel*55;
        MakeShape(Player-FVector(0,0,65),FVector(Radius/50,Radius/50,.035f),EnergyMaterial,.15f);
        for (int32 I=Enemies.Num()-1;I>=0;--I)
            if (Enemies[I].Actor.IsValid() && FVector::Dist2D(Player,Enemies[I].Actor->GetActorLocation())<Radius) HitEnemy(I,Damage*(1+PulseLevel*.4f));
    }
    for (int32 I=Orbs.Num()-1;I>=0;--I)
    {
        auto& Orb=Orbs[I];
        if (!Orb.Actor.IsValid()) { Orbs.RemoveAtSwap(I); continue; }
        FVector P=Orb.Actor->GetActorLocation();
        const float Distance=FVector::Dist2D(P,Player);
        if (Distance<PickupRadius) Orb.Actor->SetActorLocation(FMath::VInterpConstantTo(P,Player-FVector(0,0,50),Dt,800));
        if (Distance<70)
        {
            const int32 Value=Orb.Value; Orb.Actor->Destroy(); Orbs.RemoveAtSwap(I); GrantExperience(Value);
            if (State!=EHodgeSurvivorState::Playing) break;
        }
    }
    InterfaceClock+=Dt;
    if (InterfaceClock>.1f)
    {
        InterfaceClock=0;
        if (auto* UI=Cast<UHodgeSurvivorHUD>(Interface)) UI->Refresh(this);
    }
}
void AHodgeSurvivorMode::EndPlay(const EEndPlayReason::Type Reason)
{
    if (Interface) Interface->RemoveFromParent();
    Interface=nullptr; Enemies.Reset(); Orbs.Reset();
    Super::EndPlay(Reason);
}
