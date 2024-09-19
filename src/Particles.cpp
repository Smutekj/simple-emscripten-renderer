#include "Particles.h"

#include "Utils/RandomTools.h"
#include "Renderer.h"

Particle::Particle(utils::Vector2f init_pos, utils::Vector2f init_vel, utils::Vector2f acc, utils::Vector2f scale,
                   Color color, float life_time)
    : pos(init_pos), vel(init_vel), acc(acc), scale(scale), color(color), life_time(life_time)
{
}

Particles::Particles(int n_max_particles)
    : m_particle_pool(n_max_particles)
{
}

void Particles::setSpawnPos(utils::Vector2f pos)
{
    m_spawn_pos = pos;
}

void Particles::setSize(float size)
{
    m_particle_size = size;
}
void Particles::setPeriod(int spawn_period)
{
    m_spawn_period = spawn_period;
}
int Particles::getPeriod()const
{
    return m_spawn_period;;
}

void Particles::update(float dt)
{

    m_spawn_timer++;
    if (m_spawn_timer >= m_spawn_period)
    {
        m_spawn_timer = 0;

        if (!m_repeats && n_spawned < m_particle_pool.capacity())
        {
            createParticle();
        }
        else if (m_repeats)
        {
            createParticle();
        }
        else
        {
        }
    }

    integrate(dt);
    destroyDeadParticles();
}

void Particles::createParticle()
{
    auto new_particle = m_emitter(m_spawn_pos);
    m_particle_pool.insert(new_particle);
    n_spawned++;
}

void Particles::destroyDeadParticles()
{
    auto &particles = m_particle_pool.getData();
    std::vector<int> to_destroy;
    for (int p_ind = 0; p_ind < m_particle_pool.size(); ++p_ind)
    {
        auto &particle = particles[p_ind];
        if (particle.time > particle.life_time)
        {
            to_destroy.push_back(p_ind);
        }
    }
    for (auto part_ind : to_destroy)
    {
        m_particle_pool.removeByDataInd(part_ind);
    }
}



Color interpolate(Color start, Color end, float lambda)
{
    return start + (end - start) * lambda ;
}

void Particles::integrate(float dt)
{
    auto &particles = m_particle_pool.getData();
    for (int p_ind = 0; p_ind < m_particle_pool.size(); ++p_ind)
    {
        auto &particle = particles[p_ind];
        particle.time += dt;
        particle.color = interpolate(m_color, m_final_color, particle.time/particle.life_time);
        m_updater(particle);
    
    }
}

void Particles::draw(Renderer &renderer)
{
    auto &particles = m_particle_pool.getData();
    auto n_particles = m_particle_pool.size();

    auto min_it = std::min_element(particles.begin(), particles.begin() + n_particles, [](auto &p1, auto &p2)
                                   { return p1.time < p2.time; });
    //! we draw from the youngest to the oldest
    int youngest_particle_ind = min_it - particles.begin();
    Rectangle2 rect;
    for (int i = 0; i < n_particles; ++i)
    {
        int p_ind = (youngest_particle_ind + i) % n_particles;
        auto &particle = particles.at(p_ind);
        rect.setPosition(particle.pos.x, particle.pos.y);
        rect.setRotation(particle.angle);
        rect.setScale(particle.scale.x, particle.scale.y);

        renderer.drawRectangle(rect, particle.color, m_shader_id, GL_DYNAMIC_DRAW);
    }
}

void Particles::setUpdater(std::function<void(Particle &)> new_updater)
{
    m_updater = new_updater;
}
void Particles::setEmitter(std::function<Particle(utils::Vector2f)> new_emitter)
{
    m_emitter = new_emitter;
}

void Particles::setVel(float vel)
{
    m_vel = vel;
}

void Particles::setInitColor(Color color)
{
    m_color = color;
}
void Particles::setFinalColor(Color color)
{
    m_final_color = color;
}
void Particles::setLifetime(float life_time)
{
    m_lifetime = life_time;
}
void Particles::setFrequency(float frequency)
{
    m_spawn_period = 1./frequency;
}

TexturedParticles::TexturedParticles(Texture &texture)
    : m_texture(&texture), Particles(20)
{
    m_particle_size = 3;
}

void TexturedParticles::draw(Renderer &renderer)
{

    auto &particles = m_particle_pool.getData();
    auto n_particles = m_particle_pool.size();

    // utils::Vector2f texture_size = asFloat(m_texture->getSize());
    Sprite2 sprite(*m_texture);

    for (int p_ind = 0; p_ind < n_particles; ++p_ind)
    {
        auto &particle = particles[p_ind];

        sprite.setPosition(particle.pos.x, particle.pos.y);
        sprite.setRotation(particle.angle);
        sprite.setScale(particle.scale.x, particle.scale.y);

        renderer.drawSprite(sprite, "Instanced", GL_DYNAMIC_DRAW);
    }
}

void Particles::setAngleSpread(float min_spread, float max_spread)
{
    m_spread_max = max_spread;
    m_spread_min = min_spread;
}
void Particles::setRepeat(bool repeats)
{
    m_repeats = repeats;
}

void TexturedParticles::setTexture(Texture &texture)
{
    m_texture = &texture;
}
