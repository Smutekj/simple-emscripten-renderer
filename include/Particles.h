#pragma once

#include "Vertex.h"
#include "Vector2.h"
#include "Texture.h"

#include "Utils/ObjectPool.h"

#include <functional>

namespace sf
{
    class RenderTarget;
}

struct Particle
{
    utils::Vector2f pos;
    utils::Vector2f vel;
    utils::Vector2f acc;
    utils::Vector2f scale;
    float angle = 0.f;
    Color color;
    float life_time;
    float time = 0;

    Particle() = default;
    virtual ~Particle() = default;

    Particle(utils::Vector2f init_pos, utils::Vector2f init_vel, utils::Vector2f acc = {0, 0}, utils::Vector2f scale = {1,1},
             Color color = {0,0,1,1}, float life_time = 69);
};

class Renderer;

class Particles
{

public:
    explicit Particles() = default;
    explicit Particles(int n_max_particles = 100);
    ~Particles() = default;
    void update(float dt);

    virtual void draw(Renderer &target);

    void setSpawnPos(utils::Vector2f pos);
    void setVel(float vel);
    void setInitColor(Color color);
    void setFinalColor(Color color);
    void setLifetime(float lifetime);
    void setFrequency(float lifetime);
    void setAngleSpread(float min_spread, float max_spread);
    void setRepeat(bool repeats);
    void setSize(float size);
    void setUpdater(std::function<void(Particle&)> new_updater);
    void setEmitter(std::function<Particle(utils::Vector2f)> new_emitter);
    void setOnParticleDeathCallback(std::function<void(Particle&)> new_updater);
    
private:
    void integrate(float dt);
    void destroyDeadParticles();
    void createParticle();

protected:
    std::function<void(Particle&)> m_updater = [](Particle&){};
    std::function<void(Particle&)> m_on_particle_death = [](Particle&){};
    std::function<Particle(utils::Vector2f)> m_emitter = [](utils::Vector2f){return Particle{};};

    VectorMap<Particle> m_particle_pool;

    int m_spawn_period = 1;
    int m_spawn_timer = 0;
    utils::Vector2f m_spawn_pos;

    float m_particle_size = 0.5;
    float m_vel = 3;
    Color m_color;
    Color m_final_color;

    bool m_repeats = true;
    int n_spawned = 0;

    float m_spread_min = 0;
    float m_spread_max = 360;
    float m_lifetime = 1.f;
    float m_frequency = 10.f;

    std::string m_shader_id = "VertexArrayDefault";
};

class TexturedParticles : public Particles
{

public:
    explicit TexturedParticles(Texture &texture);
    virtual ~TexturedParticles() = default;

    virtual void draw(Renderer& r) override;
    void setTexture(Texture &texture);

private:
    Texture *m_texture = nullptr;
};