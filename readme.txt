[userid@machine sc20jk_A2]$ module add legacy-eng
[userid@machine sc20jk_A2]$ module add qt/5.15.2
[userid@machine sc20jk_A2]$ qmake -project QT+=opengl LIBS+=-lGLU
[userid@machine sc20jk_A2]$ qmake
[userid@machine sc20jk_A2]$ make

You should see a compiler warning about an unused parameter, which can be ignored.

To execute the renderer, pass the file name on the command line:

[userid@machine sc20jk_A2]$ ./sc20jk_A2

Design choices
When the user holds down a button the program blends into the next animation until further input is provided the character continues in the current animation cycle. If the user wants to stop the animation they will have to press the
back arrow key, which will blend into the rest pose and the character will stop moving. 
For the veering cycles I have decided to rotate the character by 90 degrees in each cycle. Thus when this cycle is ran, the character will run in a square, since the cycle is repeated until the user decides otherwise this implementation
makes sense as it would not look correct if the character turned and then ran in that direction as for that we would have to switch to the running animation after the first veering cycle. 

The character also adjusts for the height of the terrain by transorming the initial parentMatrix of the root bone by the height of the terrain. All the other bones are reliant on the root bones, thus this moves the whole character.

I have also chosen a relatively low speed for the character which does not reflect how fast the character looks to be running, as the animation for the run is quite fast but with a high speed it is difficult to follow the character with the camera and it is overall more pleaseant to inspect the animations when the character is not moving too quickly.