# 实例化渲染
实例化渲染是指在使用顶点着色器与片段着色器进行渲染时，重复调用同一个着色器程序进行相同流程的渲染。在流程一致的渲染过程中，改变顶点的属性数据，达到每一次渲染生成不同结果的效果。<br>
实例化渲染在OpenGL中的实现方式是调用 glDrawArraysInstanced ，第四个参数指定了渲染的示例数量。不同实例所具有的不同顶点属性，可以通过uniform或者顶点属性数组传递给着色器。<br>
使用uniform数组传递顶点数据，需要形如 glUniform1f(glGetUniformLocation(shaderID, valueName), value) 的函数。
## uniform数组的渲染瓶颈&
OpenGL在着色器中使用uniform数组需要GPU和CPU的通信开销(OpenGL的函数调用将cpu处理好的uniform数据传递给gpu)，当实例的渲染数量级很大时，通信开销会非常大。<br>
解决GPU和CPU的高通信开销的办法是一次性传递所有需要渲染的顶点数据给GPU，后续渲染只由GPU参与，因为不需要通信的开销，GPU在并行渲染这些实例时的速度可以非常快。<br>
OpenGL用buffer的方式传递大块的内存数据buffer，把顶点属性绑定到buffer的某偏移地址上，GPU就能在传递过来的buffer中找到渲染所需的数据。
在定义顶点属性时，使用 glVertexAttribDivisor 声明VAO(顶点属性缓冲)中的某一数据为实例化数据，需要根据实例的渲染数量进行更行。
如下代码:
```
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
  glVertexAttribDivisor(3,1);
```
指示了VAO第4个(从0开始)属性数据占据4个GL_FLOAT内存空间，偏移地址量为0，基地址为当前绑定在GL_ARRAY_BUFFER上的VBO的首地址。
并且最后一行声明了使用该VAO渲染实例时，每1个实例就要更新一次第4个属性数据。
通过提前分配VBO内存空间，并将实例化渲染时需要更新的顶点数据放进VBO，高效渲染大批量实例数据。
