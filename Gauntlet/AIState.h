#pragma once

//Enum for the state that an AI Entity is in

namespace AI
{
	enum State
	{
		Patrol,
		Attack,
		Run,
		HealthSearch,
		SeekRouteRoundCover
	};
}