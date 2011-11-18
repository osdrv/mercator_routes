//
//  Airport.h
//  Transport
//
//  Created by 4pcbr on 11/1/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef Transport_Airport_h
#define Transport_Airport_h
#pragma once
#include "cinder/Vector.h"

using namespace ci;
using namespace std;

class Airport {
    int id;
    Vec2f point;
    float opacity, radius;
    bool is_drawn;
    int from_count, to_count;
public:
    Airport( const int id, Vec2f point );
    int getId();
    Vec2f getPoint();
    void draw();
    void step();
    bool isComplete();
    void pushFrom( const int cnt );
    void pushTo( const int cnt );
    void resetRadius();
};


#endif
