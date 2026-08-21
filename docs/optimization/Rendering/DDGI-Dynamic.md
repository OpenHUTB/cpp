# Dynamic DDGI

<tldr>
<p>
Real-time ray-traced irradiance probe volumes. Enable with
<code>r.GlobalIllumination.ExperimentalPlugin 1</code>, then place DDGI volumes in the level.
Noise-free by construction, fully dynamic, and roughly 2x the frame rate of hardware Lumen in
comparable scenes.
</p>
</tldr>

Dynamic Diffuse Global Illumination is Vite's primary global illumination solution and the feature the fork
is best known for. This page covers how it works, how to set it up, and how to tune it.

![](../../img/optimization/DDGIDirectOnly.png)

*阁楼内部只有直射光，阳光照射不到的地方一片漆黑。*

*阁楼内部只有直射光，阳光照射不到的地方都是黑色的，因为没有任何东西能将光线传递到房间的各个角落。*


![](../../img/optimization/DDGIDirectPlusGI.png)

*同样是阁楼内部，启用动态DDGI后，间接反射光充满整个房间。天花板、左侧的箱子以及屋檐下的空间完全由反射光照明——由于探头辐照度本身就是平滑的，因此无需进行降噪处理。*


## How it works

DDGI places a three-dimensional grid of probes across a volume. Each frame, a budget of rays is traced from
each probe into the scene; the resulting radiance is accumulated into per-probe irradiance and depth
textures, and stored using spherical harmonics.

Two properties follow from that representation, and they explain almost everything about DDGI's behaviour:

**It is noise-free.** Because irradiance is accumulated and filtered into a smooth basis over many frames
rather than sampled per-pixel per-frame, the output has no stochastic noise and needs no denoiser. That is
why DDGI does not exhibit the boiling, ghosting and temporal instability associated with denoised
ray-traced GI.

**It is coarse.** Spatial resolution is bounded by probe spacing. Lighting detail finer than the probe grid
does not exist in the representation. This is the reason [SSGI](./SSGI.md) is recommended alongside it.

The per-probe depth information is what prevents light leaking through thin geometry &mdash; probes know how
far away the nearest surface is in each direction and can reject contributions that would have to pass
through a wall. This is why DDGI leaks less than software Lumen.

![](../../img/optimization/DDGIProbeVisualisation.png)

*编辑器视口中，DDGI探测球体在整个内部空间中可视化显示。*

*编辑器中的探针可视化功能。每个球体代表一个探针，显示其存储的辐照度，这使得探针间距和任何错位的探针都能立即显示出来。*


## Vite's integration

Vite's DDGI is not the stock 4.27 launcher plugin. It inherits NvRTX's engine-side integration, which
reaches into the ray tracing pipeline rather than sitting alongside it. The practical differences:

- **Probe-based ray-traced reflections.** Reflection rays can sample probe irradiance for their secondary
  bounce instead of returning black or falling back to a cubemap. This substantially improves RT reflection
  quality and is one of the larger visual wins in the fork. See
  [Ray-Traced Reflections](./RT-Reflections.md).
- Continued optimisation work on the DDGI update path as part of each release.
- Composition with the rest of the RT effect suite, so DDGI, RTXDI and RT reflections can all be active.

> Do not install the launcher 4.27 DDGI plugin into a Vite project. The engine already ships DDGI and the
> two conflict. The standalone
> [DDGI 1.1.5 plugin](https://github.com/GapingPixel/UE4-RTXGI-1.1.5-Latest-Official) is only for team
> members working in a stock launcher 4.27 install alongside a Vite project.
>
{style="warning"}

## Setting up

<procedure title="Set up Dynamic DDGI in a level" id="setup-ddgi">
    <step>
        Confirm ray tracing is enabled for the project. DDGI needs DXR support.
    </step>
    <step>
        Enable the plugin path with <code>r.GlobalIllumination.ExperimentalPlugin 1</code>, either from the
        console, from code, or in <code>DefaultEngine.ini</code>.
    </step>
    <step>
        Place a DDGI volume actor so that it encloses the playable space you want lit. Volumes cover
        volume, not surfaces &mdash; think about where the player and dynamic objects can go, not about
        where the walls are.
    </step>
    <step>
        Set probe counts per axis. Start coarse and increase only where you can see the difference; probe
        count drives both memory and per-frame ray cost.
    </step>
    <step>
        Enable <a href="./SSGI.md">SSGI</a> with <code>r.SSGI.Enable 1</code> to recover the contact detail
        the probe grid cannot represent.
    </step>
    <step>
        Verify in motion, not in a still frame. DDGI's advantages are temporal &mdash; a screenshot will not
        show you the stability difference against a denoised technique.
    </step>
</procedure>


![](../../img/optimization/ProjectSettingsRHI.png)

*项目设置显示默认 RHI 设置为 DirectX 12，并勾选了 DirectX 11 和 12 SM5。*

*第一步，也是很多人会忽略的一步。**项目设置 → 平台 → Windows → 目标 THI**必须设置为 DirectX 12；4.27 版本中的光线追踪仅支持 DX12。*


![](../../img/optimization/DDGIEnablePlugin.png)

*在内置渲染下启用 NVIDIA RTX 全局光照插件的插件对话框*

*GI 插件路径位于**内置”→“渲染**下。Vite 引擎自带此插件，因此这是引擎内置插件，而不是启动器 4.27 插件——请勿将其与后者同时安装。*


![](../../img/optimization/DDGIVolumeEditor.png)

*在编辑器视口的关卡中放置一个 DDGI 体积 Actor。体积覆盖空间，而非表面——根据摄像机和动态物体实际可以到达的位置来调整体积大小。*


### Volume settings

![](../../img/optimization/DDGISettingsVolume.png)

*DDGI 体积设置面板*


![](../../img/optimization/DDGISettingsProbes.png)

*DDGI探头设置面板显示每轴计数和探头间距控制*


![](../../img/optimization/DDGISettingsLighting.png)

*DDGI体积Actor的体积、探针和照明光照设置。每个轴的探针数量是影响内存占用和每帧光线消耗的关键参数。*


## Tuning

The settings that matter most, roughly in order of impact:

**Probe density.** The dominant cost and quality control. Denser grids resolve smaller lighting features and
cost proportionally more in both rays traced and memory. Interior spaces with lots of small rooms need more
probes than open exteriors.

![](../../img/optimization/DDGIProbeDensity.png)

*探针密度对比图显示了照明细节如何随探针间距变化。*

*探针密度与解析出的照明细节之间的关系。在任何质量设置下，图像中都不存在比探针间距更精细的细节——这就是 [SSGI](./SSGI.md) 的作用所在。*


**Volume placement and count.** Several tightly-fitted volumes usually beat one large loose one. A volume
that spans a whole level at low density wastes probes on solid geometry and starves the spaces that matter.

**Rays per probe.** Trades convergence speed against per-frame cost. Lower counts converge more slowly,
which shows up as lag when lighting changes rapidly &mdash; a door opening onto a bright exterior, for
instance.

**Hysteresis / update rate.** How quickly probes accept new information. Faster response reduces lag but
increases temporal variation.

**Normal and view bias.** The standard controls for trading light leaking against contact shadow darkening.
If you see light bleeding through thin walls, increase bias; if contact areas look detached, reduce it.

## Reference material

NVIDIA's original documentation and talks remain the best deep reference for the technique itself.

- [RTXGI plugin README](https://github.com/GapingPixel/UE5-PhysX-Vite/tree/ue5Vite-release/Engine/Plugins/Runtime/Nvidia/RTXGI)
- [Dynamic Diffuse Global Illumination, GDC presentation](https://developer.download.nvidia.com/video/gputechconf/gtc/2019/presentation/s9900-irradiance-fields-rtx-diffuse-global-illumination-for-local-and-cloud-graphics.pdf)

Ray-Traced Irradiance Fields:

[![Ray-Traced Irradiance Fields](https://img.youtube.com/vi/KufJBCTdn_o/0.jpg)](https://www.youtube.com/watch?v=KufJBCTdn_o)

Sample scene setup, and combined usage with SSGI:

[![DLSS and RTXGI Unreal Engine 4 Plugin: Settings Deep Dive](https://img.youtube.com/vi/ZefvmV1pdP8/0.jpg)](https://www.youtube.com/watch?v=ZefvmV1pdP8&t=1210s)

Plugin settings deep dive:

[![NVIDIA RTXGI Unreal Engine 4 Plugin: Settings Deep Dive](https://img.youtube.com/vi/U57_a3lGKOo/0.jpg)](https://www.youtube.com/watch?v=U57_a3lGKOo)

Full official playlist:

[![Getting Started with NVIDIA RTXGI Unreal Engine 4 Plugin](https://img.youtube.com/vi/bxEVMnyXxqw/0.jpg)](https://www.youtube.com/playlist?list=PL4FII4B-zM0f5h75klcOfiO1v_atlp8Ky)

There is also an internal
[DDGI reference document](https://docs.google.com/document/d/1kdZGRV6bRNjNvec1OzzEJd64NtLBDZ8hzQvFVzB2GfI/edit?tab=t.0)
maintained by the team.

## Sample scenes

- [Tech Demo Project](../ProjectsAndDemos/Tech-Demo-Project.md) &mdash; includes NVIDIA's official DDGI Cornell Box sample and a
  high-end DDGI plus SSGI cave scene.
- [Stylized Raytracing Demo](../ProjectsAndDemos/Stylized-Raytracing-Demo.md) &mdash; the 4K120 stylised target.
- [Abandoned Apartment](../ProjectsAndDemos/Abandone-Apartment.md) and [Attic Scene](../ProjectsAndDemos/Attic-Scene.md) &mdash; NVIDIA's RTGI
  showcase scenes.

## See also

- [Global Illumination](./Global-Illumination.md)
- [Static DDGI](./DDGI-Static.md)
- [SSGI](./SSGI.md)
- [Ray-Traced Reflections](./RT-Reflections.md)
- [Performance Targets](../EngineOverview/Performance-Targets.md)
