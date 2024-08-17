// #pragma once

// #include "Particles.h"
// #include "Utils/RandomTools.h"

// #include <unordered_map>

// enum class Effects
// {

// };

// class ParticleEffect
// {

// public:
//     virtual void update(float dt) = 0;
//     virtual void draw(Renderer &target) = 0;

// protected:
// };

// class FireEffect : public ParticleEffect
// {

//     FireEffect()
//     :
//     m_fire_particles()
//     {
        
//         m_fire_particles.setLifetime(2.f);
//         m_fire_particles.setUpdater([this](Particle &p)
//                                     {
//                                 auto t_left = p.life_time - p.time;
//                                 p.acc = {0, 0};
//                                 p.vel += p.time *p.acc;
//                                 p.pos += p.vel * 0.016f;

//                                 p.angle += randf(0, 3.); });
//         m_fire_particles.setEmitter([](cdt::Vector2f spawn_pos)
//                                     {
//                                 Particle p;
//                                 p.pos = spawn_pos + cdt::Vector2f{randf(-50,50), randf(0, 10.f)};
//                                 p.vel = {randf(-20, 20), randf(40, 50)};
//                                 p.vel *= 1.5;

//                                 p.angle = 45.;//randf(0, 90.);
//                                 p.scale = {10.2, 10.2};
//                                 return p; });
//         m_fire_particles.setRepeat(true);
//     }

// public:
//     virtual void update(float dt) override
//     {
//         m_smoke_particles.update(dt);
//         m_fire_particles.update(dt);
//     }

// private:
//     Particles m_smoke_particles;
//     Particles m_fire_particles;
// };