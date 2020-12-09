# Rasterizer

## Currently implemented
---
Rendering using vertices and their colors using barycentric coordinates  
Perspective with working rotations and translation matrix  
Basic clipping  
Camera translation/rotation (ZQSD/arrow keys for horizontal movement, Lshift and space for vertical movement)  
Translation and rotation functions (X & Y axis)  
Vertex shader (For color and textures only)  
Texture rendering using UV coords  
Perspective correction  
Hard coded lighting (ambient & specular)  
Vertex shader for lightning  

## WIP
---
Lighting with specular values and attenuation  
Pixel shading  
8 lights  
Optimisation and proper clipping  
Back-face culling  

## Resources
---
**Coding:**    
-[Various matrices, explanations on the rendering pipeline (PDF from stanford university)](https://stanford.edu/class/ee267/lectures/lecture2.pdf)  
-[Perspective correction (PDF from UC Davis Engineering)](https://web.cs.ucdavis.edu/~amenta/s12/perspectiveCorrect.pdf)  
-[Perspective correction (Stack overflow topic)](https://stackoverflow.com/questions/24441631/how-exactly-does-opengl-do-perspectively-correct-linear-interpolation)  

Wikipedia links:  
-[Viewing frustum](https://en.wikipedia.org/wiki/Viewing_frustum#/media/File:ViewFrustum.svg)  
-[Barycentric coordinates](https://en.wikipedia.org/wiki/Barycentric_coordinate_system#Conversion_between_barycentric_and_Cartesian_coordinates)  
-[Projection matrix](https://en.wikipedia.org/wiki/3D_projection#Perspective_projection)  
-[Texture mapping](https://en.wikipedia.org/wiki/Texture_mapping)  
-[Back-face culling](https://en.wikipedia.org/wiki/Back-face_culling)  
-[Phong shading](https://en.wikipedia.org/wiki/Phong_shading)  