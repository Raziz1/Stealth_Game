# Stealth_Game 🥷🏾
A quick stealth game with the A* pathfinding algorithm implemented. It was designed using C++ and the SFML graphics library

<p> 
    <img width=50% height=50% align='Right' src="https://github.com/Raziz1/Stealth_Game/blob/main/stealth_game.png? raw=true" >
</p> 

## Libraries 📚
* [SFML](https://www.sfml-dev.org/)
* [CodeBlocks](https://www.codeblocks.org/)

## Overview 📝
The following project is a quick stealth game designed in the CodeBlocks IDE using C++ and the SFML graphics library. It incorporates the A* Pathfinding algorithm for the bot. The bot continuously finds random paths across the map and moves to it using the shortest path. Once the player becomes visible in the flashlight it sets the players last position as the path to go to. The collision system in SFML uses rectangular collision boxes so for the bots flashlight area which is a triangle it can be inaccurate. Instead a different approach was used. In this approach three triangles are drawn from the player to each of the flashlight vertices and when the sume of the area of all three triangles is the same as the flashlight's area the player is contained within the bot's flashlight  

## Bugs 🐜
* The following code uses numerous global variables which isn't the best practice. Local variables being passed as arguments would have been better practice
* Resources for the A* pathfinding algorithm can be found in my previous [Astar_Pathfinding_Visualization](https://github.com/Raziz1/Astar_Pathfinding_Visualization) project.
* The player sometimes runs over crates and is pushed all the way to the other side of the crate
* Sometimes the bot doesn't spot the player
* Sometimes the player can spawn in a position where it is completely surrounded by crates
* This A* pathfinding algorithm calculates the euclidean distance but doesn't use diagonal movements

## Future Updates 🔼  
* Health for the main player
* The bot will shoot at the main player when at a specified distance 
* The main player can kill the bot
* Multiple bots can respawn

 
