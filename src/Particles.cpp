#include "Particles.h"

#include "Renderer.h"

Particle::Particle(utils::Vector2f init_pos, utils::Vector2f init_vel, utils::Vector2f acc, utils::Vector2f scale,
                   Color color, float life_time)
    : pos(init_pos), vel(init_vel), acc(acc), scale(scale), color(color), life_time(life_time)
{
}

//! \brief constructs from maximum number of particles
//! \param n_max_particles maximum number of particles
Particles::Particles(int n_max_particles)
    : m_particle_pool(n_max_particles)
{
}

//! \brief sets spawn position of the particles
//! \param pos new spawn position
void Particles::setSpawnPos(utils::Vector2f pos)
{
    m_spawn_pos = pos;
}

void Particles::setPeriod(float spawn_period)
{
    m_spawn_period = spawn_period;
}
int Particles::getPeriod() const
{
    return m_spawn_period;
}

//! \brief creates particle/s if needed then does one integration of updater
//! \brief afterwards destroys particles that are dead
//! \param dt time step
void Particles::update(float dt)
{

    m_spawn_timer += dt;
    int particles_to_spawn = std::floor((m_spawn_timer + dt) / m_spawn_period) - std::floor(m_spawn_timer / m_spawn_period);
    //! take into account non-repeating systems
    particles_to_spawn *= (m_repeats || n_spawned < m_particle_pool.capacity());
    for (int i = 0; i < particles_to_spawn; ++i)
    {
        createParticle();
    }
    m_spawn_timer -= std::floor(m_spawn_timer / m_spawn_period) * m_spawn_period;

    if (!m_updater_full) //! use deafult method if we haven't provided it ourselves
    {
        integrate(dt);
    }
    else
    {
        m_updater_full(m_particle_pool.getData(), m_particle_pool.size(), dt);
    }
    destroyDeadParticles();
}

//! \brief creates new particle if there is space in particle pool
void Particles::createParticle()
{
    auto new_particle = m_emitter(m_spawn_pos);
    m_particle_pool.insert(new_particle);
    n_spawned++;
}

//! \brief destroys all particles with time larger than lifetime
//! \brief and frees space in particle pool
void Particles::destroyDeadParticles()
{
    auto &particles = m_particle_pool.getData();
    std::vector<int> to_destroy;
    for (size_t p_ind = 0; p_ind < m_particle_pool.size(); ++p_ind)
    {
        auto &particle = particles[p_ind];
        if (particle.time > particle.life_time)
        {
            to_destroy.push_back(m_particle_pool.getEntityInd(p_ind));
        }
    }
    for (auto part_ind : to_destroy)
    {
        m_particle_pool.removeByEntityInd(part_ind);
    }
}

Color static interpolate(Color start, Color end, float lambda)
{
    return start + (end - start) * lambda;
}

//! \brief one step of integration algorithm (euler i guess...)
//! \param dt time step
void Particles::integrate(float dt)
{
    auto &particles = m_particle_pool.getData();
    for (size_t p_ind = 0; p_ind < m_particle_pool.size(); ++p_ind)
    {
        auto &particle = particles[p_ind];
        particle.time += dt;
        // particle.color = interpolate(m_init_color, m_final_color, particle.time / particle.life_time);
        m_updater(particle, dt);
    }
}

//! \brief Draws particles into \p canvas
//! \brief \param canvas target to draw into
void Particles::draw(Renderer &canvas)
{
    auto &particles = m_particle_pool.getData();
    auto n_particles = m_particle_pool.size();

    auto min_it = std::min_element(particles.begin(), particles.begin() + n_particles, [](auto &p1, auto &p2)
                                   { return p1.time < p2.time; });
    //! we draw from the youngest to the oldest
    int youngest_particle_ind = min_it - particles.begin();
    // RectangleSimple rect;
    BlurredRect rect;
    for (size_t i = 0; i < n_particles; ++i)
    {
        int p_ind = (youngest_particle_ind + i) % n_particles;
        auto &particle = particles.at(p_ind);
        // rect.setPosition(particle.pos);
        // rect.setScale(particle.scale);
        // rect.setRotation(utils::radians(particle.angle));
        // rect.m_color = particle.color;
        rect.angle = particle.angle;
        rect.corner_radius = 0.2;
        rect.scale = particle.scale;
        rect.pos = particle.pos;
        rect.fill_color = particle.color;
        canvas.drawBatched(rect, m_shader_id);
        // canvas.drawRectangle(rect);
    }
}

void Particles::setUpdater(std::function<void(Particle &, float)> new_updater)
{
    m_updater = new_updater;
}
void Particles::setUpdaterFull(std::function<void(std::vector<Particle> &, int, float)> new_updater)
{
    m_updater_full = new_updater;
}

void Particles::setEmitter(std::function<Particle(utils::Vector2f)> new_emitter)
{
    m_emitter = new_emitter;
}

void Particles::setInitColor(Color color)
{
    m_init_color = color;
}

void Particles::setFinalColor(Color color)
{
    m_final_color = color;
}

utils::Vector2f Particles::getSpawnPos() const
{
    return m_spawn_pos;
}

bool Particles::getRepeat() const
{
    return m_repeats;
}

void Particles::setShader(const std::string &shader_id)
{
    m_shader_id = shader_id;
}

TexturedParticles::TexturedParticles(int n_parts)
    : Particles(n_parts)
{
}

TexturedParticles::TexturedParticles(Texture &texture, int n_parts)
    : Particles(n_parts), m_texture(&texture)
{
}

void TexturedParticles::draw(Renderer &renderer)
{

    auto &particles = m_particle_pool.getData();
    auto n_particles = m_particle_pool.size();

    // utils::Vector2f texture_size = asFloat(m_texture->getSize());
    Sprite sprite(*m_texture);

    for (size_t p_ind = 0; p_ind < n_particles; ++p_ind)
    {
        auto &particle = particles[p_ind];

        sprite.setPosition(particle.pos.x, particle.pos.y);
        sprite.setRotation(particle.angle);
        sprite.setScale(particle.scale.x, particle.scale.y);

        renderer.drawSprite(sprite);
    }
}

void Particles::setRepeat(bool repeats)
{
    m_repeats = repeats;
}

void TexturedParticles::setTexture(Texture &texture)
{
    m_texture = &texture;
}

SpriteParticles::SpriteParticles(Sprite sprite, int n_parts)
    : m_prototype(sprite),
      m_particle_pool(n_parts)
{
}

//! \brief creates particle/s if needed then does one integration of updater
//! \brief afterwards destroys particles that are dead
//! \param dt time step
void SpriteParticles::update(float dt)
{
    spawn(dt);
    integrate(dt);
    destroyDead();
}

void SpriteParticles::spawn(float dt)
{
    m_spawn_timer += dt;
    int particles_to_spawn = std::floor((m_spawn_timer + dt) / spawn_period) - std::floor(m_spawn_timer / spawn_period);
    //! take into account non-repeating systems
    particles_to_spawn *= (repeats || m_live_count < m_particle_pool.capacity());
    for (int i = 0; i < particles_to_spawn; ++i)
    {
        createParticle();
    }
    m_spawn_timer -= std::floor(m_spawn_timer / spawn_period) * spawn_period;
}

//! \brief creates new particle if there is space in particle pool
void SpriteParticles::createParticle()
{
    auto new_particle = emitter(spawn_pos);
    m_particle_pool.insert(new_particle);
    m_live_count++;
}

//! \brief destroys all particles with time larger than lifetime
//! \brief and frees space in particle pool
void SpriteParticles::destroyDead()
{
    auto &particles = m_particle_pool.getData();
    std::vector<std::pair<int, ParticleData>> to_destroy;
    for (size_t p_ind = 0; p_ind < m_particle_pool.size(); ++p_ind)
    {
        auto &particle = particles[p_ind];
        if (particle.time > particle.life_time)
        {
            to_destroy.emplace_back(m_particle_pool.getEntityInd(p_ind), particle);
        }
    }
    for (auto& [part_ind, particle] : to_destroy)
    {
        on_particle_death(particle);
        m_particle_pool.removeByEntityInd(part_ind);
    }
}

//! \brief one step of integration algorithm (euler i guess...)
//! \param dt time step
void SpriteParticles::integrate(float dt)
{
    auto &particles = m_particle_pool.getData();
    for (size_t p_ind = 0; p_ind < m_particle_pool.size(); ++p_ind)
    {
        auto &particle = particles[p_ind];
        particle.time += dt;
        updater(particle, dt);
    }
}

//! \brief Draws particles into \p canvas
//! \brief \param canvas target to draw into
void SpriteParticles::draw(Renderer &canvas)
{
    auto &particles = m_particle_pool.getData();
    auto n_particles = m_particle_pool.size();

    auto min_it = std::min_element(particles.begin(), particles.begin() + n_particles, [](auto &p1, auto &p2)
                                   { return p1.time < p2.time; });
    //! we draw from the youngest to the oldest
    int youngest_particle_ind = min_it - particles.begin();
    // RectangleSimple rect;
    for (size_t i = 0; i < n_particles; ++i)
    {
        int p_ind = (youngest_particle_ind + i) % n_particles;
        auto &particle = particles.at(p_ind);
        canvas.drawSprite(particle.image, m_shader_id);
    }
}