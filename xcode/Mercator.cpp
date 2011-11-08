//
//  Mercator.cpp
//  Transport
//
//  Created by 4pcbr on 11/2/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "Mercator.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Vector.h"

using namespace ci;
using namespace std;

Vec2f Mercator::mapLatLon( const Vec2f lat_lon ) {
    
    Vec2f offset = Vec2f( app::getWindowWidth() / 2, app::getWindowHeight() / 2 );
    
    float radius = app::getWindowWidth() / ( 2 * M_PI );
    float x = radius * lat_lon.y * M_PI / 180 + offset.x;
    float y = offset.y - radius * ( log( tan( ( 90 + lat_lon.x ) * M_PI / 360 ) )  /  M_PI ) * 2.8f;
    
    return Vec2f( x, y );
}