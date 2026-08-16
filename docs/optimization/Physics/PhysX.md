# PhysX

Vite 集成了 <b>NVIDIA PhysX 3.4</b>，用于刚体模拟、碰撞查询、约束、车辆和骨骼物理。 Vite 通过现代编译器支持、固定时间步长模拟、NVIDIA Blast 和高计数实例物理子系统扩展了后端。

PhysX 通过虚幻引擎的标准物理框架公开。现有的引擎类型（例如主体实例、物理材质、物理资产、碰撞轮廓和约束组件）使用下面的 PhysX 场景。

## 核心模拟功能

### 刚体

- 静态、动态和运动参与者；
- 盒子、球体、胶囊、凸面和三角网格碰撞几何体；
- 具有摩擦力、恢复力和密度的物理材料；
- 重力、阻尼、力、脉冲和扭矩；
- 睡眠、清醒和主动身体跟踪；
- 快速移动物体的连续碰撞检测；
- 碰撞过滤和模拟/查询形状标志；
- 接触、重叠和唤醒/睡眠通知。

### 场景查询

PhysX 提供了整个引擎使用的碰撞查询路径：

- 光线投射;
- 形状扫描;
- 重叠;
- 单次命中和多次命中查询;
- 对象通道、跟踪通道和配置文件过滤;
- 运动、武器、导航和游戏系统使用的同步场景查询.

查询成本取决于冲突复杂性、宽相填充、过滤器回调和结果计数。对重复的游戏查询使用简单碰撞，并避免在仅需要第一次阻止命中时请求多次命中结果。

### 约束

该集成通过 Unreal 的约束框架支持固定、距离、铰链、球形和 D6 式约束。可用的控件包括：

- 线性和角度限制;
- 运动和驱动;
- 断裂力和断裂扭矩;
- 约束投影;
- 受约束物体之间的碰撞启用;
- 质量和惯性标度;
- 在 Physics Assets 中创作的骨骼约束链.

### Character and skeletal physics

PhysX supplies collision queries for Character Movement and rigid-body simulation for Physics Assets. This
includes ragdolls, physical animation, hit reactions, body welding and per-body collision configuration.

### Vehicles

Vite retains the UE4.27 PhysX vehicle stack, including wheel simulation, suspension, tire friction, engine,
gearing, differential and drivetrain configuration. Vehicle behavior is authored through the standard
vehicle and tire data assets used by UE4 projects.

## Destruction, cloth and Blast

The Vite physics stack includes the following NVIDIA systems:

| Feature | Function |
|---|---|
| APEX Destruction | Authored destructible meshes, support chunks, damage and fracture events |
| APEX Cloth | Vertex-painted cloth constraints and skeletal-mesh clothing simulation |
| NVIDIA Blast | Destruction assets and fracture workflows integrated alongside the PhysX backend |

See [Destruction and Cloth](Destruction-And-Cloth.md) for authoring and runtime guidance.

## Vite extensions

### Modern toolchain support

Vite's PhysX libraries and build files have been updated for newer MSVC and Clang toolchains, including the
Android NDK Clang path used by current Vite builds. Toolchain changes are validated with the physics test and
stress workloads before release.

### Fixed-timestep simulation

The optional fixed-timestep path decouples the simulation cadence from variable render frames and adds render
interpolation. It is intended for projects that require a stable simulation delta or reproducible captures.
See [Fixed Timestep](Fixed-Timestep.md) for compile-time setup and integration requirements.

### Instanced physics subsystem

The instanced subsystem represents large homogeneous rigid-body sets through instanced meshes instead of one
Actor and component hierarchy per body. It is intended for debris, shells, environmental objects and other
high-count simulations. See [Instanced Physics Subsystem](Instanced-Physics.md).

### Native actor path

Engine-level systems that already own compact simulation state can operate closer to native PhysX actors and
avoid unnecessary high-level Actor/component work. This is a specialized integration path: ownership,
lifetime, transform synchronization, collision filtering and teardown remain the caller's responsibility.

## Configuration

Physics settings are under **Project Settings > Engine > Physics**.

| Setting | Function |
|---|---|
| Default Gravity Z | World gravity; `-980.0` corresponds to 1 uu = 1 cm |
| Substepping | Divides a long frame into smaller simulation steps |
| Max Substep Delta Time | Maximum delta processed by one substep |
| Max Substeps | Upper bound on simulation steps performed for one frame |
| Simulate Skeletal Mesh on Dedicated Server | Enables skeletal rigid-body simulation on server targets |
| Default Degrees Of Freedom | Constrains motion for planar or limited-axis games |

### Substepping

Without substepping, the physics scene advances with the frame delta. Large or variable deltas can reduce
contact and constraint stability. Substepping performs multiple smaller advances when required; its CPU cost
increases with the number of executed substeps.

Choose `Max Substep Delta Time` from the fastest interaction that must remain stable, then set `Max Substeps`
to cap worst-case work. A cap prevents a slow frame from creating an unbounded simulation backlog.

For a fixed simulation cadence rather than frame-triggered substeps, use the
[fixed-timestep path](Fixed-Timestep.md).

## PhysX Visual Debugger

PhysX Visual Debugger can inspect a connected scene, including actors, shapes, contacts, constraints and
simulation state. Use a development build with PVD support, connect before reproducing the issue, and limit
capture duration when the scene contains many bodies.

Viewport collision visualization remains useful for confirming authored geometry and filtering before a
full PVD capture.

## Profiling

| Command | Shows |
|---|---|
| `stat physics` | Physics timing divided by engine phase |
| `stat game` | Physics cost in the context of game-thread work |
| `p.NumPhysScenes` | Number of active physics scenes |
| `show Collision` | Collision geometry in the viewport |

Profile with representative collision geometry, body counts, sleeping behavior and event generation. Record
physics milliseconds, body/shape counts, active-body count, solver settings, substeps and worker configuration
with every comparison.

For high body counts, compare standard Actor-based simulation with the
[instanced subsystem](Instanced-Physics.md) using the same shapes and solver settings.

## See also

- [Physics](Physics.md)
- [Fixed Timestep](Fixed-Timestep.md)
- [Destruction and Cloth](Destruction-And-Cloth.md)
- [Instanced Physics Subsystem](Instanced-Physics.md)
- [Physics Cube Bench](Physics-Cube-Bench.md)
- [PhysX Test](PhysXTest.md)
