# 切换 GameMmode

在运行游戏时决定Map使用哪一个GameMode。

## 流程

1. 调用`UGameplayStatics::OpenLevel()`函数
2. 传入Options参数"Game=XXX"，或者直接在LevelName里面拼接好`LevelName?Game=XXX`，其中XXX是GameMode别名
3. 在 ProjectSettings 中设置 GameModeClassAliases 配置GameMode别名

## 分析

可以在打开关卡时决定选用的GameMode，玄机就在 [UGameplayStatics::OpenLevel](https://github.com/OpenHUTB/engine/blob/421ec66f4d50f5dce22bb2d03fa35858dd7eec2b/Engine/Source/Runtime/Engine/Private/GameplayStatics.cpp#L778) 的参数里。


一般来讲，我们调用这个函数的时候会忽略掉Options参数，而其实OpenLevel里会将LevelName参数和Options参数拼接形成一个Url，格式大致为"LevelName?Options"。

```cpp
void UGameplayStatics::OpenLevel(const UObject* WorldContextObject, FName LevelName, bool bAbsolute, FString Options)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World == nullptr)
	{
		return;
	}

	const ETravelType TravelType = (bAbsolute ? TRAVEL_Absolute : TRAVEL_Relative);
	FWorldContext &WorldContext = GEngine->GetWorldContextFromWorldChecked(World);
	FString Cmd = LevelName.ToString();
	if (Options.Len() > 0)
	{
		Cmd += FString(TEXT("?")) + Options;
	}
	FURL TestURL(&WorldContext.LastURL, *Cmd, TravelType);
	if (TestURL.IsLocalInternal())
	{
		// make sure the file exists if we are opening a local file
		if (!GEngine->MakeSureMapNameIsValid(TestURL.Map))
		{
			UE_LOG(LogLevel, Warning, TEXT("WARNING: The map '%s' does not exist."), *TestURL.Map);
		}
	}

	GEngine->SetClientTravel( World, *Cmd, TravelType );
}
```


打开关卡时，在 [GameInstance](https://github.com/OpenHUTB/engine/blob/421ec66f4d50f5dce22bb2d03fa35858dd7eec2b/Engine/Source/Runtime/Engine/Private/GameInstance.cpp#L1186) 里会根据刚才传输的 Url 设置当前Map的GameMode，此时会解析字符串"Game="，而该参数即作为GameMode的参数。

```cpp
AGameModeBase* UGameInstance::CreateGameModeForURL(FURL InURL, UWorld* InWorld)
{
	// Init the game info.
	FString Options;
	FString GameParam;
	for (int32 i = 0; i < InURL.Op.Num(); i++)
	{
		Options += TEXT("?");
		Options += InURL.Op[i];
		FParse::Value(*InURL.Op[i], TEXT("GAME="), GameParam);
	}
```

而GameMode参数应该传入什么呢？其实解析到的GameMode字符串会作为UGameMapsSettings::GetGameModeForName()的参数被调用，在该函数内会检查它是否为 [GameMode的别名](https://github.com/OpenHUTB/engine/blob/421ec66f4d50f5dce22bb2d03fa35858dd7eec2b/Engine/Source/Runtime/EngineSettings/Private/EngineSettingsModule.cpp#L106) 。简单来说，Game=XXX需要填写的是GameMode的别名。

```cpp
FString UGameMapsSettings::GetGameModeForName(const FString& GameModeName)
{
	UGameMapsSettings* GameMapsSettings = Cast<UGameMapsSettings>(UGameMapsSettings::StaticClass()->GetDefaultObject());

	// Look to see if this should be remapped from a shortname to full class name
	for (const FGameModeName& Alias : GameMapsSettings->GameModeClassAliases)
	{
		if (GameModeName == Alias.Name)
		{
			// switch GameClassName to the full name
			return Alias.GameMode.ToString();
		}
	}
```

[GameMode的别名](https://dev.epicgames.com/documentation/zh-cn/unreal-engine/game-mode-and-game-state?application_version=4.27) 的说法来自于GameMapSettings的一个成员。
可以在 `DefaultEngine.ini` 文件的 `/Script/Engine.WorldSettings/` 部分中设置地图前缀（和 URL 法的别名）。这些前缀设置所有拥有特定前缀的地图的默认游戏模式。

```ini
[/Script/EngineSettings.GameMapsSettings]
+GameModeMapPrefixes=(Name="DM",GameMode="/Script/UnrealTournament.UTDMGameMode")
+GameModeClassAliases=(Name="DM",GameMode="/Script/UnrealTournament.UTDMGameMode")
```


## PythonAPI 

在 [PythonAPI/carla/source/libcarla/Client.cpp](https://github.com/OpenHUTB/hutb/blob/c95e2d44a30c1fc066e8c9de082c0ec764b1be8a/PythonAPI/carla/source/libcarla/Client.cpp#L225) 声明 load_world 函数的对应关系：
```cpp
.def("load_world", CONST_CALL_WITHOUT_GIL_3(cc::Client, LoadWorld, std::string, bool, rpc::MapLayer), (arg("map_name"), arg("reset_settings")=true, arg("map_layers")=rpc::MapLayer::All))
```

调用 [hutb/LibCarla/source/carla/client/Client.h](https://github.com/OpenHUTB/hutb/blob/c95e2d44a30c1fc066e8c9de082c0ec764b1be8a/LibCarla/source/carla/client/Client.h#L75) 中的LoadWord ，

```cpp
World LoadWorld(
    std::string map_name,
    bool reset_settings = true,
    rpc::MapLayer map_layers = rpc::MapLayer::All) const {
    return World{_simulator->LoadEpisode(std::move(map_name), reset_settings, map_layers)};
}
```

然后调用 [LibCarla/source/carla/client/detail/Simulator.cpp](https://github.com/OpenHUTB/hutb/blob/c95e2d44a30c1fc066e8c9de082c0ec764b1be8a/LibCarla/source/carla/client/detail/Simulator.cpp#L87) 中的 LoadEpisode 。

```cpp
EpisodeProxy Simulator::LoadEpisode(std::string map_name, bool reset_settings, rpc::MapLayer map_layers) {
    const auto id = GetCurrentEpisode().GetId();
    _client.LoadEpisode(std::move(map_name), reset_settings, map_layers);
```

然后调用 [LibCarla/source/carla/client/detail
/Client.cpp](https://github.com/OpenHUTB/hutb/blob/c95e2d44a30c1fc066e8c9de082c0ec764b1be8a/LibCarla/source/carla/client/detail/Client.cpp#L147) 中的 load_new_episode

```cpp
void Client::LoadEpisode(std::string map_name, bool reset_settings, rpc::MapLayer map_layer) {
    // Await response, we need to be sure in this one.
    _pimpl->CallAndWait<void>("load_new_episode", std::move(map_name), reset_settings, map_layer);
}
```

然后调用 [Unreal/CarlaUE4/Plugins/Carla/Source/Carla/Server
/CarlaServer.cpp](https://github.com/OpenHUTB/hutb/blob/c95e2d44a30c1fc066e8c9de082c0ec764b1be8a/Unreal/CarlaUE4/Plugins/Carla/Source/Carla/Server/CarlaServer.cpp#L308) 中的 SetMapLayer

```cpp
  BIND_SYNC(load_new_episode) << [this](const std::string &map_name, const bool reset_settings, cr::MapLayer MapLayers) -> R<void>
  {
    REQUIRE_CARLA_EPISODE();

    UCarlaGameInstance* GameInstance = UCarlaStatics::GetGameInstance(Episode->GetWorld());
    if (!GameInstance)
    {
      RESPOND_ERROR("unable to find CARLA game instance");
    }
    GameInstance->SetMapLayer(static_cast<int32>(MapLayers));

    if(!Episode->LoadNewEpisode(cr::ToFString(map_name), reset_settings))
    {
      FString Str(TEXT("Map '"));
      Str += cr::ToFString(map_name);
      Str += TEXT("' not found");
      RESPOND_ERROR_FSTRING(Str);
    }

    return R<void>::Success();
  };
```

然后调用 [Unreal/CarlaUE4/Plugins/Carla/Source/Carla/Game
/CarlaGameInstance.h](https://github.com/OpenHUTB/hutb/blob/c95e2d44a30c1fc066e8c9de082c0ec764b1be8a/Unreal/CarlaUE4/Plugins/Carla/Source/Carla/Game/CarlaGameInstance.h#L100) 的 SetMapLayer

```cpp
  UFUNCTION(Category = "Carla Game Instance", BlueprintCallable)
  void SetMapLayer(int32 MapLayer)
  {
    CurrentMapLayer = MapLayer;
  }
```




## 参考

* [绕开WorldSetting！切换GameMode的新姿势](https://zhuanlan.zhihu.com/p/660795062?share_code=1kQREJgxnStpH&utm_psn=1953537310399370643)
