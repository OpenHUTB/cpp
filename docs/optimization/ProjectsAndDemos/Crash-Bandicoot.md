# Crash Bandicoot: Timetwister

<tldr>
<p>
A community remake of the Tomb Wader level from Crash Bandicoot 3: Warped, built on Unreal Engine 4.26.
A complete gameplay level rather than a lighting scene.
</p>
</tldr>

<img src="CrashBandicootRemake.png" alt="Tomb Wader level remake running in the engine" border-effect="line"/>

*A complete playable level rather than a lighting scene, which gives it a frame cost profile closer to a
real project.*

## Source

[dyanikoglu/CrashBandicoot-Timetwister](https://github.com/dyanikoglu/CrashBandicoot-Timetwister)

## Why it is here

Most of the projects in this section are technology demonstrations: a scene that exists to show one
rendering feature. This one is a playable level with movement, collision, hazards, collectibles and a
camera system, which makes it a different kind of reference.

It is useful for two things:

- **Gameplay-shaped profiling.** A real level has a frame cost profile that a lighting scene does not:
  actor ticks, animation, collision queries and gameplay logic competing with rendering. That is the shape
  most projects actually have. See [Profiling](Profiling.md).
- **Demonstrating 4.x sufficiency.** Stylized platformers are precisely the genre where UE5's headline
  features add cost without adding much, and where 4.27's lower base cost is the better trade. See
  [Why NvRTX 4.27](Why-NvRTX-427.md).

## Version note

<note>
The project targets Unreal Engine 4.26, not 4.27. Opening it in Vite will prompt an engine version
conversion. Expect to resolve some asset and API differences; the 4.26 to 4.27 gap is small but not zero.
</note>

This is a much easier conversion than coming from UE5, which requires the Asset Downgrader &mdash; see
[Migrating from UE5](Migrating-From-UE5.md).

## Licensing

This is a third-party community project, not a Vite deliverable. It is a fan remake of copyrighted
material; treat it as a reference to study rather than a source of assets to reuse. See the repository for
its own terms.

## See also

- [Profiling](Profiling.md)
- [Why NvRTX 4.27](Why-NvRTX-427.md)
- [First Project](First-Project.md)
