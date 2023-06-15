#### **Aim: Render a spinning, shaded cuboid on screen and use a web browser to display the output.**



**Query：**

**1. What is the purpose of the four arguments of the PerspectiveCamera function?**
**A:** it is the camera attributes, together these define the camera's viewing frustum.
<u>*the 1st*</u> is <u>Field of view angle in degree</u>, the angle between the upper and lower sides of the viewing frustum, which determines what is visible in the visualization;
<u>*the 2nd*</u> is <u>aspect ratio of the viewing window</u>, control the proportions of Three.js elements;
<u>*the 3rd*</u> is <u>near plane</u>, distance to the near clipping plane along the Z axis, things that are closer than near value won't be rendered;
<u>*the last*</u> is <u>far plane</u>, which must be greater than the current value of near plane, distance to the far clipping plane, things that are further than far value won't be rendered.



**2. What happens when you increase or decrease the numerical values?**
**A:** <u>1) Field of view angle:</u> increase -> view angle become wider -> cube become smaller;
                        					decrease -> view narrows -> cube become bigger;

<u>2) aspect ratio:</u> if the aspect ratio changes but the canvas remains unchanged, the rendering will be distorted.
    increase -> Objects become very narrow;
    decrease -> Objects become very wide and flat;

<u>3) near plane</u>: has no impact on the rendered view,
    if it is too big -> will not able to see the element, because it's been clipped from the scene;

<u>4) far plane:</u> has no impact on the rendered view,
    if it is too small -> will also not able to see the element;



**3. What is the purpose of the value of camera.position.z?**
**A:** the value represent the distance between the camera and the scene in the z-axis(z=0 plane).



**4. What happens when you increase or decrease the value?**
**A:** increase -> camera become closer -> bigger element;
	decrease -> camera become further -> smaller element.



**5. What happens when the values of the arguments to renderer.setSize() are changed?**
**A:** change the renderer.setSize() will change the width and size to the area we want to fill up, our screen



**6. What is the effect of changing the three arguments of BoxGeometry called in shaderCube()?**
**A:** three arguments of BoxGeometry is:

<u>1) width</u> -> the length of the edges parallel to the X axis;

<u>2) height</u> -> the length of the edges parallel to the Y axis;

<u>3) depth</u> -> the length of the edges parallel to the Z axis;

changing these will changing the size of the cube.



**7. How will you speed up the cuboid’s motion?**
**A:** incerase the incremental values of the mesh.rotation.x and mesh.rotation.y;
e.g. mesh.rotation.x += 0.03;
     mesh.rotation.y += 0.03;



**8. How would you change the cuboid’s colour?**
**A:** Change the value of gl_FragColor set by fragment shader;
e.g. by set gl_FragColor = vec4(1, 1, 1, 1.0); can set the cuboid’s color to white.