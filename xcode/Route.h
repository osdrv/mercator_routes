//
//  Route.h
//  Transport
//
//  Created by 4pcbr on 10/31/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef Transport_Route_h
#define Transport_Route_h

#include "cinder/Vector.h"
#include "Airport.h"
#include "cinder/Shape2d.h"
#include <vector>

using namespace ci;

class Route {
    Vec2f from, to, current;
    Shape2d shape;
    bool complete, via_pacific, parity;
    float lon_range, lat1, lat2, lon1, lon2, step_lon;
    int direction, steps, max_steps, step_cycle, from_id, to_id, step_parity;
    char sign;
public:
    Route();
    Route( Airport from, Airport to );
    void draw();
    void step();
    Vec2f getFrom();
    Vec2f getTo();
    const int getFromId();
    const int getToId();
    bool isComplete();
    float getLonRange();
    static bool sort( Route r1, Route r2 );
};

#endif
