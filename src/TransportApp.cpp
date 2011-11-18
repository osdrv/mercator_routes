#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/Shape2d.h"
#include "PointLoader.h"
#include "cinder/CinderResources.h"
#include "cinder/qtime/MovieWriter.h"
#include "cinder/Utilities.h"
#include "Route.h"
#include "math.h"
#include <list>
#include <map>

#define ROUTES_SAME_TIME 50
#define AIRPORTS_SAME_TIME 250
//#define DISPLAY_WIDTH 1920
//#define DISPLAY_HEIGHT 1080
#define DISPLAY_WIDTH 1280
#define DISPLAY_HEIGHT 800
#define MOVIE_FILENAME "avialines.mov"
#define ROUTES_LIMIT 1000
#define BACKGROUND_IMAGE "routes-map-mercator.png"

#define WRITE_MOVIE false
#define DRAW_BACKGROUND true

using namespace ci;
using namespace ci::app;
using namespace std;
using std::list;
using std::map;

class TransportApp : public AppBasic {
    PointLoader pointLoader;
    list<Airport> airports, active_airports;
    list<Route> routes, tmp_routes, active_routes;
    list<Route>::iterator active_route_iterator;
//    list<Airport>::iterator active_airport_iterator;
    map<int, Airport>::iterator active_airport_iterator;
    list< pair<int, int> > routes_map;
    map<int, Airport> airports_map;
    gl::Texture background;
    map<int, list<int> >airport_routes;
    qtime::MovieWriter movie;
    bool gonna_draw_airports;
  public:
    void prepareSettings( Settings *settings );
    void addActiveRoute( int to, int from );
	void setup();
	void update();
	void draw();
    bool pushRoutes();
    bool pushAirports();
};

void TransportApp::prepareSettings( Settings *settings ) {
    
    settings->setWindowSize( DISPLAY_WIDTH, DISPLAY_HEIGHT );
    settings->setFrameRate( 30.0f );
    
    if ( WRITE_MOVIE ) {
        qtime::MovieWriter::Format format;
        format.setCodec( qtime::MovieWriter::CODEC_JPEG );
        format.setQuality( 0.5f );
        movie = qtime::MovieWriter( getDocumentsDirectory() + MOVIE_FILENAME, DISPLAY_WIDTH, DISPLAY_HEIGHT, format );
    }
    
    this->gonna_draw_airports = true;
}

void TransportApp::setup() {
    airports = pointLoader.loadAirports( "airports.xml" );
    
    map<int, Airport>::iterator ai = airports_map.begin();
    
    for ( list<Airport>::iterator it = airports.begin(); it != airports.end(); ++it ) {
        airports_map.insert(ai, pair<int, Airport>( it->getId(), *it ) );
    }
    
    routes_map = pointLoader.loadRoutes( "routes.xml" );

    map<int, Airport>::iterator from, to;
    map<int, list<int> >::iterator f1, f2;
    
    for ( list< pair<int, int> >::iterator ri = routes_map.begin(); ri != routes_map.end(); ++ri ) {
        from = airports_map.find( ri->first );
        to = airports_map.find( ri->second );
        if ( from == airports_map.end() || to == airports_map.end()) {
            continue;
        }
        from->second.pushFrom( 1 );
        to->second.pushTo( 1 );
        routes.push_back( Route( from->second, to->second ) );
        
//        f1 = airport_routes.find( ri->first );
//        if ( f1 == airport_routes.end() ) {
//            list<int> tmp_list;
//            tmp_list.push_back( ri->second );
//            airport_routes.insert( airport_routes.begin(), pair<int, list<int> >( ri->first, tmp_list ) );
//        } else {
//            f1->second.push_back( ri->second );
//        }
//        
//        f2 = airport_routes.find( ri->second );
//        if ( f2 == airport_routes.end() ) {
//            list<int> tmp_list;
//            tmp_list.push_back( ri->first );
//            airport_routes.insert( airport_routes.begin(), pair<int, list<int> >( ri->second, tmp_list ) );
//        } else {
//            f2->second.push_back( ri->first );
//        }
    }
    
    routes.sort( Route::sort );
    routes.resize( ROUTES_LIMIT );
    console() << routes.size() << endl;
    this->active_route_iterator = routes.begin();
    
    this->active_airport_iterator = airports_map.begin();
    
    background = gl::Texture( loadImage( loadResource( BACKGROUND_IMAGE ) ) );
    
//    f1 = airport_routes.begin();
    
//    if ( f1 != airport_routes.end() ) {
//        addActiveRoute( f1->first, -1 );
//    }
}

void TransportApp::update() {
}

void TransportApp::addActiveRoute( int to, int from ) {
    map<int, Airport>::iterator a1, a2;
    map<int, list<int> >::iterator f1 = airport_routes.find( to );
    a1 = airports_map.find( to );
    
    if ( ( a1 != airports_map.end() ) && ( f1 != airport_routes.end() ) ) {
        // тут надо забирать город from из списка городов to
        if ( from != -1 ) {
            f1->second.remove( from );
        }
        for ( list<int>::iterator i = f1->second.begin(); i != f1->second.end(); ++i ) {
            if ( *i == from ) {
                return;
            }
            a2 = airports_map.find( *i );
            if ( a2 != airports_map.end() ) {
                active_routes.push_back( Route( a1->second, a2->second ) );
            }
        }
        
        airport_routes.erase( f1 );
    }
}

bool TransportApp::pushRoutes() {
    if ( this->active_route_iterator == this->routes.end() ) {
        return false;
    }
    
    for ( int i = 0; i < ROUTES_SAME_TIME; ++i ) {
        if ( this->active_route_iterator == this->routes.end() ) {
            return false;
        }
        this->active_routes.push_back( *this->active_route_iterator );
        ++this->active_route_iterator;
    }
    
    return true;
}

bool TransportApp::pushAirports() {
    if ( this->active_airport_iterator == this->airports_map.end() ) {
        return false;
    }
    
    for ( int i = 0; i < AIRPORTS_SAME_TIME; ++i ) {
        if ( this->active_airport_iterator == this->airports_map.end() ) {
            return false;
        }
        this->active_airports.push_back( this->active_airport_iterator->second );
        ++this->active_airport_iterator;
    }
    
    return true;
}

void TransportApp::draw() {
	
    gl::enableAlphaBlending();
    
    gl::clear( Color( 0, 0, 0 ) );
    
    if ( DRAW_BACKGROUND ) {
        gl::draw( background, getWindowBounds() );
    }
    
    bool airports_drawn = true;
//    if ( this->gonna_draw_airports ) {
        for ( list<Airport>::iterator a = active_airports.begin(); a != active_airports.end(); ++a ) {
            a->step();
            a->draw();
            airports_drawn = airports_drawn && a->isComplete();
        }
        
        if ( airports_drawn ) {
            airports_drawn = airports_drawn && !( pushAirports() );
            if ( airports_drawn && this->gonna_draw_airports ) {
//                writeImage( "/tmp/airports.png", copyWindowSurface() );
                this->gonna_draw_airports = false;
            }
        }
//    }
    
    if ( airports_drawn ) {
//        bool routes_drawn = true;
        for ( list<Route>::iterator ari = routes.begin(); ari != routes.end(); ++ari ) {
            ari->step();
            ari->draw();
//            routes_drawn = routes_drawn && ari->isComplete();
        }

//        if ( routes_drawn ) {
//            routes_drawn = routes_drawn && !( pushRoutes() );
//        }
        
//        if ( routes_drawn ) {
            if ( WRITE_MOVIE ) {
//                if ( routes_drawn ) {
                    this->movie.finish();
//                }
            }
//            writeImage( "/tmp/routes.png", copyWindowSurface() );
//            quit();
//        }
    }

    gl::color( Color( 1.0f, 1.0f, 1.0f ) );
    
    if ( WRITE_MOVIE ) {
        movie.addFrame( copyWindowSurface() );
    }
}

CINDER_APP_BASIC( TransportApp, RendererGl )
