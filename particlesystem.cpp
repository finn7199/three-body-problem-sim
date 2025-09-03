#include "ParticleSystem.h"
#include <glad/glad.h>
#include <random>

// Helper class for random numbers
class Random
{
	public:
		static void Init() { s_RandomEngine.seed(std::random_device()()); }
		static float Float() { return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max(); }
	private:
		static std::mt19937 s_RandomEngine;
		static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
};
std::mt19937 Random::s_RandomEngine;
std::uniform_int_distribution<std::mt19937::result_type> Random::s_Distribution;


ParticleSystem::ParticleSystem(uint32_t maxParticles) : m_ParticleShader("particle.vs", "particle.frag"), m_PoolIndex(maxParticles - 1)
{
	Random::Init();
	m_ParticlePool.resize(maxParticles);

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	// We will be streaming data, so use GL_STREAM_DRAW
	glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(float) * 8, nullptr, GL_STREAM_DRAW);

	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
	// Color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
	// Life
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 7));

	glBindVertexArray(0);
}


void ParticleSystem::Update(float dt)
{
	for (auto& particle : m_ParticlePool)
	{
		if (particle.LifeRemaining <= 0.0f)
			continue;

		particle.LifeRemaining -= dt;
		particle.Position += particle.Velocity * dt;
	}
}

void ParticleSystem::Render(const glm::mat4& view, const glm::mat4& projection) const
{
	m_ParticleShader.use();
	m_ParticleShader.setMat4("view", view);
	m_ParticleShader.setMat4("projection", projection);
	m_ParticleShader.setFloat("u_SizeMultiplier", 3.0f);

	std::vector<float> vertexData;
	vertexData.reserve(m_ParticlePool.size() * 8); // 8 floats per particle (pos, color, life)

	int particleCount = 0;
	for (auto& particle : m_ParticlePool)
	{
		if (particle.LifeRemaining <= 0.0f)
			continue;

		particleCount++;
		float life = particle.LifeRemaining / particle.LifeTime;
		glm::vec4 color = glm::mix(particle.ColorEnd, particle.ColorBegin, life);

		vertexData.push_back(particle.Position.x);
		vertexData.push_back(particle.Position.y);
		vertexData.push_back(particle.Position.z);
		vertexData.push_back(color.r);
		vertexData.push_back(color.g);
		vertexData.push_back(color.b);
		vertexData.push_back(color.a);
		vertexData.push_back(life);
	}

	if (particleCount > 0)
	{
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertexData.size() * sizeof(float), vertexData.data());

		glEnable(GL_PROGRAM_POINT_SIZE);
		glDrawArrays(GL_POINTS, 0, particleCount);
		glDisable(GL_PROGRAM_POINT_SIZE);
		glBindVertexArray(0);
	}
}


void ParticleSystem::Emit(const ParticleProps& props)
{
	Particle& particle = m_ParticlePool[m_PoolIndex];

	particle.Position = props.Position;
	particle.LifeTime = props.LifeTime;
	particle.LifeRemaining = props.LifeTime;
	particle.ColorBegin = props.ColorBegin;
	particle.ColorEnd = props.ColorEnd;

	particle.Velocity = props.Velocity;
	particle.Velocity.x += props.VelocityVariation.x * (Random::Float() - 0.5f);
	particle.Velocity.y += props.VelocityVariation.y * (Random::Float() - 0.5f);
	particle.Velocity.z += props.VelocityVariation.z * (Random::Float() - 0.5f);

	m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();
}