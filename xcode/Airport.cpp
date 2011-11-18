//
//  Airport.cpp
//  Transport
//
//  Created by 4pcbr on 11/1/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "cinder/app/AppBasic.h"
#include "Airport.h"
#include "cinder/Vector.h"
#include "Mercator.h"

#define AIRPORT_OPACITY_STEP 0.05f
#define AIRPORT_BOUND_OPACITY 0.3f
#define AIRPORT_RADIUS 2.0f
#define RADIUS_FACTOR 0.05f

using namespace ci;
using namespace std;

Airport::Airport( const int id, Vec2f point ) {
    this->id = id;
    this->point = point;
    this->is_drawn = false;
    this->opacity = 0.0f;
    this->from_count = 0;
    this->to_count = 0;
    this->resetRadius();
}

void Airport::step() {
    if ( this->is_drawn ) {
        return;
    }
    this->opacity += AIRPORT_OPACITY_STEP;
    if ( this->opacity >= AIRPORT_BOUND_OPACITY ) {
        this->is_drawn = true;
    }
}

void Airport::draw() {
    gl::color( ColorA( 1.0f, 0.5686f, 0.02745f, this->opacity ) );
    gl::drawSolidCircle( Mercator::mapLatLon( this->point ) , this->radius );
}

int Airport::getId() {
    return this->id;
}

ci::Vec2f Airport::getPoint() {
    return this->point;
}

bool Airport::isComplete() { return this->is_drawn; }

void Airport::pushFrom( int cnt ) {
    this->from_count += cnt;
    this->resetRadius();
}

void Airport::pushTo( int cnt ) {
    this->to_count += cnt;
    this->resetRadius();
}

void Airport::resetRadius() {
    this->radius = AIRPORT_RADIUS;// + ( this->from_count + this->to_count ) * RADIUS_FACTOR;
}
