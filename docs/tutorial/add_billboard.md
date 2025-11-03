# 添加一个广告牌组件


接上一节，在本教程中，我们将添加一个广告牌组件（Billboard）到我们的 Actor 对象上。在 UE4 编辑器中也可以很容易地添加组件，但是这次让我们通过编程来实现。


首先，我们将创建一个名为 `AddBillboardComp` 的新的 Actor 子类。请记住，如果您用不同的名字的话，请确保在使用了该头文件和 cpp 文件的地方都相应更改了名称。


在头文件中，我们将创建一个继承自 `UBillboardComponent` 类的变量。这将允许我们添加一个广告牌组件并使用它的属性。


**AddBillboardComp.h**
```C++
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AddBillboardComp.generated.h"
UCLASS()
class UNREALCPP_API AAddBillboardComp : publicAActor
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	AAddBillboardComp();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// declare point light comp
	UPROPERTY(VisibleAnywhere)
	class UBillboardComponent* MyBillboardComp;
};
```

在 cpp 文件中，我们将把广告牌组件添加到该 actor 类中。向 actor 类中添加任何组件的这个过程都是非常类似的。


如果想在 actor 中使用任何组件类，就必须在 cpp 文件中包含组件头文件。那么，让我们将公告牌组件文件添加到代码中。

```C++
#include "Components/BillboardComponent.h"
```

在本教程中，我们将在 actor 子类的构造函数中添加组件。这将确保组件在添加到场景时被添加到 actor 中。


创建公告牌组件的默认子对象：
```C++
MyBillboardComp = CreateDefaultSubobject<UBillboardComponent>(TEXT("Root Billboard Comp"));
```
并使其可见：
```C++
MyBillboardComp->SetHiddenInGame(false, true);
```
使公告牌组件成为根组件：
```C++
RootComponent = MyBillboardComp;
```

最后完整的 cpp 代码位于 [AddBillboardComp.cpp](../../src/tutorial/AddBillboardComp.cpp)


最终效果如图所示：


