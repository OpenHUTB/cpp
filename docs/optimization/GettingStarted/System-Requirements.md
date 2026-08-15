# System Requirements

<tldr>
<p>
Windows 10 or 11 x64. A ray-tracing capable GPU is recommended but not required &mdash;
<a href="DDGI-Static.md">Static DDGI</a> and the raster paths run on hardware with no DXR support at all.
Dynamic DDGI is designed to scale down to GTX 1060 6&nbsp;GB class GPUs.
</p>
</tldr>

Vite has two distinct sets of requirements: what it takes to *run* content built on the engine, and what it
takes to *build the engine itself*. They are very different, and the second one is much heavier.

## Running Vite content

Vite is explicitly designed to scale across a wide hardware range, which is the point of choosing DDGI over
Lumen in the first place.

| | Minimum                         | Recommended |
|---|---------------------------------|---|
| OS | Windows 10 x64                  | Windows 11 x64 |
| GPU (no ray tracing) | Any DirectX 11 capable GPU      | &mdash; |
| GPU (Dynamic DDGI) | GTX 1060 6&nbsp;GB or any RDNA2 | RTX 2060 / RX 6600 or better |
| GPU (full RT suite) | RTX 2060 / RX 6600              | RTX 3070 / RX 6700 XT or better |
| VRAM | 6&nbsp;GB                       | 8&nbsp;GB or more |

Dynamic DDGI works from GTX 1060 6&nbsp;GB class GPUs upward. Below that, or on any GPU with no ray tracing
support, use [Static DDGI](DDGI-Static.md), which bakes irradiance into probe volumes and has no runtime
ray tracing cost.

### Test Hardware

These are the configurations Vite is actively benchmarked and tuned against. The following Test Hardware is owned by main Devs of this fork.



| Vendor | Hardware                        | Why it matters                               |
|---|---------------------------------|----------------------------------------------|
| AMD | RX 6700 (RDNA2)                 | Closest desktop match to PlayStation 5       |
| AMD | Steam Deck LCD Van Gogh (RDNA2) | Handheld Class                               |
| AMD | RX 9600XT (RDNA4)               | Mid Tier GPU                                 |
| NVIDIA | RTX 2060 (Turing)               | Lower Bound for HW RT & DLSS                 |
| NVIDIA | RTX 3060 (Ampere)               | Matches Steam most common GPU                |
| NVIDIA | RTX 4080 Super (Ada Lovelace)   | Upper-bound reference for 4K native captures |

Testing is most useful on a 4K native monitor or TV(from 6700/RTX 4060), because Vite's whole argument is about holding native
resolution rather than upscaling from a lower internal one.

If you want to showcase results in your hardware (as in from the Demos), the `#showcase` channel on Discord is where testing is coordinated.

## Building the engine from source

Building is CPU-, RAM- and disk-bound. Treat these as practical guidance rather than hard cutoffs.

| | Practical minimum           | Comfortable                         |
|---|-----------------------------|-------------------------------------|
| CPU | 6 cores / 12 threads        | 16 cores / 32 threads or more       |
| RAM | 24&nbsp;GB                  | 32&nbsp;GB                          |
| Storage | SATA SSD with 50gb of Space | NVMe SSD 157GB of space (Full Size) |
| OS | Windows 10 x64              | Windows 11 x64                      |

An Unreal Engine 4.27 source tree with dependencies, intermediates and a built editor is very large &mdash;
budget well over a hundred gigabytes, and more again if you produce an installed build alongside it, since
that writes a second copy into `LocalBuilds\Engine\Windows\`. A mechanical hard drive will work but makes
every step painful; use an SSD.

Compile time scales almost linearly with core count. A full build on a Ryzen 9 9950X3D takes roughly
14 minutes. You can reclaim a meaningful amount of both time and disk space by excluding platforms you do
not target and stripping optional content &mdash; see [Debloat Guide](Debloat-Guide.md) and the setup
presets described in [Building from Source](Build-From-Source.md).

You also need a specific compiler and SDK. This is not optional and is covered separately in
[Toolchain Requirements](Toolchain-Requirements.md).

## See also

- [Toolchain Requirements](Toolchain-Requirements.md)
- [Building from Source](Build-From-Source.md)
- [Platform Support](Platforms.md)
- [Performance Targets](Performance-Targets.md)
