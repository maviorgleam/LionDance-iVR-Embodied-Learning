// Fill out your copyright notice in the Description page of Project Settings.

#include "LionGameInstance.h"
#include "Kismet/GameplayStatics.h"

void ULionGameInstance::StartReplayRecording() { StartRecordingReplay(ReplayName, ReplayName); }
void ULionGameInstance::StopReplayRecording()  { StopRecordingReplay(); }
void ULionGameInstance::PlayLastReplay()       { PlayReplay(ReplayName); } // 自动重载关卡进回放
void ULionGameInstance::ReturnToMenuLevel()    { UGameplayStatics::OpenLevel(this, MainLevelName); }

