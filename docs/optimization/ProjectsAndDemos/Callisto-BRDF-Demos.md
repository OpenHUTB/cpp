# Callisto BRDF 演示

两个角色场景演示了 Vite 的自定义 Callisto BRDF （双向反射分布函数）着色模型，该模型重新利用标准材质引脚来实现[逆反射](https://baike.baidu.com/item/%E9%80%86%E5%8F%8D%E5%B0%84/7584246)（retroreflection）、漫反射菲涅尔和终止线控制。


## Downloads

| Demo | Link |
|---|---|
| Male character | [Download](https://drive.google.com/file/d/133RwxvHT9jELgXWn338hieiAJUUeIoVq/view?usp=sharing) |
| Female character | [Download](https://mega.nz/file/5I1STApA#zmKqFu_8X1bYZPakb2VvAbCF-GPQhmR98Iot3QVJtRM) |

![](../../img/optimization/CallistoMale.png)

*使用 Callisto BRDF 着色模型渲染的男性角色。掠射角下，逆反射和漫反射菲涅尔效应共同作用。*


![](../../img/optimization/CallistoFemale.png)

*使用 Callisto BRDF 着色模型渲染的女性角色。“平滑终止线”技术柔化了面部和手臂曲面上的阴影线条。*

!!! 笔记
    **终止线**是指物体表面从“被照亮”区域过渡到“阴影”区域的那条分界线。在现实世界中，这条线往往是柔和、平滑的（因为光线有衍射和散射）。但在实时渲染的3D游戏中，由于计算精度的限制，这条线可能会变得非常生硬、锯齿状，甚至在模型表面断裂。


## Callisto BRDF 的作用

Unreal 默认的光照模型是一种优秀的通用型 BRDF，但在处理皮肤和角色渲染时表现平平。Callisto BRDF 是一项 Vite 扩展功能，它通过复用现有的材质引脚（而非新增引脚）来赋予你对角色渲染关键参数的直接控制权：


| 标准引脚 | 变为 |
|---|---|
| 不透明度（Opacity） | 逆反射（Retroreflection） |
| 各向异性（Anisotropy） | 漫反射菲涅尔（Diffuse Fresnel） |
| 自定义数据 0（Custom Data 0） | 平滑明暗交界线（Smooth Terminator） |
| 自定义数据 1（Custom Data 1） | 漫反射菲涅尔衰减（Diffuse Fresnel Falloff） |
| 环境光遮蔽（Ambient Occlusion） | 逆反射衰减（Retroreflection Falloff） |

首先需要关注的是平滑终结线。曲面上的硬阴影终结线是实时角色最明显的特征之一，直接控制它比通过法线贴图或光照技巧来处理更为经济且更可预测。


完整的引用和创作说明见[着色模型](../Rendering/Shading-Models.md)。


## 使用演示

打开角色材质，一个一个地更改引脚。重新映射的引脚会相互作用，每个引脚的效果在单独查看时比在默认组合中更清晰。

注意模型对掠射光的反应。逆向反射和漫反射菲涅尔在这里最强烈，这也是和默认点亮模型最明显的区别所在。


## 移植设置

这些演示中有用的部分是材质图。Callisto BRDF 是一种着色模型选择，所以采用它只是更换模型并重新连接受影响的引脚——没有需要启用的插件，也没有编译时开关。

每增加一个着色模型，就会增加着色器的组合。如果你只需要在角色上使用 Callisto，就只在角色上使用。参见[着色器编译和 PSO](../Performance/Shader-Compilation-And-PSO.md)。


## 另请参见

- [阴影模型](../Rendering/Shading-Models.md)
- [颜色管理](../Rendering/Color-Management.md)
- [头发渲染](../Rendering/Hair-Rendering.md)
