#pragma once
#include <string>
#include <vector>

// Used to allocate / free memory within the engine,
// and to track memory leaks.
namespace Pine::Memory
{
	struct MemoryAllocation {
		void* ptr = nullptr;
		size_t size = 0;

		// These two are optional, expect -1 or nullptr.
		int id = -1;
		const char* text = "";
	};

	// If enabled, every allocation will be stored and tracked. Will have a small hit on performance.
	// So it should only be enabled when you are trying to track memory leaks.
	void EnableMemoryTracking( );
	void DisableMemoryTracking( );

	// Get a list of every allocation
	std::vector<MemoryAllocation>& GetMemoryAllocations( );

	// Either use this or, the new operator.
	void* Allocate( size_t size, const char* name = nullptr );	
	void Free( void* ptr );

}

// Operator overloading
void* operator new( size_t size );
void operator delete( void* );
