# 平台

Vite main target is Windows 64-bit on DirectX 12. Other platforms inherit 4.27 support.

Vite is a focused fork. The features that distinguish it &mdash; the NvRTX ray tracing suite, DDGI, RTXDI,
DLSS and its alternatives, HBAO+, hardware tessellation &mdash; are Deferred Path GPU features. Mean mainly for Desktop and 9th Gen Consoles

## 支持层次

| 平台 | 层次 | 说明                                                                  |
|---|---|------------------------------------------------------------------------|
| Windows 64-bit | **主要** | What everything is built and measured against                          |
| Windows 32-bit | Support files only | Required by Win64 tooling; not a shipping target                       |
| Linux | 已继承 | Stock 4.27 support. Ray tracing suite untested.                        |
| Mac | 已继承 | Stock 4.27 support. No DXR, so the ray tracing suite does not apply.   |
| Android | 已继承 | Inherits Vite updates to Forward Path.                                 |
| iOS / tvOS | 已继承 | Inherits Vite updates to Forward Path.                                 |
| HTML5 | 已继承 | Stock 4.27 support, long deprecated upstream                           |
| 控制台 | 未处理 | Requires platform SDKs Vite cannot make console related changes public |

"Inherited" means the stock 4.27 code is present and has not been deliberately broken, but Vite does not
test it and the fork's rendering features are unavailable or unvalidated there.


## 启用其他平台

Platform dependencies are opt-in at setup time.

Through [ViteSetup](ViteSetup.md), choose the **Win64 + Android** or **Win64 + Linux** profile, or use the
step-by-step assistant to toggle platforms individually. **Full setup** passes no `-exclude` argument to
`Setup.bat` and downloads everything.

For the debloat scripts, uncomment the relevant line in `devops\config.txt`:

```ini
# Win64 is always kept. Uncomment a platform to ALSO download and keep
# its dependencies during setup.
#KEEP_PLATFORM=Android
#KEEP_PLATFORM=IOS
#KEEP_PLATFORM=Linux
#KEEP_PLATFORM=Mac
#KEEP_PLATFORM=HTML5
```

<warning>
Adding a platform after the fact requires re-running dependency setup and rebuilding. Decide before your
first build if you can.
</warning>

## 图形 API

| API | 状态 | 笔记 |
|---|---|---|
| DirectX 12 | **主要** | Required for DXR, and therefore for everything ray-traced |
| DirectX 11 | 已支持 | HBAO+ works here; ray tracing does not |
| Vulkan | 已继承 | Stock 4.27 support; Vite's ray tracing suite is DXR-based |
| Metal | 已继承 | 仅 Mac |
| OpenGL ES | 已继承 | Mobile |

### 需要 DX12 什么

Everything ray-traced needs DXR, which in Vite means DirectX 12:

- [Ray-traced reflections](RT-Reflections.md), shadows, ambient occlusion and sky light
- [动态 DDGI](DDGI-Dynamic.md) and per-pixel ray-traced GI
- [RTXDI](RTXDI.md) and [path tracing](Path-Tracing.md), if you rebuild with `VITE_RT_PSO_DEBLOAT=0`
- [Translucency and caustics](RT-Translucency-And-Caustics.md), same condition

[FSR 4](Upscalers.md) is also DX12-only, independently of ray tracing.

### What works on DX11

- [HBAO4+](Ambient-Occlusion.md) &mdash; despite the console variable help text claiming DX11-only, it has a
  D3D12 implementation too
- [SMAA](Anti-Aliasing.md)
- [SSGI](SSGI.md)
- [Hardware tessellation](Tessellation.md)
- The [Callisto BRDF and Toon shading models](Shading-Models.md)

A DX11 target is viable if you are willing to give up ray tracing. It is the right choice for a project
whose [performance targets](Performance-Targets.md) do not leave budget for DXR.

## 硬件

See [System Requirements](System-Requirements.md) for development and runtime hardware.


## 打包

The [installed build](Installed-Builds.md) is produced with Development and Shipping game configurations
for Win64. If you need other configurations or platforms, edit the `RunUAT.bat` BuildGraph arguments:

```
-set:WithLinux=true
-set:GameConfigurations=Development;Test;Shipping
```

Each additional platform and configuration lengthens an already long build considerably.

## 另请参阅

- [系统要求](System-Requirements.md)
- [已安装好的构建](Installed-Builds.md)
- [ViteSetup 助手](ViteSetup.md)
- [光线追踪](Ray-Tracing.md)
- [Debloat 指南](Debloat-Guide.md)
