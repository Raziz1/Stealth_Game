///========== Header Guards ==========///
#ifndef OBJECTS_H_INCLUDED
#define OBJECTS_H_INCLUDED
///========== Libraries ==========///
//#include "physics.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <cmath>
#include <chrono>
#include <thread>

///========== Namespaces ==========///
using namespace std;
using namespace sf; //For graphics

///========== Global Variables ==========///
const float PI = 3.14159265359;
const float deg_per_rad = 180.0/PI;
const float pixels_per_meter = 32.0;
const int rows = 26;
const int cols = 18;

//width & height of grid members
int width=50;
int height=50;

///========== Ground Generation Structure ==========///
struct Grid: public RectangleShape {

    //x & y position
    int x;
    int y;

    // f, g, and h values for A*
    float f = 0; //f(n) = g(n) + h(n)
    float g = 0; //g(n) is the cost of the path from the start node to n
    float h = 0; //h(n) is a heuristic function that estimates the cost of the cheapest path from n to the goal

    //Neighbors
    vector <Grid*> neighbors;

    //Previous nodes
    Grid *previous;
    bool wall;
    bool endNode;
    bool startNode;
    bool openSet;
    bool closedSet;
    bool previousCheck;

    //Create a collision box inside each grid
    RectangleShape rectCollision;

    //Constructor
    Grid() {
        //Randomly assigning walls
        wall=false;
        endNode=false;
        startNode=false;
        openSet=false;
        closedSet=false;
        previousCheck=false;
        previous=nullptr;

        //Set the width and height of each grid
        this->setSize(Vector2f(width,height));
        this->setOrigin(Vector2f(this->getSize().x/2,this->getSize().y/2));

        //Set the size and origin of the collision box
        this->rectCollision.setSize(Vector2f(5,5));
        this->rectCollision.setOrigin(Vector2f(this->rectCollision.getSize().x/2,this->rectCollision.getSize().y/2));
    }

    void randomizeGrid() {
        int randomAssign = rand()%100+1;
        if (randomAssign<25 && this->startNode==false && this->endNode==false) {
            wall=true;
        } else {
            wall=false;
        }
    }

    //Display the grid and color it depending on its role
    void showGrid(Texture &crateTex, Texture &groundTex) {
        //Set the position of each grid
        this->setPosition(this->x*(width+5), this->y*(height+5));

        //Set the collision box to the center of each grid
        this->rectCollision.setPosition(this->getPosition());

        //Display each grid color to its corresponding role (wall or endpoint
        //Most of the coloring is for debuggin purposes
        if(this->wall) {
            //this->setFillColor(Color(165,42,42));
            this->setTexture(&crateTex); //Set the crate texture
        } else if (this->endNode) {
            this->setFillColor(Color(255,0,0));
        } else if (this->startNode) {
            this->setFillColor(Color(0,255,0));
        } else if (this->openSet) {
            this->setFillColor(Color(180,255,180));
        } else if (this->previousCheck && this->closedSet) {
            this->setFillColor(Color(0,0,180));
        } else if (this->closedSet) {
            this->setFillColor(Color(255,180,180));
        } else {
            //this->setFillColor(Color(255,255,255));
            this->setTexture(&groundTex); //Set the ground texture

        }
    }

    //Figure out who the neighbor nodes are
    void addNeighbors(vector<vector<Grid>> (&neighborNodes), int indexI, int indexJ) {
        //Bottom
        if (neighborNodes[indexI][indexJ].x < rows - 1) {
            //cout<<"NEIGHBOR BOTTOM"<<endl;
            neighborNodes[indexI][indexJ].neighbors.push_back(&neighborNodes[indexI+1][indexJ]);
        }
        //TOP
        if (neighborNodes[indexI][indexJ].x > 0) {
            //cout<<"NEIGHBOR TOP"<<endl;
            neighborNodes[indexI][indexJ].neighbors.push_back(&neighborNodes[indexI-1][indexJ]);
        }
        //LEFT
        if (neighborNodes[indexI][indexJ].y > 0) {
            //cout<<"NEIGHBOR LEFT"<<endl;
            neighborNodes[indexI][indexJ].neighbors.push_back(&neighborNodes[indexI][indexJ-1]);
        }
        //RIGHT
        if (neighborNodes[indexI][indexJ].y < cols-1) {
            //cout<<"NEIGHBOR RIGHT"<<endl;
            neighborNodes[indexI][indexJ].neighbors.push_back(&neighborNodes[indexI][indexJ+1]);
        }
    }

    float heuristic (Grid a,Grid b) {
        float d = sqrt(pow(b.x - a.x,2) + pow(b.y - a.y,2));
        return d;
    }
};

///========== Player Srtucture ==========///
struct Player : public Sprite {

    float a;
    float player_rot;

    //Player Constructor
    Player(float speed, Texture &tex) {
        a = speed;

        this->setTexture(tex);
        this->setOrigin(tex.getSize().x/2,tex.getSize().y/2);
        this->setPosition(25, 25);
        this->setScale(Vector2f(0.16f, 0.16f)); // absolute scale factor
    }

    void player_movement() {
        //Player movement with "W,A,S,D" keys________________________
        if (Keyboard::isKeyPressed(Keyboard::W)) {
            player_rot=0;
            this->move(0,-a);
        } else if (Keyboard::isKeyPressed(Keyboard::S)) {
            player_rot=180;
            this->move(0,a);
        } else if (Keyboard::isKeyPressed(Keyboard::A)) {
            player_rot=-90;
            this->move(-a,0);
        } else if (Keyboard::isKeyPressed(Keyboard::D)) {
            player_rot=90;
            this->move(a,0);
        }
        this->setRotation(player_rot);
    }
};

///========== Bot Structure ==========///
struct Bot: public Sprite {


public:
    Bot(Texture &tex) {
        this->setTexture(tex);
        this->setOrigin(tex.getSize().x/2,tex.getSize().y/2);
        this->setScale(Vector2f(0.16f, 0.16f)); // absolute scale factor
    }
};

///========== Flashlight Structure with Vertices ==========///
struct Flashlight : public ConvexShape {
public:
    Flashlight() {
        this->setPointCount(3);
        this->setPoint(0,Vector2f(180,0));
        this->setPoint(1,Vector2f(0,360));
        this->setPoint(2,Vector2f(360,360));
        this->setFillColor(Color(255,255,255,50));
        this->setOrigin(180,0);
    }
};

///========== Vertices to calculate collision area ==========///
struct colVert: public ConvexShape {
public:
    colVert() {
        this->setPointCount(3);
        this->setFillColor(Color(255,255,255,50));
        this->setOutlineColor(Color(255,0,0));
        this->setOutlineThickness(1);

    }


};

#endif // OBJECTS_H_INCLUDED
