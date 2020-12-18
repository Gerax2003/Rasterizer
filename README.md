# Rasterizer

## Currently implemented
---
Rendering using vertices and their colors using barycentric coordinates  
Perspective with working rotations and translation matrix  
Basic clipping  
Camera translation/rotation (ZQSD/arrow keys for horizontal movement, Lshift and space for vertical movement)  
Translation and rotation functions  
Vertex shader (For color and textures only)  
Texture rendering using UV coords  
Perspective correction  
Vertex shader for lightning  
8 functionnal lights with ambient and diffuse lighting and attenuation  
Pixel shading  

## WIP
---
Lighting with specular values  
Optimisation and proper clipping  
Back-face culling  

## External libraries
---
GLFW (Display in ImGui)  
ImGui (Parameters window/display window)  
Tinyobjloader (.obj loading)  
stbimage (texture loading)  

## Resources
---
**Coding:**    
-[Various matrices, explanations on the rendering pipeline (PDF from stanford university)](https://stanford.edu/class/ee267/lectures/lecture2.pdf)  
-[Perspective correction (PDF from UC Davis Engineering)](https://web.cs.ucdavis.edu/~amenta/s12/perspectiveCorrect.pdf)  
-[Perspective correction (Stack overflow topic)](https://stackoverflow.com/questions/24441631/how-exactly-does-opengl-do-perspectively-correct-linear-interpolation)  
-[Light attenuation using constant, linear and quadratic values (OGLdev)](http://ogldev.atspace.co.uk/www/tutorial20/tutorial20.html)  
-[Specular light (OGLdev)](http://ogldev.atspace.co.uk/www/tutorial19/tutorial19.html)  

**Wikipedia links:**  
-[Viewing frustum](https://en.wikipedia.org/wiki/Viewing_frustum#/media/File:ViewFrustum.svg)  
-[Barycentric coordinates](https://en.wikipedia.org/wiki/Barycentric_coordinate_system#Conversion_between_barycentric_and_Cartesian_coordinates)  
-[Projection matrix](https://en.wikipedia.org/wiki/3D_projection#Perspective_projection)  
-[Texture mapping](https://en.wikipedia.org/wiki/Texture_mapping)  
-[Back-face culling](https://en.wikipedia.org/wiki/Back-face_culling)  
-[Phong shading](https://en.wikipedia.org/wiki/Phong_shading)  