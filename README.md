# arduboy-hungry-caterpillar
Arduboy game based on the old snake game

Based on the old game Snake! You are a hungry caterpillar and must move around (D-Pad) eating cabbages.<br/>
With each cabbage you eat you grow longer making it harder not to crash into the walls or yourself (game over!)<br/>
You will find different coloured cabbages, these are bonuses and will reduce your tail length, and slow you down, making things easier!<br/>
There is a second level, where the play area gets reduced amking it even harder!
<h3>Special notes for compiling</h3>
I've included a hex file if you just want to play.<br/>
If you want to compile it, you may need some extra steps:<br/>
This is because I use C++ Vectors in the code, which do not come with the standard Arduboy library.<br/>

This uses C++ Standard Library from <a href='https://github.com/maniacbug/StandardCplusplus'>https://github.com/maniacbug/StandardCplusplus</a>
<br/>download, unpack into your sketchbook \libraries folder
<br/>also you need to downgrade "Arduino AVR Boards" in your IDE for the Std library to work:
<br/>Arduino IDE: Tools > Board > Boards Manager
<br/>Wait for downloads to finish.
<br/>When you move the mouse pointer over "Arduino AVR Boards", you will see a "Select version" dropdown menu appear. 
<br/>Select "1.6.20". Click "Install".
<br/>Wait for installation to finish. Click "Close". Now you can compile this game.
