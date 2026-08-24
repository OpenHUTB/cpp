# 着色模型

Vite 为 4.27 版本的默认着色模型集新增了三种着色模型：**Callisto BRDF** 用于高保真度的皮肤和角色表面，**Toon** 用于风格化的非写实渲染，以及 **Lit Reactive**。每种模型都会重新利用现有的材质引脚，因此在创建之前请务必阅读引脚映射表。

着色模型可以从材质详情面板中的**着色模型（Shading Model）** 下拉菜单中选择，也可以通过**着色模型（Shading Model）** 材质表达式逐像素地进行驱动。所有 Unreal Engine 4.27 的默认着色模型仍然可用。


## 新增着色模型

| 着色模型 | 显示名称 | 用途 |
|---|---|---|
| `MSM_CallistoBRDF` | Callisto BRDF | 高保真角色和皮肤表面，支持单瓣和双瓣 GGX 高光 |
| `MSM_Toon` | 卡通（Toon） | 风格化的赛璐珞着色，灵感源自《罪恶装备》 |
| `MSM_LitReactive` | 响应式光照（Lit Reactive） | 支持各向异性和切线的光照表面变体 |

## Callisto BRDF

Callisto BRDF 提供单瓣和双瓣 GGX 镜面反射，并带有镜面菲涅尔衰减，旨在改善默认光照模型单瓣镜面反射效果过于粗糙的角色和皮肤。[Callisto BRDF 演示项目](../ProjectsAndDemos/Callisto-BRDF-Demos.md)展示了其在男性和女性角色表面上的应用。


### 引脚映射

该模型重新利用了几个标准材质引脚。材质编辑器会在选择着色模型后自动重新标记这些引脚，但了解其映射关系有助于读取材质图或编写材质函数。


| 标准引脚 | Callisto BRDF 标签 | 用途 |
|---|---|---|
| 不透明度（Opacity） | **逆反射（Retroreflection）** | 逆反射响应强度 |
| 环境光遮蔽（Ambient Occlusion） | **逆反射衰减（Retroreflection Falloff）** | 逆反射项的衰减曲线 |
| 各向异性（Anisotropy） | **漫反射菲涅尔（Diffuse Fresnel）** | 漫反射菲涅尔项 |
| 自定义数据 0（Custom Data 0） | **平滑明暗交界线（Smooth Terminator）** | 柔化阴影边界处的明暗交界线 |
| Custom Data 1（自定义数据 1） | **漫反射菲涅尔衰减（Diffuse Fresnel Falloff）** | 漫反射菲涅尔项的衰减曲线 |

### Callisto 高级参数

**Callisto 高级参数**是一个额外的自定义输出节点，它公开了不符合标准引脚集的参数：

| 输入 | 用途 |
|---|---|
| 镜面菲涅尔衰减（Specular Fresnel Falloff） | 镜面菲涅尔响应的衰减曲线 |

将此节点添加到材质图的方式与其他自定义输出相同。它会编译为 `CallistoAdvancedParams` 材质函数。


### 作者说明

平滑终止线控制是最值得理解的。标准 GGX 着色产生的硬光终止线是曲面有机表面上一个众所周知的瑕疵，尤其是在掠射光照射的表面上，着色边界显得异常锐利。平滑终止线输入可以柔化这种过渡，而不会使整体着色变得扁平。

逆反射和漫反射菲涅尔控制可以让你调整那些在掠射光照射下会变亮的表面——例如皮肤、细纤维织物、灰尘表面——而无需使用次表面模型。


## 卡通（Toon）

卡通着色模型提供赛璐珞着色，并可对着色渐变、镜面反射形状和着色颜色进行明确的艺术控制，其灵感来自 Arc System Works 在《罪恶装备》中的方法。


### 引脚映射

卡通渲染模型比其他任何模型都更频繁地重新利用引脚，因为大多数标准 PBR 参数在卡通渲染下都没有意义。

| 标准引脚 | 卡通标签 | 用途 |
|---|---|---|
| 金属度（Metallic） | **镜面亮度（Specular Brightness）** | 镜面高光的强度 |
| 镜面（Specular） | **镜面大小（Specular Size）** | 镜面高光的大小 |
| 粗糙度（Roughness） | **阴影偏移（Shadow Bias）** | 调整光照和阴影之间的着色阈值 |
| 各向异性（Anisotropy） | **柔和度（Softness）** | 光照/阴影过渡的柔和度 |
| 次表面颜色（Subsurface Color） | **阴影颜色（Shaded Color）** | 阴影区域使用的具体颜色 |

**阴影颜色**是关键控制项。它并非通过加深底色来生成阴影颜色，而是直接指定阴影颜色，这使得卡通渲染艺术呈现出其特有的非物理阴影色彩变化——例如，皮肤呈现紫色而非深棕色。

**阴影偏向**和**柔和度**共同定义了渐变效果。偏向值接近中间且柔和度接近于零时，会产生明显的双色调效果；柔和度越高，渐变效果越明显。


## 响应式光照（Lit Reactive）

一种支持各向异性、切线输​​入和自定义数据 0 的光照表面变体。在不透明度和不透明度遮罩优化路径中，它与默认光照 (Default Lit) 一起被视为基本不透明表面，这意味着它避免了其他光照模型触发的遮罩和仅半透明属性编译。

## 着色器排列组合

每个使用的着色模型都会增加着色器排列组合，这会增加编译时间、PSO 数量和磁盘空间。这是一个实际的预算问题，而非理论上的担忧——排列组合的增长正是 Vite 存在的目的之一，旨在避免性能下降，正如 [UE4 与 UE5 成本分析](../EngineOverview/UE4-Versus-UE5-Cost-Analysis.md)中所述。

**注意：** 使用美术指导所需的最小着色模型集。一个只包含默认光照 (Default Lit)、角色使用 Callisto BRDF 且不包含其他任何着色器的项目，其编译速度、PSO 数量和加载速度都将比在其材质库中使用六个模型的项目更快。


请参阅[着色器编译和 PSO](../Performance/Shader-Compilation-And-PSO.md)了解 Vite 如何管理排列组合，并注意运行时 PSO 在正式版配置中已进行精简。


## 另请参阅

- [渲染](./Rendering.md)
- [Callisto BRDF 演示](../ProjectsAndDemos/Callisto-BRDF-Demos.md)
- [头发渲染](./Hair-Rendering.md)
- [着色器编译和 PSO](../Performance/Shader-Compilation-And-PSO.md)
- [Colour Management](./Color-Management.md)
