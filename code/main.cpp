///========== Libraries ==========///
//#include "physics.h"
#include <vector>
#include <string>
#include "objects.h"
#include <SFML/Graphics.hpp>
#include <math.h>
#include <algorithm>
#include <cstdlib>

///========== Namespaces ==========///
using namespace std;
using namespace sf; //for graphics

///========== Variables ==========///
//__________ Calling the structures from objects.h__________//
//Initialize 2D grid array
//Using a 2D vector because memory management is much easier
vector<vector<Grid>> grid(rows, vector<Grid>((cols)));

//Keep track of the start grid, end grid, and current grid
Grid* startNode;
Grid* endNode;
Grid* current;

//Intialize the openset and closedset
vector<Grid*> openSet;
vector<Grid*> closedSet;

//Keep track of the path of the shortest distance from the start node to the end node
vector<Grid*> path;
Grid* temp;

bool foundDestination=false; //Have we found the destination yet
bool findPath=false; //Did we find the shortest path yet

Vector2f botspawPoint;
Vector2f botendPoint;

int botSpeed;


///========== Defining Functions ==========///
//Randomize the bot spawning point
void botSpawn() {
    bool foundSpawn=false;
    bool foundEnd=false;

    while (!foundSpawn & !foundEnd) {
        for(int i=0; i< rows; i++) {
            for (int j = 0; j < cols; j++) {
                int spawnRandom = rand()%100+1;
                if (grid[i][j].wall==false && spawnRandom==50) {
                    botspawPoint = Vector2f(grid[i][j].getPosition().x,grid[i][j].getPosition().y);
                    foundSpawn=true;
                } else if(grid[i][j].wall==false && spawnRandom==75) {
                    botendPoint = Vector2f(grid[i][j].getPosition().x,grid[i][j].getPosition().y);
                    foundEnd=true;
                }

            }
        }
    }
}



//Clear all the gird data
void clearGrid() {
    path.clear();
    openSet.clear();
    closedSet.clear();
    for(int i=0; i< rows; i++) {
        for (int j = 0; j < cols; j++) {
            grid[i][j].f=0;

            grid[i][j].g=0;

            grid[i][j].h=0;

            grid[i][j].endNode=false;
            grid[i][j].startNode=false;
            grid[i][j].openSet=false;
            grid[i][j].closedSet=false;
            grid[i][j].previousCheck=false;
            grid[i][j].previous=nullptr;

        }
    }
}


//Set the end point to the player so the bot can follow the player
void followPlayer(Player player) {
    for(int i=0; i< rows; i++) {
        for (int j = 0; j < cols; j++) {

            if (player.getGlobalBounds().intersects(grid[i][j].getGlobalBounds())) {
                botendPoint = grid[i][j].getPosition();
            }
        }
    }
}


//Initialize the start and end points
void initializeGrid(Bot bot) {
    botSpeed=1; //Reset the bots speed when it is no longer looking for the player
    for(int i=0; i< rows; i++) {
        for (int j = 0; j < cols; j++) {
            //Initialize the start and end nodes
            if (bot.getGlobalBounds().intersects(grid[i][j].getGlobalBounds())) {
                grid[i][j].startNode=true;
                openSet.push_back(&grid[i][j]);
                grid[i][j].openSet=true;
            } else if (grid[i][j].getPosition()==botendPoint) {
                grid[i][j].endNode=true;
                endNode = (&grid[i][j]);
            }
        }
    }

}

//Astar pathfinding algorithm
void aStar(Bot bot) {
    if(foundDestination==false) {
        for (int o=0; o<openSet.size(); o++) {
            openSet.at(o)->openSet=true;
            openSet.at(o)->closedSet=false;

        }
        for (int c=0; c<closedSet.size(); c++) {
            closedSet.at(c)->openSet=false;
            closedSet.at(c)->closedSet=true;
        }

        //Am I still searching?
        if (openSet.size()>0) {

            // Find node with the lowest F score in the open set list and set it as the current
            int winner=0;
            for (int k=0; k<openSet.size(); k++) {
                if (openSet.at(k)->f<openSet.at(winner)->f) {
                    winner=k;
                }
            }
            current = openSet.at(winner);

            //Have we reached the end of the path
            if ((current->x==endNode->x) && (current->y==endNode->y)) {
                cout<<"END OF PATH"<<endl;
                foundDestination=true;
            }

            // Best option moves from openSet to closedSet
            //Remove it from the open set and add it to the closedset because we no longer need to calculate it
            vector<Grid*>::iterator it;
            it = find(openSet.begin(), openSet.end(), current);
            int indexCurrent;
            if (it!=openSet.end()) {
                indexCurrent = distance(openSet.begin(),it);
            } else {
            }
            openSet.erase(openSet.begin()+indexCurrent);
            //openSet.remove(openSet.begin, openSet.end, indexCurrent);
            //openSet.erase(openSet.begin());
            closedSet.push_back(current);

            // Check all the neighbors of the current node
            vector<Grid*> activeNeighbors;
            for (int m=0; m<current->neighbors.size(); m++) {
                activeNeighbors.push_back(current->neighbors.at(m));
            }
            for (int n=0; n<activeNeighbors.size(); n++) {
                Grid* neighbor = activeNeighbors.at(n);

                // Check that the neighbor hasn't been visited and is not a wall
                if(!neighbor->closedSet && !neighbor->wall) {

                    //tempG is the distance traveled to the current node plus the distance from the current node to the neighbor node
                    float tempG = current->g + neighbor->heuristic(*neighbor, *current);
                    // Is this a better path than before?
                    bool newPath = false;

                    //Is the neighbor already in the openlist
                    if (neighbor->openSet) {
                        if (tempG< neighbor->g) {
                            neighbor->g = tempG;//If going through the current node to the neighbor from the start node is shorter than the distance from the start directly to the neighbor node
                            newPath=true;//Then set the neighbor.g to tempG
                        }
                    } else {
                        neighbor->g = tempG; //Set the neighbor.g to tempG
                        newPath = true;
                        openSet.push_back(neighbor); //Consider it for the next path
                    }

                    //Yes, it's a better path
                    if(newPath) {
                        neighbor->h= neighbor->heuristic(*neighbor,*endNode); //Calculate the cost from the neighbor to the end
                        neighbor->f = neighbor->g + neighbor->h;  //Calculate the cost of the distance from the start through previous nodes plus the cost of the distance to the end
                        neighbor->previous = current; //Set the parent node of the neighbors
                    }

                }
            }

        } else {
            //If there is no solution pick new endpoints
            cout<<"No Solution"<<endl;
            clearGrid();
            botSpawn();
            initializeGrid(bot);
        }
    } else if (findPath==false) {
        //Find the path by working backwards
        temp=current;
        path.push_back(temp);
        while (temp->previous!=nullptr) {
            temp->previousCheck=true;
            path.push_back(temp->previous);
            temp = temp->previous;
        }
        findPath=true;
    }

}


///========== Main setup loop ==========///
int main() {
    // Create the main window
    RenderWindow window(VideoMode(1440,990),"Stealth Game");  //creates a window on the screen that is 800 by 600
    window.setFramerateLimit(60); //sets the game loop to run 60 times per second

    ///========== Textures ==========///
    Texture playerTexture;
    Texture botTexture;
    Texture groundTexture;
    Texture crateTexture;
    Texture bulletTexture;

    botTexture.loadFromFile("bot2.png");
    playerTexture.loadFromFile("main_character.png");
    groundTexture.loadFromFile("ground.png",IntRect(422,722,32,32));;
    crateTexture.loadFromFile("crate.png");
    bulletTexture.loadFromFile("bullet.png");

    //Initialize the bot
    Bot bot(botTexture);
    Player player_one(2.f, playerTexture);

    //Initialze flashlight
    Flashlight flashlight;

    //Initialize bullet
    Bullet bullet(bulletTexture);
    vector<Bullet> bullets; //Create a list of bullets so the bot can fire multiple
    int bullet_timer=0; //Create a bullet timer so we can shoot bullets at intervals

    //Colision vertices
    colVert areaVert;
    colVert areaVert1;
    colVert areaVert2;

    //Bot chase speed
    botSpeed=1;


    //__________Generating the ground__________//
    for(int i=0; i< rows; i++) {
        for (int j = 0; j < cols; j++) {
            //Intialize the x coordinates of each grid
            grid[i][j].x=i;
            grid[i][j].y=j;

            //Intialize the grid positions and colors
            grid[i][j].randomizeGrid(); //Randomize the walls
            grid[i][j].showGrid(crateTexture, groundTexture);
        }
    }


    //Set the bot spawn point
    botSpawn();
    bot.setPosition(botspawPoint);

    //Loop through each grid and add its nighbors
    for(int i=0; i< rows; i++) {
        for (int j = 0; j < cols; j++) {
            grid[i][j].addNeighbors(grid, i, j);

            //Initialize the start and end nodes
            if (grid[i][j].getPosition()==botspawPoint) {
                grid[i][j].startNode=true;
                openSet.push_back(&grid[i][j]);
                grid[i][j].openSet=true;
            } else if (grid[i][j].getPosition()==botendPoint) {
                grid[i][j].endNode=true;
                endNode = (&grid[i][j]);
            }
        }
    }

    int botPath;
    ///==================== MAIN GAME LOOP ====================///
    while (window.isOpen()) {// Process events
        sf::Event event;
        while (window.pollEvent(event)) {
            // Close window : exit
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Clear screen
        window.clear();




        //Player Collision detection
        for(int i=0; i< rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (grid[i][j].wall) {
                    if (player_one.getGlobalBounds().intersects(grid[i][j].getGlobalBounds())) { //If the player intersects a grid which is a wall
                        if (player_one.player_rot == 0) player_one.move(0, player_one.a); //Move you in opposite direction of collision
                        if (player_one.player_rot == 180) player_one.move(0, player_one.a*-1);
                        if (player_one.player_rot == -90) player_one.move(player_one.a, 0);
                        if (player_one.player_rot == 90) player_one.move(player_one.a*-1, 0);
                    }
                }
            }
        }

        //Check if a bullet has collided with the player
        for(int b=0; b<bullets.size(); b++) {
            if(bullets[b].getGlobalBounds().intersects(player_one.getGlobalBounds())) {
                bullets.erase(bullets.begin()+b);
                cout<<"YOU DIED"<<endl;
                exit(1);
            }
        }

        //Check if player has collided with the bot
        if (player_one.getGlobalBounds().intersects(bot.getGlobalBounds())) {
            cout<<"YOU KILLED THE BOT!"<<endl;
            exit(1);
        }


        //Flashlight
        flashlight.setPosition(bot.getPosition());
        flashlight.setRotation(bot.getRotation());

        //Calculate the area of the flaslight
        int aX=flashlight.getPoint(0).x;
        int aY=flashlight.getPoint(0).y;
        int bX=flashlight.getPoint(1).x;
        int bY=flashlight.getPoint(1).y;
        int cX=flashlight.getPoint(2).x;
        int cY=flashlight.getPoint(2).y;

        int flashlightArea=abs(((aX*(bY - cY) + bX*(cY - aY) + cX*(aY - bY))/2));

        ///========== Area vertices ==========///
        //Set position of area triangle
        areaVert.setPoint(0,Vector2f(player_one.getPosition().x,player_one.getPosition().y));
        areaVert.setPoint(1,Vector2f(flashlight.getTransform().transformPoint(flashlight.getPoint(1)).x,flashlight.getTransform().transformPoint(flashlight.getPoint(1)).y));
        areaVert.setPoint(2,Vector2f(flashlight.getTransform().transformPoint(flashlight.getPoint(2)).x,flashlight.getTransform().transformPoint(flashlight.getPoint(2)).y));

        //Calculate the area of the vertices
        int dX=areaVert.getPoint(0).x;
        int dY=areaVert.getPoint(0).y;

        int eX=areaVert.getPoint(1).x;
        int eY=areaVert.getPoint(1).y;

        int fX=areaVert.getPoint(2).x;
        int fY=areaVert.getPoint(2).y;
        int triArea=abs(((dX*(eY - fY) + eX*(fY - dY) + fX*(dY - eY))/2));

        ///========== Area vertices1 ==========///
        //Set the position of area triangle 1
        areaVert1.setPoint(0,Vector2f(player_one.getPosition().x,player_one.getPosition().y));
        areaVert1.setPoint(1,Vector2f(flashlight.getTransform().transformPoint(flashlight.getPoint(0)).x,flashlight.getTransform().transformPoint(flashlight.getPoint(0)).y));
        areaVert1.setPoint(2,Vector2f(flashlight.getTransform().transformPoint(flashlight.getPoint(1)).x,flashlight.getTransform().transformPoint(flashlight.getPoint(1)).y));

        //Calculate the area of the vertices
        int gX=areaVert1.getPoint(0).x;
        int gY=areaVert1.getPoint(0).y;

        int hX=areaVert1.getPoint(1).x;
        int hY=areaVert1.getPoint(1).y;

        int iX=areaVert1.getPoint(2).x;
        int iY=areaVert1.getPoint(2).y;

        int triArea1=abs(((gX*(hY - iY) + hX*(iY - gY) + iX*(gY - hY))/2));

        ///========== Area vertices2 ==========///
        //Set the position of area triangle 2
        areaVert2.setPoint(0,Vector2f(player_one.getPosition().x,player_one.getPosition().y));
        areaVert2.setPoint(1,Vector2f(flashlight.getTransform().transformPoint(flashlight.getPoint(0)).x,flashlight.getTransform().transformPoint(flashlight.getPoint(0)).y));
        areaVert2.setPoint(2,Vector2f(flashlight.getTransform().transformPoint(flashlight.getPoint(2)).x,flashlight.getTransform().transformPoint(flashlight.getPoint(2)).y));

        //Calculate the area of the vertices
        int jX=areaVert2.getPoint(0).x;
        int jY=areaVert2.getPoint(0).y;

        int kX=areaVert2.getPoint(1).x;
        int kY=areaVert2.getPoint(1).y;

        int lX=areaVert2.getPoint(2).x;
        int lY=areaVert2.getPoint(2).y;
        int triArea2=abs(((jX*(kY - lY) + kX*(lY - jY) + lX*(jY - kY))/2));

        //If the player collides with the flaslight given a small buffer range
        if((triArea+triArea1+triArea2>flashlightArea-200) && (triArea+triArea1+triArea2<flashlightArea+200)) {
            cout<<"SPOTTED!"<<endl;
            flashlight.setFillColor(Color(255,150,150,100));
            //Chase the player
            findPath=false;
            foundDestination=false;
            clearGrid();
            followPlayer(player_one);
            initializeGrid(bot);
            botSpeed=2; //Make the bot rush to the players last position

            ///========== Bot Shoot Bullet ==========///
            //If the bot sees the player it will shoot a bullet at it
            //Center the bullet on the enemy
            Vector2f botCenter = Vector2f(bot.getPosition());
            Vector2f playerCenter = Vector2f(player_one.getPosition());
            Vector2f aimDir = playerCenter - botCenter;
            Vector2f aimDirNorm = aimDir / ( static_cast<float>(sqrt(pow(aimDir.x, 2) + pow(aimDir.y, 2))));
            bullet.setPosition(botCenter);

            //Rotate the bullet so it faces the player
            float adjacent=player_one.getPosition().x-bot.getPosition().x;
            float opposite=player_one.getPosition().y-bot.getPosition().y;
            float result=atan(adjacent/opposite)*180/PI;
            if (opposite<0) {
                bullet.setRotation(-result-180);
            } else {
                bullet.setRotation(-result);
            }

            //Set the bullet max speed
            bullet.maxSpeed=7.f;
            bullet.currVelocity=aimDirNorm*bullet.maxSpeed;

            //After a certain amount of time shoot a bullet and reset the timer
            if (bullet_timer>=60) {
                bullets.push_back(Bullet(bullet));
                bullet_timer=0;
            }

            //Increase the shooting bullet timer
            bullet_timer+=1;

        } else {
            flashlight.setFillColor(Color(255,255,255,50));
        }

        //For every bullet shot draw it moving
        for(size_t i=0; i<bullets.size(); i++) {
            bullets[i].move(bullets[i].currVelocity);
            //If the bullet is off the screen remove it from the array
            if(bullets[i].getPosition().x<0||bullets[i].getPosition().x>window.getSize().x||bullets[i].getPosition().y<0||bullets[i].getPosition().y>window.getSize().y) {
                bullets.erase(bullets.begin()+i);
            }
        }


        ///========== Bot Main Moving ==========///
        //Main bot stalking phase
        if (findPath) {
            //If we have found the shortest path
            Vector2f desiredTarget; //Keep track of the bots closest target position
            desiredTarget = path.at(botPath)->getPosition();
            //If we haven't reached the end of the path
            if(botPath>0) {
                //Check if we are in the collision box of the grid
                if (bot.getGlobalBounds().intersects(path.at(botPath)->rectCollision.getGlobalBounds())) {
                    botPath-=1;
                } else {
                    //If we aren't at the desired target determine which direction we need to be to be closer
                    //Move bot RIGHT
                    if (desiredTarget.x > bot.getPosition().x) {
                        bot.move(botSpeed,0);
                        bot.setRotation(-90);
                        //Move bot LEFT
                    } else if (desiredTarget.x < bot.getPosition().x) {
                        bot.move(-botSpeed,0);
                        bot.setRotation(90);
                    }
                    //Move bot UP
                    if (desiredTarget.y > bot.getPosition().y) {
                        bot.move(0,botSpeed);
                        bot.setRotation(0);
                        //Move bot DONW
                    } else if (desiredTarget.y < bot.getPosition().y) {
                        bot.move(0,-botSpeed);
                        bot.setRotation(180);

                    }

                }

            } else {
                //We have reached the end of the path which means we need to find a new path
                findPath=false;
                foundDestination=false;
                clearGrid();
                botSpawn();
                initializeGrid(bot);
            }


        } else {
            //Use the Astar pathfinding algorithm
            aStar(bot);

            //Reset the path size counter
            botPath=path.size()-1;
        }

        //Draw each grid square in each column and row
        for(int i=0; i< rows; i++) {
            for (int j = 0; j < cols; j++) {
                grid[i][j].showGrid(crateTexture, groundTexture);
                window.draw(grid[i][j]);
            }
        }

        //Check for player keyboard input
        player_one.player_movement();

        window.draw(bot);
        window.draw(player_one);
        window.draw(flashlight);

        //For all the bullets shot draw them all
        for (size_t i=0; i<bullets.size(); i++) {
            window.draw(bullets[i]);
        }
        //Draw the area vertices for debugging purposes
        /*
        window.draw(areaVert);
        window.draw(areaVert1);
        window.draw(areaVert2);
        */

        // Update the window
        window.display();
    }
    return EXIT_SUCCESS;
}
