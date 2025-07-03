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

	//pmove.numtouch = 0;
	// if (pmove.spectator)
	// {
	// 	SpectatorMove ();
	// 	return;
	// }
    // NudgePosition ();

	// set onground, watertype, and waterlevel
	// PM_CatagorizePosition ();

	// if (waterlevel == 2)
	// 	CheckWaterJump ();

	// if (pmove.velocity[2] < 0)
	// 	pmove.waterjumptime = 0;

	// if (pmove.cmd.buttons & BUTTON_JUMP)
	// 	JumpButton ();
	// else
	// 	pmove.oldbuttons &= ~BUTTON_JUMP;

	Friction ();
	/* if (waterlevel >= 2)
		PM_WaterMove ();
	else */
		AirMove (wishVelocity);

	// set onground, watertype, and waterlevel for final spot
	//CatagorizePosition ();
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
	// if (pmove.waterjumptime) {
	// 	return;
    // }

    float speed = glm::length(velocity);
	if (speed < 1) {
		velocity.x = 0;
		velocity.y = 0;
		return;
	}
    float friction = 4/* movevars.friction */;

    // If the leading edge is over a dropoff, increase friction
	// if (onground != -1) {
	// 	start[0] = stop[0] = pmove.origin[0] + vel[0]/speed*16;
	// 	start[1] = stop[1] = pmove.origin[1] + vel[1]/speed*16;
	// 	start[2] = pmove.origin[2] + player_mins[2];
	// 	stop[2] = start[2] - 34;

	// 	trace = PM_PlayerMove (start, stop);

	// 	if (trace.fraction == 1) {
	// 		friction *= 2;
	// 	}
	// }

    float drop = 0;

	/* if (waterlevel >= 2) // apply water friction
		drop += speed*movevars.waterfriction*waterlevel*frametime;
	else  */if (onground != -1) {
        // Apply ground friction
		float control = (speed < 100/* movevars.stopspeed */) ? 100/* movevars.stopspeed */ : speed;
		drop += control * friction * elapsed;
	}

    // Scale the velocity
	float newspeed = speed - drop;
	if (newspeed < 0)
		newspeed = 0;
	newspeed /= speed;
    velocity = velocity * newspeed;
}

#define	STOP_EPSILON	0.1
void	PlayerMove::ClipVelocity (const glm::vec3& in, const glm::vec3& normal, glm::vec3& out, float overbounce)
{
	float backoff = dot(in, normal) * overbounce;
	for (int i=0 ; i<3 ; i++) {
		float change = normal[i] * backoff;
		out[i] = in[i] - change;
		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON) {
			out[i] = 0;
		}
	}
}
