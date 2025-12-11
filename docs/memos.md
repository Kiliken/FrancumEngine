# MEMOS

My memos.

## Differences Between Khronos and Microsoft Coordinate Systems

APIs from **The Khronos Group** (such as **OpenGL** and **Vulkan**) utilize the **right-hand rule** for their coordinate system, where you use the right hand as a reference. On the other hand, APIs from **Microsoft** (such as **DirectX**) utilize the **left-hand rule**.

The primary difference lies in the direction of the **positive Z-axis**:

* In **OpenGL/Khronos**, the **positive Z-axis** points **out of the screen** (towards the viewer).
* In **DirectX/Microsoft**, the **positive Z-axis** points **into the screen** (away from the viewer).

---

## File Extension used in the Industry

### 🎨 Game Texture File Extensions
Game textures are essentially 2D images applied to 3D models. The choice of format depends on whether the file is for editing, preview, or runtime use.

👉 **Most common workflow:** PNG/TGA for import → DDS/KTX for runtime.

#### Texture Formats

| Purpose         | Common Formats         | Notes                                |
|-----------------|------------------------|--------------------------------------|
| Editing/Source  | PSD, EXR, TIFF         | Layers, high bit depth, lossless     |
| Preview/Import  | PNG, TGA, BMP          | Widely supported, lossless/compressed|
| Runtime/Engine  | DDS, KTX, ASTC, ETC2   | GPU-optimized, supports compression  |
| Engine-specific | Unity (.meta), Unreal (.uasset) | Internal asset containers |

---

### 🕹️ Game Model File Extensions
Game models are 3D meshes that define geometry, sometimes with animations or materials.

👉 **Most common workflow:** FBX for full assets, OBJ for simple meshes, GLTF for modern pipelines.

#### Model Formats

| Format | Use Case        | Strengths                                | Limitations             |
|--------|-----------------|------------------------------------------|-------------------------|
| OBJ    | Simple meshes   | Widely supported, stores geometry & UVs  | No animation support    |
| FBX    | Full-featured   | Industry standard, supports animation, materials, skeletons | Proprietary |
| GLTF/GLB | Modern pipelines | Compact, web-friendly, optimized for real-time engines | Still emerging |
| COLLADA (.dae) | Interchange | XML-based, tool compatibility         | Less common today       |
