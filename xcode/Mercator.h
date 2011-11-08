//
//  Mercator.h
//  Transport
//
//  Created by 4pcbr on 11/2/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef Transport_Mercator_h
#define Transport_Mercator_h
#pragma once
#include "cinder/Vector.h"

class Mercator {
public:
    static ci::Vec2f mapLatLon( const ci::Vec2f lat_lon );
};

#endif
