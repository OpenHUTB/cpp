# 5.打印调式信息

新创建一个名为 ConsoleLog 的新 Actor 子类（不需要在头文件中执行任何操作）。

**ConsoleLog.h**
```C++
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ConsoleLog.generated.h"
UCLASS()
class UNREALCPP_API AConsoleLog : public AActor
{
    GENERATED_BODY()
    public:
    // Sets default values for this actor's properties
    AConsoleLog();
    protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;
};
```





为了避免每次需要重启引擎编辑器使修改的代码生效，可以参考[热重载](./hot_reload.md)。