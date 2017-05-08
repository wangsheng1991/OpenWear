# OpenWear

### 基于sdm的虚拟试戴

<p align="left">
    <img src="figures/OpenWear.png">
</p>

实现了如下功能：

* 1.将OpenCV获取的Mat格式数据通过纹理贴图的方式绘制到OpenGL窗口上

* 2.使用sdm检测和跟踪人脸的68个关键点

* 3.加载3D眼镜模型

* 4.渲染的眼镜随人脸的移动而移动，随人脸的远近而缩放，随人脸的旋转而旋转

特性：VS2013编译通过，仅依赖OpenCV和OpenGL库，可实时运行