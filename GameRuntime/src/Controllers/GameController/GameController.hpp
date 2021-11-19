#pragma once
#include <Pine/Components/NativeScript/NativeScript.hpp>

#include "Pine/Assets/Blueprint/Blueprint.hpp"

#include <glm/vec2.hpp>

struct Chunk
{
	glm::ivec2 m_Position;
	Pine::Entity* m_Entity;
};

class GameController : public Pine::NativeScript
{
private:

	std::vector<Chunk> m_Chunks;

	void CreateChunk( int x, int y );

	// Some entities that will define locations in the world.
	Pine::Entity* m_EnemySpawnLocation;
	Pine::Entity* m_ChunkSpawnLocation;

public:

	void OnSetup( ) override;
	void OnRender( ) override;
	void OnUpdate( float deltaTime ) override;

};
