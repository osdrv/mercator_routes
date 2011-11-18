//
//  Route.cpp
//  Transport
//
//  Created by 4pcbr on 10/31/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "Route.h"
#include "Mercator.h"
#include "Airport.h"
#include <list>
#include "cinder/app/AppBasic.h"
#include "cinder/Shape2d.h"


using namespace ci;
using namespace std;
using std::list;

Route::Route() {}


Route::Route( Airport from, Airport to ) {
    this->from = from.getPoint();
    this->from_id = from.getId();
    this->to = to.getPoint();
    this->to_id = to.getId();
    this->complete = false;
    this->parity = true;

    this->current = this->from;
    this->via_pacific = false;
    this->lon_range = fabs( this->from.y - this->to.y );
    this->sign = ( ( this->from.y - this->to.y ) > 0 ) ? -1 : 1;
    this->step_lon = 1.0f;

    this->lat1 = this->from.x * M_PI / 180;
    this->lat2 = this->to.x * M_PI / 180;
    
    // маршрут надо строить через тихий океан
    if ( ( ( this->lon_range ) > ( 360 - this->lon_range ) ) && ( this->from.y * this->to.y < 0 ) ) {
        this->via_pacific = true;
        this->sign *= -1;
        this->lon_range = 360 - this->lon_range;
        this->lon1 = ( ( this->from.y > 0 ) ? ( -180 + this->from.y ) : ( 180 + this->from.y ) ) * M_PI / 180;
        this->lon2 = ( ( this->to.y > 0 ) ? ( -180 + this->to.y ) : ( 180 + this->to.y ) ) * M_PI / 180;
        this->current = Vec2f( this->from.x, this->lon1 * 180 / M_PI );
    } else {
        this->lon1 = this->from.y * M_PI / 180;
        this->lon2 = this->to.y * M_PI / 180;
    }
    
    this->max_steps = round( lon_range / step_lon );
    this->steps = 0;
    this->step_cycle = 0;
}

void Route::step() {
    
//    if ( this->complete ) {
//        return;
//    }
    
    if ( ( this->lon_range < 3 ) && !this->via_pacific ) {
        this->shape.lineTo( Mercator::mapLatLon( this->to ) );
        this->complete = true;
        return;
    }

    if ( this->steps >= this->max_steps ) {
        this->is_complete = true;
    }
    
//    if ( this->steps * this->step_lon >= this->lon_range ) {
//        this->complete = true;
//        return;
//    }
    this->shape = Shape2d();
    this->shape.moveTo( Mercator::mapLatLon( this->from ) );
    bool step_parity = this->parity;
    Vec2f current_point = this->current;
    
    for ( int j = 0; j < this->steps; ++j ) {
        
        float lon = ( current_point.y + this->sign * this->step_lon ) * M_PI / 180;
        
        if ( abs( current_point.y - this->to.y ) <= this->step_lon ) {
            lon = this->to.y * M_PI / 180;
        }
        
        float lat = atan( ( tan( this->lat1 ) * sin( this->lon2 - lon ) / sin( this->lon2 - this->lon1 ) ) + ( tan( this->lat2 ) * sin( lon - this->lon1 ) / sin( this->lon2 - this->lon1 ) ) );
        
        
        Vec2f next_point = Vec2f( lat * 180 / M_PI, lon * 180 / M_PI );
        Vec2f tmp_next_point = next_point;
        
        if ( this->via_pacific ) {
            if ( next_point.y * current_point.y < 0 ) {
                this->shape.moveTo( Mercator::mapLatLon( Vec2f( next_point.x, ( next_point.y > 0 ) ? ( next_point.y - 180 ) : ( next_point.y + 180 ) ) ) );
            }
            tmp_next_point = Vec2f( next_point.x, ( next_point.y > 0 ) ? ( next_point.y - 180 ) : ( next_point.y + 180 ) );
        }
        if ( step_parity ) {
            this->shape.lineTo( Mercator::mapLatLon( tmp_next_point ) );
        } else {
            this->shape.moveTo( Mercator::mapLatLon( tmp_next_point ) );
        }
        
        current_point = next_point;
        step_parity = !step_parity;
    }
    
    if ( this->is_complete ) {
        this->parity = !this->parity;
    } else {
        ++this->steps;
    }
}

void Route::draw() {
    gl::color( ColorA( 1.0f, 0.51f, 0.157f, 0.4f ) );
    gl::draw( this->shape );
}

Vec2f Route::getFrom() {
    return this->from;
}

Vec2f Route::getTo() {
    return this->to;
}

float Route::getLonRange() { return this->lon_range; }
const int Route::getFromId() { return this->from_id; }
const int Route::getToId() { return this->to_id; }

bool Route::isComplete() {
    return this->complete;
}

bool Route::sort( Route r1, Route r2 ) { return r1.getLonRange() > r2.getLonRange(); }
