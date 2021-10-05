#pragma once

#include <queue>

#include "GameObject.h"
#include "Component.h"

class ChunkManager : public Component
{
public:
	ChunkManager() :
		m_totalTime(0.0f)
	{
	}

	void spawnChunk(int playerId, float maxScale, const glm::vec3& location, const glm::vec3& velocity);

	void clearChunks();

	virtual void update(float deltaTime);

private:
	class ChunkInfo
	{
	public:
		ChunkInfo(GameObject* pChunk, float expirationTime) :
			m_pChunk(pChunk),
			m_expirationTime(expirationTime)
		{
		}

		GameObject* getGameObject() const { return m_pChunk; }
		float getExpirationTime() const { return m_expirationTime; }

	private:
		GameObject* m_pChunk;
		float m_expirationTime;
	};

	std::queue<ChunkInfo> m_chunks;
	float m_totalTime;

	void destroyOldestChunk();
};

