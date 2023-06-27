#include "game.hpp"

#include "IL/il.h"
#include "IL/ilu.h"
#include "RmlUi/Core/Log.h"
#include "util/util.hpp"
#include <iostream>
#include <map>
#include <string>

const int directions[ 8 ][ 2 ] = {
	{  1,  0 },
	{  1, -1 },
	{  0, -1 },
	{ -1, -1 },
	{ -1,  0 },
	{ -1,  1 },
	{  0,  1 },
	{  1,  1 }
};

vec2f Game::screen2world( vec2f pos ) const {
	auto height = static_cast<float>( this->zoomsize * 2.0 );
	auto width = static_cast<float>( this->canvaswidth ) / static_cast<float>( this->canvasheight ) * height;
	return vec2f{
		static_cast<float>( ( pos.x / static_cast<float>( this->canvaswidth ) - 0.5 ) * width + this->camx ),
		static_cast<float>( ( pos.y / static_cast<float>( this->canvasheight ) - 0.5 ) * -height + this->camy )
	};
}

void Game::loadShip( const std::string& filename ) {
	this->lastFilename = filename;

	int nodecount = 0, springcount = 0;

	std::unordered_map<vec3f, Material*> colourdict;
	for ( auto& material : this->materials )
		colourdict[ material->color ] = material;

	ILuint imghandle;
	ilGenImages( 1, &imghandle );
	ilBindImage( imghandle );

	if ( !ilLoadImage( ( const ILstring )( filename.c_str() ) ) ) {
		auto devilError = ilGetError();
		std::cout << "Error: could not load image \"" << filename << "\":";
		std::string errstr( iluErrorString( devilError ) );
		std::cout << devilError << ": " << errstr << "\n";
	}

	ILubyte* data = ilGetData();

	int width = ilGetInteger( IL_IMAGE_WIDTH );
	int height = ilGetInteger( IL_IMAGE_HEIGHT );

	auto* shp = new phys::ship( wld );

	std::map< int, std::map< int, phys::point* > > points;

	for ( int x = 0; x < width; x++ ) {
		for ( int y = 0; y < height; y++ ) {
			// assume R G B:
			vec3f colour{
				static_cast<float>( data[ ( x + ( height - y ) * width ) * 3 + 0 ] ) / 255.f,
				static_cast<float>( data[ ( x + ( height - y ) * width ) * 3 + 1 ] ) / 255.f,
				static_cast<float>( data[ ( x + ( height - y ) * width ) * 3 + 2 ] ) / 255.f
			};
			if ( colourdict.find( colour ) != colourdict.end() ) {
				Material* mtl = colourdict[ colour ];
				points[ x ][ y ] = new phys::point(
					wld,
					vec2f{ static_cast<float>( static_cast<float>( x ) - static_cast<float>( width ) / 2 ), static_cast<float>( y ) },
					mtl,
					mtl->isHull ? 0 : 1 // no buoyancy if it's a hull section
				);
				shp->points.insert( points[ x ][ y ] );
				nodecount++;
			} else {
				points[ x ][ y ] = nullptr;
			}
		}
	}

	// Points have been generated, so fill in all the beams between them.
	// If a beam joins two hull nodes, it is a hull beam.
	// If a non-hull node has empty space on one of its four sides, it is automatically leaking.

	for ( int x = 0; x < width; x++ ) {
		for ( int y = 0; y < height; y++ ) {
			phys::point* a = points[ x ][ y ];
			if ( !a )
				continue;
			// The first four directions out of 8: from 0 deg (+x) through to 135 deg (-x +y) - this covers each pair of points in each direction
			for ( int i = 0; i < 4; i++ ) {
				// adjacent point in direction (i)
				phys::point* b = points[ x + directions[ i ][ 0 ] ][ y + directions[ i ][ 1 ] ];
				// adjacent point in the next CW direction ( for constructing triangles )
				phys::point* c = points[ x + directions[ ( i + 1 ) % 8 ][ 0 ] ][ y + directions[ ( i + 1 ) % 8 ][ 1 ] ];
				if ( b ) {
					bool pointIsHull = a->mtl->isHull;
					bool isHull = pointIsHull && b->mtl->isHull;
					Material* mtl = b->mtl->isHull ? a->mtl : b->mtl;// the spring is hull iff both nodes are hull; if so we use the hull Material.
					shp->springs.insert( new phys::spring( wld, a, b, mtl, -1 ) );
					if ( !isHull ) {
						shp->adjacentnodes[ a ].insert( b );
						shp->adjacentnodes[ b ].insert( a );
					}
					if ( !( pointIsHull || ( points[ x + 1 ][ y ] && points[ x ][ y + 1 ] && points[ x - 1 ][ y ] && points[ x ][ y - 1 ] ) ) )// check for gaps next to non-hull areas:
					{
						a->isLeaking = true;
					}
					if ( c )
						shp->triangles.insert( new phys::ship::triangle( shp, a, b, c ) );
					springcount++;
				}
			}
		}
	}
	ilDeleteImage( imghandle );
	Rml::Log::Message( Rml::Log::LT_INFO, "Loaded ship \"%s\": %d points, %d springs.", filename.c_str(), nodecount, springcount );
}

void Game::loadDepth( const std::string& filename ) {
	/*wxImage depthimage(filename, wxBITMAP_TYPE_PNG);
    oceandepthbuffer = new float[2048];
    for (unsigned i = 0; i < 2048; i++)
    {
        float xpos = i / 16.f;
        oceandepthbuffer[i] = depthimage.GetRed(floorf(xpos), 0) * (floorf(xpos) - xpos) + depthimage.GetRed(ceilf(xpos), 0) * (1 - (floorf(xpos) - xpos))
                            ;//+ depthimage.GetGreen(i % 256, 0) * 0.0625f;
        oceandepthbuffer[i] = oceandepthbuffer[i] * 1.f - 180.f;
    }*/
}


void Game::assertSettings() const {
	this->wld->buoyancy = this->buoyancy;
	this->wld->strength = this->strength;
	this->wld->waterpressure = this->waterpressure;
	this->wld->waveheight = this->waveheight;
	this->wld->seadepth = this->seadepth;
	this->wld->showstress = this->showstress;
	this->wld->quickwaterfix = this->quickwaterfix;
	this->wld->oceandepthbuffer = this->oceandepthbuffer;
	this->wld->xraymode = this->xraymode;
}

void Game::update() {
	if ( this->mouse.ldown ) {
		if ( this->tool == Game::ToolType::SMASH )
			this->wld->destroyAt( screen2world( this->mouse.getPosVec() ) );
		else if ( this->tool == Game::ToolType::GRAB )
			this->wld->drawTo( screen2world( this->mouse.getPosVec() ) );
	}
	if ( this->mouse.rdown ) {
		vec2f difference = screen2world( this->mouse.getPosVec() ) - screen2world( this->mouse.getLastPosVec() );
		this->camx -= difference.x;
		this->camy -= difference.y;
	}
	this->mouse.lastx = mouse.x;
	this->mouse.lasty = mouse.y;
	if ( this->running )
		this->wld->update( 0.02 );
}

void Game::render() const {
	float halfheight = this->zoomsize;
	float halfwidth = static_cast<float>( this->canvaswidth ) / static_cast<float>( this->canvasheight ) * halfheight;
	this->wld->render( this->camx - halfwidth, this->camx + halfwidth, this->camy - halfheight, this->camx + halfheight );
}

Game::Game() : running(true), showstress(false), quickwaterfix(false), xraymode(false),
			   buoyancy(4.0), strength(0.01), waveheight(1.0), waterpressure(0.3), seadepth(150), zoomsize(30.f) {
	Json::Value matroot = jsonParseFile( "data/materials.json" );
	for ( const auto& i : matroot )
		materials.push_back( new Material{ i } );
	this->wld = new phys::world{};
	this->loadDepth( "data/depth.png" );
	this->assertSettings();
}

inline auto Game::Mouse::getPosVec() const -> vec2f {
	return vec2f{ static_cast<float>( this->x ), static_cast<float>( this->y ) };
}

inline auto Game::Mouse::getLastPosVec() const -> vec2f {
	return vec2f{ static_cast<float>( this->lastx ), static_cast<float>( this->lasty ) };
}
