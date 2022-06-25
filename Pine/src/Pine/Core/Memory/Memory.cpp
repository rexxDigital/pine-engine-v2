#include "Memory.hpp"
#include <unordered_map>

#include "../Log/Log.hpp"

namespace
{
	bool g_MemoryTrackingEnabled = false;

	std::vector<Pine::Memory::MemoryAllocation> g_MemoryAllocations;
}

void Pine::Memory::EnableMemoryTracking( )
{
	g_MemoryTrackingEnabled = true;

	Log->Warning( "Memory tracking is now enabled!" );
}

void Pine::Memory::DisableMemoryTracking( )
{
	g_MemoryTrackingEnabled = false;
}

std::vector<Pine::Memory::MemoryAllocation>& Pine::Memory::GetMemoryAllocations( )
{
	return g_MemoryAllocations;
}

void* Pine::Memory::Allocate( size_t size, const char* name )
{
	const auto ptr = malloc( size );

	if ( !g_MemoryTrackingEnabled ) 
	{
		return ptr;
	}

	MemoryAllocation mem;

	mem.ptr = ptr;
	mem.size = size;
	mem.text = name;

	g_MemoryAllocations.push_back( mem );

	return ptr;
}

void Pine::Memory::Free( void* ptr )
{
	free( ptr );

	if ( !g_MemoryTrackingEnabled ) 
	{
		return;
	}

	// Find the allocated memory
	for ( int i = 0; i < g_MemoryAllocations.size( ); i++ ) 
	{
		const auto& mem = g_MemoryAllocations[ i ];

		// Remove it!
		if ( mem.ptr == ptr ) 
		{
			g_MemoryAllocations.erase( g_MemoryAllocations.begin( ) + i );
			return;
		}
	}

	Log->Warning( "Failed to free allocated memory." );
}

/*
void* operator new( size_t size )
{
	return Pine::Memory::Allocate( size );
}

void operator delete( void* ptr )
{
	Pine::Memory::Free( ptr );
}
 */
