#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"

struct ParticleProps
{
	glm::vec3 Position;
	glm::vec3 Velocity, VelocityVariation;
	glm::vec4 ColorBegin, ColorEnd;
	float SizeBegin, SizeEnd;
	float LifeTime = 1.0f;
};

class ParticleSystem
{
public:
	ParticleSystem(uint32_t maxParticles = 10000);

	void Update(float dt);
	void Render(const glm::mat4& view, const glm::mat4& projection) const;

	void Emit(const ParticleProps& particleProps);

private:
	struct Particle
	{
		glm::vec3 Position;
		glm::vec3 Velocity;
		glm::vec4 ColorBegin, ColorEnd;
		float LifeTime = 1.0f;
		float LifeRemaining = 0.0f;
	};

	std::vector<Particle> m_ParticlePool;
	uint32_t m_PoolIndex;

	unsigned int m_VAO, m_VBO;
	Shader m_ParticleShader;
};