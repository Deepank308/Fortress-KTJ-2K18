# Fortress-KTJ-2K18
This repo contains code for the Fortress Event of KTJ-2K18, implimented from scratch

Arena folder have some sample arenas and template for priority assignment.

SourceCode folder contains the main code files written in .cpp

To run the code : 
1. Run the startPointExtraction.cpp. It generates a .txt file for the coordinates of the bot and a pre-processed image of the arena.

2. Run the areanColorExt.cpp to detect the objects on the arena.

3. Run either of the mainRound*.cpp code to get the path and generate command to navigate the bot on the arena.



-rrt.cpp file has the imlpementation of "primitive" RRT planner written from scratch to check the code. It just generates the     waypoints for navigation of the bot.

-botComm.cpp is to check the communication with the arduino.

Here is a small video of the bot : 

![Output sample](https://github.com/Deepank308/Fortress-KTJ-2K18/blob/master/output.gif)

