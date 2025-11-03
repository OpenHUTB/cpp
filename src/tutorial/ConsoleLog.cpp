// Copyright (c) 2025 OpenHUTB at the Human University of Technology and Business (HUTB). This work is licensed under the terms of the MIT license. For a copy, see <https://opensource.org/licenses/MIT>.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, text)
#define printFString(text, fstring) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(TEXT(text), fstring))

#include "Tutorial/ConsoleLog.h"

// Sets default values
AConsoleLog::AConsoleLog()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AConsoleLog::BeginPlay()
{
	Super::BeginPlay();
	// 标注的方式：输出到控制台
	UE_LOG(LogTemp, Warning, TEXT("I just started running"));
	// 记录到屏幕上
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Screen Message"));
	FVector MyVector = FVector(200, 100, 900);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("My Location is: %s"), *GetActorLocation().ToString()));
	print("Hello Engine");
	printFString("My Variable Vector is: %s", *MyVector.ToString());
}

// Called every frame
void AConsoleLog::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

