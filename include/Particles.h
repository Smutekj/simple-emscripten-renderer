#pragma once

#include "Vertex.h"
#include "Vector2.h"
#include "Texture.h"

#include "Utils/ObjectPool.h"

#include <functional>

//! \struct Particle
//! \brief contains data used in particle drawing + lifetime management
struct Particle
{
    utils::Vector2f pos;
    utils::Vector2f vel = {0,0};
    utils::Vector2f acc = {0,0};
    utils::Vector2f scale = {1,1};
    float angle = 0.f;
    Color color = {0, 0, 0, 1};
    float life_time = 1.f;
    float time = 0;

    Particle() = default;
    virtual ~Particle() = default;

    Particle(utils::Vector2f init_pos, utils::Vector2f init_vel, utils::Vector2f acc = {0, 0}, utils::Vector2f scale = {1, 1},
             Color color = {0, 0, 1, 1}, float life_time = 69);
};

class Renderer;

//! \class Particles
//! \brief contains a managed array of particles
//! takes care of their lifetimes and kills particles who are too old
//! can specify integration step using the setUpdaterFull(...) method
//! spawning of particles can be controlled using the setEmitter(...) method
//! both of them take a single lambda function
class Particles
{

public:
    explicit Particles() = default;
    explicit Particles(int n_max_particles = 100);
    ~Particles() = default;
    void update(float dt);

    virtual void draw(Renderer &target);

    void setSpawnPos(utils::Vector2f pos);
    utils::Vector2f getSpawnPos() const;

    void setInitColor(Color color);
    void setFinalColor(Color color);

    void setLifetime(float lifetime);

    void setRepeat(bool repeats);
    bool getRepeat() const;

    void setPeriod(int period);
    int getPeriod() const;

    void setUpdater(std::function<void(Particle &, float dt)> new_updater);
    void setUpdaterFull(std::function<void(std::vector<Particle> &, int, float)> new_updater);
    void setEmitter(std::function<Particle(utils::Vector2f)> new_emitter);
    void setOnParticleDeathCallback(std::function<void(Particle &)> new_updater);

    void setShader(const std::string &shader_id);

public:
    Color m_init_color;
    Color m_final_color;
    utils::Vector2f m_spawn_pos;
    std::function<void(std::vector<Particle> &, int, float)> m_updater_full; 

private:
    void integrate(float dt);
    void destroyDeadParticles();
    void createParticle();

protected:
    std::function<void(Particle &, float)> m_updater = [](Particle &, float) {};
    std::function<void(Particle &)> m_on_particle_death = [](Particle &) {};
    std::function<Particle(utils::Vector2f)> m_emitter = [](utils::Vector2f)
    { return Particle{}; };

    VectorMap<Particle> m_particle_pool;

    int m_spawn_period = 1; //!< m_spawn_period frames need to pass for one particle
    int m_spawn_timer = 0;  //!< measures frames since last spawn

    bool m_repeats = true; //!< true if particles should be created continuously
    int n_spawned = 0;      //!< number of live particles

    float m_lifetime = 1.f; //!< maximum time of life of the particle (when particle lives this long, it gets killed)
    std::string m_shader_id = "VertexArrayDefault"; //!< shader id
};

//! \class TexturedParticles
//! \brief probably not working at the moment :(
//! \todo will fix when necessary
class TexturedParticles : public Particles
{

public:
    explicit TexturedParticles(Texture &texture);
    virtual ~TexturedParticles() = default;

    virtual void draw(Renderer &r) override;
    void setTexture(Texture &texture);

private:
    Texture *m_texture = nullptr;
};