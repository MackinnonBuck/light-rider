#include "Components/ChunkManager.h"

#include <btBulletCollisionCommon.h>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GameConstants.h"
#include "ConversionUtils.h"
#include "Components/ChunkRenderer.h"
#include "Components/RigidBodyComponent.h"

namespace GC = GameConstants;

void ChunkManager::spawnChunk(int playerId, float maxScale, const glm::vec3& location, const glm::vec3& velocity)
{
	float scale = glm::linearRand(GC::minChunkscale, maxScale >= GC::maxChunkScale
		? GC::maxChunkScale
		: maxScale);

	GameObject* pChunkObject = GameObject::create("chunk", getGameObject());
	MeshRenderer* pMeshRenderer = pChunkObject->addComponent<ChunkRenderer>(playerId);
	pMeshRenderer->setLocalTransform(glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale)));

	btCollisionShape* pChunkShape = new btBoxShape(btVector3(0.8f, 0.1f, 0.8f));
	pChunkShape->setLocalScaling(btVector3(scale, scale, scale));

	btTransform chunkTransform;
	chunkTransform.setIdentity();
	chunkTransform.setOrigin(toBullet(location));

	float mass = 1.0f;
	btVector3 chunkLocalInertia;
	pChunkShape->calculateLocalInertia(mass, chunkLocalInertia);

	btDefaultMotionState* pChunkMotionState = new btDefaultMotionState(chunkTransform);
	btRigidBody::btRigidBodyConstructionInfo groundRbInfo(mass, pChunkMotionState, pChunkShape, chunkLocalInertia);

	RigidBodyComponent* pChunkRigidBodyComponent = pChunkObject->addComponent<RigidBodyComponent>(groundRbInfo);
	btRigidBody* pChunkRigidBody = pChunkRigidBodyComponent->getRigidBody();
	pChunkRigidBody->setFriction(1.0f);
	pChunkRigidBody->setRestitution(0.5f);
	pChunkRigidBody->setLinearVelocity(toBullet(velocity + glm::ballRand(1.0f) * glm::linearRand(15.0f, 35.0f)));

	m_chunks.push(ChunkInfo(pChunkObject, m_totalTime + GC::chunkLifetime));

	if (m_chunks.size() > GC::maxChunkCount)
	{
		destroyOldestChunk();
	}
}

void ChunkManager::clearChunks()
{
	while (!m_chunks.empty())
	{
		destroyOldestChunk();
	}
}

void ChunkManager::update(float deltaTime)
{
	m_totalTime += deltaTime;
	
	while (!m_chunks.empty() && m_totalTime >= m_chunks.front().getExpirationTime())
	{
		destroyOldestChunk();
	}
}

void ChunkManager::destroyOldestChunk()
{
	assert(!m_chunks.empty());

	GameObject::destroy(m_chunks.front().getGameObject());
	m_chunks.pop();
}
