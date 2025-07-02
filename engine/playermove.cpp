#include "playermove.h"
#include "bsp.h"
#include "client.h"
#include <glm/gtc/matrix_transform.hpp>


PlayerMove::PlayerMove(BSP& bsp) :
    bsp(bsp),
    velocity(0)
{}

void    PlayerMove::Move(Actor& player, const glm::vec3& velocityBase, float elapsed_)
{
    elapsed = elapsed_;
    glm::mat4   mat = glm::rotate(glm::mat4(1), player.Yaw() * glm::pi<float>() / 180.0f, {0, 0, 1.0f});
    glm::vec3   wishVelocity = glm::vec3(mat * glm::vec4(velocityBase, 0));
    AirMove(wishVelocity);
}

void    PlayerMove::AirMove(const glm::vec3& wishVelocity)
{
    float       wishSpeed = glm::length(wishVelocity);
    [[maybe_unused]]glm::vec3   wishDir = glm::normalize(wishVelocity);

    // Clamp to max speed
	if (wishSpeed > /* movevars.maxspeed */320) {
		wishSpeed = /* movevars.maxspeed */320;
	}
	
	if (onground != -1) {
		velocity[2] = 0;
		Accelerate (OnGround, wishDir, wishSpeed, 10/* movevars.accelerate */);
		// Add gravity
		// pmove.velocity[2] -= movevars.entgravity * movevars.gravity * frametime;
		// GroundMove ();
	} else {
        // Not on ground, so little effect on velocity
		Accelerate (InAir, wishDir, wishSpeed, 10/* movevars.accelerate */);
		// Add gravity
		// pmove.velocity[2] -= movevars.entgravity * movevars.gravity * frametime;
		// FlyMove ();
	}
}

void    PlayerMove::Accelerate(AccelerateMode mode, const glm::vec3& wishDir, float wishSpeed, float accel)
{
	// if (pmove.dead)
	// 	return;
	// if (pmove.waterjumptime)
	// 	return;
    if (mode == InAir && wishSpeed > 30) {
        return;
    }
    float currentspeed = glm::dot(velocity, wishDir);
    float addSpeed = wishSpeed - currentspeed;
	if (addSpeed <= 0) {
		return;
    }
	float accelSpeed = accel * wishSpeed * elapsed;
	if (accelSpeed > addSpeed) {
		accelSpeed = addSpeed;
    }
    velocity += wishDir * accelSpeed;
}

void    PlayerMove::Friction()
{
/*
	float	*vel;
	float	speed, newspeed, control;
	float	friction;
	float	drop;
	vec3_t	start, stop;
	pmtrace_t		trace;
	
	if (pmove.waterjumptime)
		return;

	vel = pmove.velocity;
	
	speed = sqrt(vel[0]*vel[0] +vel[1]*vel[1] + vel[2]*vel[2]);
	if (speed < 1)
	{
		vel[0] = 0;
		vel[1] = 0;
		return;
	}

	friction = movevars.friction;

// if the leading edge is over a dropoff, increase friction
	if (onground != -1) {
		start[0] = stop[0] = pmove.origin[0] + vel[0]/speed*16;
		start[1] = stop[1] = pmove.origin[1] + vel[1]/speed*16;
		start[2] = pmove.origin[2] + player_mins[2];
		stop[2] = start[2] - 34;

		trace = PM_PlayerMove (start, stop);

		if (trace.fraction == 1) {
			friction *= 2;
		}
	}

	drop = 0;

	if (waterlevel >= 2) // apply water friction
		drop += speed*movevars.waterfriction*waterlevel*frametime;
	else if (onground != -1) // apply ground friction
	{
		control = speed < movevars.stopspeed ? movevars.stopspeed : speed;
		drop += control*friction*frametime;
	}


// scale the velocity
	newspeed = speed - drop;
	if (newspeed < 0)
		newspeed = 0;
	newspeed /= speed;

	vel[0] = vel[0] * newspeed;
	vel[1] = vel[1] * newspeed;
	vel[2] = vel[2] * newspeed;
*/
}