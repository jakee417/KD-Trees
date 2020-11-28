/*
 * File: ball.cpp
 * --------------
 * Final Project - adapted from previous assignment
 * (credit goes towards CS106 instructors)
 * Jake Taylor
 */

# include "ball.h"

/*////////////////////////////////////////////////////////////
 * Code for building graphics using ball.h and gobjects.h
 * ///////////////////////////////////////////////////////////
 */

Ball::Ball(GWindow* w, int id, int x, int y) {
    _window = w;
    _x = x;
    _y = y;
    _id = id;
}

void Ball::draw(string color, bool id) {
    _window->setColor(color);
    _window->fillOval(_x, _y, SIZE, SIZE);
    _window->setColor("Black");
    if(id){
        drawCenteredText(_window, _x, _y, SIZE, SIZE, integerToString(_id));
    }
}
