# 27. 漂浮的 Actor

接上一节教程，本教程创建一个新的 C++ Actor 子类并将其命名为 `FloatingActor`。在头文件中将创建四个浮点变量。我们将创建 `RunningTime`、`XValue`、`YValue` 和 `ZValue` 变量。

下面是最终的头文件代码

**FloatingActor.h**

```cpp
#pragma once
 
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingActor.generated.h"
 
UCLASS()
class UNREALCPP_API AFloatingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingActor();
 
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
 
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
 
	// declare our float variables 
	float RunningTime;
	
	UPROPERTY(EditAnywhere, Category = Movement)
	float XValue;
 
	UPROPERTY(EditAnywhere, Category = Movement)
	float YValue;
 
	UPROPERTY(EditAnywhere, Category = Movement)
	float ZValue;
	
	
};
```

接下来，我们将把所有的逻辑都放到 `Tick` 函数中。首先，让我们使用 `GetActorLocation` 在每一帧声明一个变量，该变量等于 actor 的当前位置。这将允许我们改变 actor 的 X, Y 和 Z 值，并在场景中移动。为了平滑的运动，我们将使用 `FMath:Sin` 来设置我们的 `DeltaHeight` 变量。

```cpp
float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
```

 接下来，我们将获取 actor 的位置 X、Y 和 Z 坐标，并将加上 `DeltaTIme * Value` 。在头文件中，我们让变量在任何地方都可以编辑，所以在编辑器中，我们可以轻松地调整角色移动的数量和方向。接下来，将 `RunningTime` 设置为 `DeltaHeight` 。最后将 actor 的位置设置为 `NewLocation` 。

下面是。cpp的最终版本。

**FloatingActor.cpp**

```cpp
#include "FloatingActor.h"
 
 
// Sets default values
AFloatingActor::AFloatingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
 
}
 
// Called when the game starts or when spawned
void AFloatingActor::BeginPlay()
{
	Super::BeginPlay();
	
}
 
// Called every frame
void AFloatingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
 
	// on every frame change location for a smooth floating actor
	FVector NewLocation = GetActorLocation();
	float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
	NewLocation.X += DeltaHeight *  XValue;
	NewLocation.Y += DeltaHeight *  YValue;
	NewLocation.Z += DeltaHeight *  ZValue;
 
	RunningTime += DeltaTime;
	SetActorLocation(NewLocation);
 
}
```

运行起来后的效果图如下

![](../img/floating_actor.gif)


## 参考

* [漂浮的 Actor【二十七】](https://panda1234lee.blog.csdn.net/article/details/119211224)
* [英文原地址](https://unrealcpp.com/floating-actor/)