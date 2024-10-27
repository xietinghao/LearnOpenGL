# 多光源 
## 定向光源：
发射的光是平行光，一般为无限远处的物体，和场景内所有片元的入射角保持一致。
## 点光源：
发射的光不是平行光，一般为近处物体，和片元的入射角需要独立计算。
## 聚光源：
发射的光不是平行光，一般为特殊物件发出的光源（手电筒、聚光灯），和片元的入射角需要独立计算，同时需要计算入射角与聚光方向是否超出聚光半径。<br>
超出外径时聚光不计入渲染结果，位于内径和外径间用: ($\theta$ - $\gamma$)/($\epsilon$) 表示聚光强度值，<br>
($\theta$) 为入射角和聚光方向的余弦值，($\gamma$) 为聚光外半径角的余弦值，$\epsilon$ = $\phi$ - $\gamma$, $\phi$ 为聚光内半径的余弦值。
综上，聚光源的强度 $\mathbf{I}$ = clamp(($\theta$ - $\gamma$)/$\epsilon$, 0.0f, 1.0f)。
## 衰减
光的能量在传播中的衰减参数随传播距离的变化为 $\mathbf{attenuation}$ = 1.0/(k<sub>c</sub> + k<sub>l</sub> * distance + k<sub>q</sub> * distance * distance)，表现为先快速衰减，后缓慢减少。
# 多光源下的phong着色模型
$\dot$ 定向光源: $\mathbf{Color}$ = ambient + diffuse +specular ; <br>
$\dot$ 点光源: $\mathbf{Color}$ = (ambient + diffuse +specular) * $\mathbf{attenuation}$; <br>
$\dot$ 聚光源: $\mathbf{Color}$ = (ambient + diffuse +specular) * $\mathbf{attenuation}$ * $\mathbf{intensity}$; <br>
使用上述phong着色模型建立顶点和片段着色器，渲染的结果能表现出不同类型光源对着色的影响。<br>
<br>![MultiLights](https://github.com/xietinghao/LearnOpenGL/tree/main/MultiLights/MultiLights.gif)<br>
