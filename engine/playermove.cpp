#include "playermove.h"
#include "bsp.h"
#include "client.h"
#include <glm/gtc/matrix_transform.hpp>


PlayerMove::PlayerMove(BSP& bsp, Actor& player) :
    bsp(bsp),
    player(player),
    velocity(0)
{}

void    PlayerMove::Move(const glm::vec3& velocityBase, float elapsed)
{
	origin = player.Position();
    frameTime = elapsed;
    glm::mat4   mat = glm::rotate(glm::mat4(1), player.Yaw() * glm::pi<float>() / 180.0f, {0, 0, 1.0f});
    glm::vec3   wishVelocity = glm::vec3(mat * glm::vec4(velocityBase, 0));

	numTouch = 0;
	// if (pmove.spectator)
	// {
	// 	SpectatorMove ();
	// 	return;
	// }
    NudgePosition ();

	// Set onground, watertype, and waterlevel
	CategorizePosition ();

	// if (waterlevel == 2)
	// 	CheckWaterJump ();

	// if (pmove.velocity[2] < 0)
	// 	pmove.waterjumptime = 0;

	if (jumpKeyDown)
		Jump();
	else
		jumpKey = false;

	Friction ();
	/* if (waterlevel >= 2)
		PM_WaterMove ();
	else */
		AirMove (wishVelocity);

	// Set onground, watertype, and waterlevel for final spot
	CategorizePosition ();
}

void    PlayerMove::Fly(const glm::vec3& velocityBase, float elapsed)
{
    origin = player.Position();
    frameTime = elapsed;
    glm::mat4   mat = glm::rotate(glm::mat4(1), player.Yaw() * glm::pi<float>() / 180.0f, {0, 0, 1.0f});
    glm::vec3   wishVelocity = glm::vec3(mat * glm::vec4(velocityBase, 0));

    float       wishSpeed = glm::length(wishVelocity);
    glm::vec3   wishDir(0);
	if (wishSpeed > SMALL_EPS) {
		wishDir = glm::normalize(wishVelocity);
	}
    velocity = wishVelocity;
    FlyMove();
}

void    PlayerMove::SetVelocity(const glm::vec3& velocityBase)
{
    velocity = velocityBase;
}

void    PlayerMove::SetKeys(bool jumpKeyDown_, bool useKeyDown_)
{
    jumpKeyDown = jumpKeyDown_;
    useKeyDown = useKeyDown_;
}

void    PlayerMove::AirMove(const glm::vec3& wishVelocity)
{
    float       wishSpeed = glm::length(wishVelocity);
    glm::vec3   wishDir(0);
	if (wishSpeed > SMALL_EPS) {
		wishDir = glm::normalize(wishVelocity);
	}

    // Clamp to max speed
	if (wishSpeed > /* movevars.maxspeed */320) {
		wishSpeed = /* movevars.maxspeed */320;
	}
	
	if (onground != -1) {
		velocity[2] = 0;
		Accelerate (OnGround, wishDir, wishSpeed, 10/* movevars.accelerate */);
		// Add gravity
		velocity[2] -= /* movevars.entgravity(==1.0) * */ 800/* movevars.gravity */ * frameTime;
		//TODO Delete if not used?
		GroundMove ();
	} else {
        // Not on ground, so little effect on velocity
		Accelerate (InAir, wishDir, wishSpeed, 10/* movevars.accelerate */);
		// Add gravity
		velocity[2] -= /* movevars.entgravity(==1.0) * */ 800/* movevars.gravity */ * frameTime;
		FlyMove ();
	}
}

void	PlayerMove::FlyMove()
{
	int			numbumps = 4;
	//int			blocked = 0;
	glm::vec3	originalVelocity = velocity;
	glm::vec3	primalVelocity = velocity;
	glm::vec3	dir;
	static constexpr int MAX_CLIP_PLANES = 5;
	int 		numplanes = 0;
	glm::vec3	planes[MAX_CLIP_PLANES];
	
	float 		timeLeft = frameTime;
	for (int bumpcount = 0; bumpcount < numbumps; ++bumpcount) {
		glm::vec3	end = origin + timeLeft * velocity;
		Trace		trace = MovePlayer(origin, end);

		if (trace.startSolid || trace.allSolid) {
			// Entity is trapped in another solid
			velocity = {0,0,0};
			return/*  3 */;
		}
		if (trace.fraction > 0) {
			// Actually covered some distance
			origin = trace.end;
			numplanes = 0;
		}
		if (trace.fraction == 1) {
			 break;		// Moved the entire distance
		}

		// Save entity for contact
		TouchEnt(trace.entity);
/*
		if (trace.plane.normal[2] > 0.7)
		{
			blocked |= 1;		// floor
		}
		if (!trace.plane.normal[2])
		{
			blocked |= 2;		// step
		}
*/
		timeLeft -= timeLeft * trace.fraction;
		
		// Cliped to another plane
		if (numplanes >= MAX_CLIP_PLANES) {
			// This shouldn't really happen
			velocity = {0, 0, 0};
			break;
		}
		planes[numplanes++] = trace.plane.Normal();
		int i = 0;
		for (; i < numplanes; ++i) {
			ClipVelocity (originalVelocity, planes[i], velocity, 1);
			int j = 0;
			for (; j < numplanes; ++j) {
				if (j != i) {
					if (glm::dot(velocity, planes[j]) < 0) {
						break;	// Not ok
					}
				}
			}
			if (j == numplanes) {
				break;
			}
		}
		
		// Modify original_velocity so it parallels all of the clip planes
		if (i != numplanes) {
			// Go along this plane
		} else {
			// Go along the crease
			if (numplanes != 2) {
//				Con_Printf ("clip velocity, numplanes == %i\n",numplanes);
				velocity = {0, 0, 0};
				break;
			}
			dir = glm::cross(planes[0], planes[1]);
			float d = glm::dot(dir, velocity);
			velocity = dir * d;
		}
		
		// If original velocity is against the original velocity, stop dead
		// to avoid tiny occilations in sloping corners
		if (glm::dot(velocity, primalVelocity) <= 0) {
			velocity = {0, 0, 0};
			break;
		}
	}

	// if (pmove.waterjumptime)
	// {
	// 	VectorCopy (primal_velocity, pmove.velocity);
	// }
	return /* blocked */;
}

void	PlayerMove::GroundMove()
{
	velocity[2] = 0;
	if (glm::length(velocity) < SMALL_EPS) {
		return;
	}
	
	// First try just moving to the destination
	glm::vec3	dest;
    dest[0] = origin[0] + velocity[0] * frameTime;
	dest[1] = origin[1] + velocity[1] * frameTime;	
	dest[2] = origin[2];
	
	// First try moving directly to the next spot
	Trace	trace = MovePlayer(origin, dest);
	if (trace.fraction == 1) {
		origin = trace.end;
		return;
	}

	// Try sliding forward both on ground and up 16 pixels
	// Take the move that goes farthest
	glm::vec3	original = origin;
	glm::vec3	originalVel = velocity;
	// Slide move
	FlyMove ();

	glm::vec3	down = origin;
	glm::vec3	downVel = velocity;
	glm::vec3	up;

	origin = original;
	velocity = originalVel;

	// Move up a stair height
	static constexpr int STEPSIZE = 18;
	dest = origin;
	dest[2] += STEPSIZE;
	trace = MovePlayer(origin, dest);
	if (!trace.startSolid && !trace.allSolid) {
		origin = trace.end;
	}
	// Slide move
	FlyMove ();

	// Press down the stepheight
	dest = origin;
	dest[2] -= STEPSIZE;
	trace = MovePlayer(origin, dest);
	bool useDown = false;
	if (trace.plane.Normal()[2] < 0.7) {
		useDown = true;
	} else {
		if (!trace.startSolid && !trace.allSolid) {
			origin = trace.end;
		}
		up = origin;
	}

	// Decide which one went farther
	float	downDist = glm::length(glm::vec2(down) - glm::vec2(original));
	float	upDist = glm::length(glm::vec2(up) - glm::vec2(original));
	if (useDown || downDist > upDist) {
		origin = down;
		velocity = downVel;
	} else {
		// Copy z value from slide move
		velocity[2] = downVel[2];
	}
}

void    PlayerMove::Jump()
{
	// if (pmove.dead)
	// {
	// 	pmove.oldbuttons |= BUTTON_JUMP;	// don't jump again until released
	// 	return;
	// }

	// if (pmove.waterjumptime)
	// {
	// 	pmove.waterjumptime -= frametime;
	// 	if (pmove.waterjumptime < 0)
	// 		pmove.waterjumptime = 0;
	// 	return;
	// }

	// if (waterlevel >= 2)
	// {	// swimming, not jumping
	// 	onground = -1;

	// 	if (watertype == CONTENTS_WATER)
	// 		pmove.velocity[2] = 100;
	// 	else if (watertype == CONTENTS_SLIME)
	// 		pmove.velocity[2] = 80;
	// 	else
	// 		pmove.velocity[2] = 50;
	// 	return;
	// }

	if (onground == -1) {
		return;		// in air, so no effect
    }
	if (jumpKey) {
		return;		// don't pogo stick
    }

	onground = -1;
	velocity[2] += 270;

	jumpKey = true;	// don't jump again until released
}

void    PlayerMove::Accelerate(AccelerateMode mode, const glm::vec3& wishDir, float wishSpeed, float accel)
{
	// if (pmove.dead)
	// 	return;
	// if (pmove.waterjumptime)
	// 	return;
    float wishSpd = wishSpeed;
    if (mode == InAir && wishSpd > 30) {
        wishSpd = 30;
    }
    float currentspeed = glm::dot(velocity, wishDir);
    float addSpeed = wishSpd - currentspeed;
	if (addSpeed <= 0) {
		return;
    }
	float accelSpeed = accel * wishSpeed * frameTime;
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
	if (onground != -1) {
        glm::vec3   start;
        glm::vec3   stop;
		start[0] = stop[0] = origin[0] + velocity[0] / speed * 16;
		start[1] = stop[1] = origin[1] + velocity[1] / speed * 16;
		start[2] = origin[2] + player.mins[2];
		stop[2]  = start[2] - 34;

		Trace   trace = MovePlayer(start, stop);
		if (trace.fraction == 1) {
			friction *= 2;
		}
	}

    float drop = 0;

	/* if (waterlevel >= 2) // apply water friction
		drop += speed*movevars.waterfriction*waterlevel*frametime;
	else  */if (onground != -1) {
        // Apply ground friction
		float control = (speed < 100/* movevars.stopspeed */) ? 100/* movevars.stopspeed */ : speed;
		drop += control * friction * frameTime;
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

void	PlayerMove::CategorizePosition (void)
{
	// If the player hull point one unit down is solid, the player is on ground
	// See if standing on something solid
	glm::vec3	point = origin;
	point[2] -= 1;
	if (velocity[2] > 180) {
		onground = -1;
	} else {
		Trace tr = MovePlayer(origin, point);
		if (tr.plane.Normal()[2] < 0.7) {
			onground = -1;	// too steep
		} else {
			onground = 1;//tr.entity;
		}
		if (onground != -1) {
			//pmove.waterjumptime = 0;
			if (!tr.startSolid && !tr.allSolid) {
				origin = tr.end;
			}
		}

		// Standing on an entity other than the world
        TouchEnt(tr.entity);
	}

	// Get waterlevel
	// waterlevel = 0;
	// watertype = CONTENTS_EMPTY;

	// point[2] = pmove.origin[2] + player_mins[2] + 1;	
	// cont = PM_PointContents (point);

	// if (cont <= CONTENTS_WATER)
	// {
	// 	watertype = cont;
	// 	waterlevel = 1;
	// 	point[2] = pmove.origin[2] + (player_mins[2] + player_maxs[2])*0.5;
	// 	cont = PM_PointContents (point);
	// 	if (cont <= CONTENTS_WATER)
	// 	{
	// 		waterlevel = 2;
	// 		point[2] = pmove.origin[2] + 22;
	// 		cont = PM_PointContents (point);
	// 		if (cont <= CONTENTS_WATER)
	// 			waterlevel = 3;
	// 	}
	// }
    TouchEnt(bsp.PointContent(point).entity);
}

void	PlayerMove::NudgePosition()
{
	glm::vec3	base = origin;
	for (int i = 0; i<3 ; i++) {
		origin[i] = int(origin[i] * 8) * 0.125;
	}
	//	pmove.origin[2] += 0.124;

	//	if (pmove.dead)
	//		return;		// might be a squished point, so don'y bother
	//	if (PM_TestPlayerPosition (pmove.origin) )
	//		return;
	glm::vec3	sign = {0, -1, 1};

	for (int z = 0; z <= 2; z++) {
		for (int x = 0; x <= 2; x++) {
			for (int y = 0; y <= 2; y++) {
				origin[0] = base[0] + (sign[x] * 1.0/8);
				origin[1] = base[1] + (sign[y] * 1.0/8);
				origin[2] = base[2] + (sign[z] * 1.0/8);
				if (TestPlayerPosition(origin)) {
					return;
				}
			}
		}
	}
	origin = base;
//	Con_DPrintf ("NudgePosition: stuck\n");
}

Trace	PlayerMove::MovePlayer(const glm::vec3& start, const glm::vec3& end)
{
	Trace trace;
	bsp.TraceLine(start, end, trace);
	return trace;
}

bool	PlayerMove::TestPlayerPosition(const glm::vec3& pos)
{
	return bsp.TracePoint(pos).content != SOLID;
}

void    PlayerMove::TouchEnt(EntPtr entity)
{
    if (entity == nullptr) {
        return;
    }
    if (entity == &bsp.actEntities[0]) {
        // Don't care for wolrdspawn
        return;
    }
    for (int te = 0; te < numTouch; ++te) {
        if (touchEnts[te] == entity) {
            return;
        }
    }
    touchEnts[numTouch++] = entity;
}