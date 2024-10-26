Gouroud着色：在顶点着色器中使用phong光照模型计算顶点颜色，并作为像素颜色。<br>
Gouroud算法的着色效果(左图)：<br>
![Gouraud_Phong](https://github.com/xietinghao/LearnOpenGL/blob/main/Gouraud_Phong.png)<br>
效果：Phong模型中specular造成的高光不自然，原因是顶点着色器计算出顶点颜色后，在渲染时各点的高光值用的是各顶点的重心插值。而各顶点的高光插值无法准确还原片段内的高光，所以出现了和Phong着色的较大差异。<br>

<br>Phong着色(右图)：在片段着色器中使用phong光照模型计算顶点颜色，并作为像素颜色。<br>
