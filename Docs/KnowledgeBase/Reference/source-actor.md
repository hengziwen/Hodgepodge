# Actor 源码参考

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

每个文件给出职责、项目内 include、有效定义与头文件声明摘录。摘录保留原行号，排除注释。

## HodgeActorBase.cpp

项目 Actor 基类扩展。

源码：[Source/Hodgepodge/Private/Actor/HodgeActorBase.cpp](../../../Source/Hodgepodge/Private/Actor/HodgeActorBase.cpp)

项目内直接 include（不是运行调用关系）：[Actor/HodgeActorBase.h](../../../Source/Hodgepodge/Public/Actor/HodgeActorBase.h)

定义候选（多行签名仅展示首行）：

- L24: `AHodgeActorBase::AHodgeActorBase()`
- L37: `void AHodgeActorBase::BeginPlay()`
- L48: `void AHodgeActorBase::Tick(float DeltaTime)`

## HodgeActorBase.h

项目 Actor 基类扩展。

源码：[Source/Hodgepodge/Public/Actor/HodgeActorBase.h](../../../Source/Hodgepodge/Public/Actor/HodgeActorBase.h)

有效头文件声明摘录（未展开宏，未求值预处理分支）：

```cpp
  12: #pragma once
  14: #include "CoreMinimal.h"
  15: #include "GameFramework/Actor.h"
  16: #include "HodgeActorBase.generated.h"
  30: UCLASS()
  31: class HODGEPODGE_API AHodgeActorBase : public AActor
  32: {
  33: 	GENERATED_BODY()
  35: public:
  43: 	AHodgeActorBase();
  45: protected:
  53: 	virtual void BeginPlay() override;
  55: public:
  67: 	virtual void Tick(float DeltaTime) override;
  68: };
```
