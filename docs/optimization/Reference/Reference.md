# Reference

<tldr>
<p>
Lookup material: console variables, terminology and frequently asked questions. Use these when you know
what you are looking for; use the feature sections when you do not.
</p>
</tldr>

## In this section

| Topic | Covers |
|---|---|
| [Console Variables](./Console-Variables.md) | Vite-specific and commonly needed console variables, grouped by subsystem |
| [Compile-Time Switches](../Performance/Compile-Time-Switches.md) | The `VITE_*` build switches and what they change |
| [Glossary](./Glossary.md) | Terminology used throughout these docs |
| [FAQ](./FAQ.md) | Questions that come up repeatedly |

## Quick answers

<deflist>
<def title="A console variable I set does nothing">
Most likely the feature is compiled out. <code>VITE_RT_PSO_DEBLOAT</code> defaults to <code>1</code> and
removes the shader permutations for RTXDI, path tracing, ray-traced translucency, caustics and per-pixel
ray-traced GI. The variables still set; nothing renders. See
<a href="../Performance/Compile-Time-Switches.md">Compile-Time Switches</a>.
</def>
<def title="A feature is missing from the editor">
Check whether its plugin is enabled. Most bundled plugins default to off &mdash; see
<a href="../Plugins/Bundled-Plugins.md">Bundled Plugins</a>.
</def>
<def title="The build fails on a fresh clone">
Usually a toolchain mismatch or missing .NET Framework 4.5 reference assemblies. See
<a href="../GettingStarted/Build-Troubleshooting.md">Build Troubleshooting</a>.
</def>
<def title="Shaders behave as though my change did not happen">
Stale cache. See <a href="../Tools/Cache-Management.md">Cache Management</a>.
</def>
</deflist>

## See also

- [入门](../GettingStarted/Getting-Started.md)
- [性能和优化](../Performance/Performance.md)
- [贡献](../Contributing/Contributing.md)
