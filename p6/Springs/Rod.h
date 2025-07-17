#pragma once
#include "ParticleLink.h"

namespace Physics {
	class Rod : public ParticleLink {
	public:
		float length = 1;
		float restitution = 0;

		ParticleContact* GetContact() override;
	};
}
