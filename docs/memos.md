# MEMOS

My memos.

## Differences Between Khronos and Microsoft Coordinate Systems

APIs from **The Khronos Group** (such as **OpenGL** and **Vulkan**) utilize the **right-hand rule** for their coordinate system, where you use the right hand as a reference. On the other hand, APIs from **Microsoft** (such as **DirectX**) utilize the **left-hand rule**.

The primary difference lies in the direction of the **positive Z-axis**:

* In **OpenGL/Khronos**, the **positive Z-axis** points **out of the screen** (towards the viewer).
* In **DirectX/Microsoft**, the **positive Z-axis** points **into the screen** (away from the viewer).