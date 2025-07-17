#include "Rod.h"

namespace Physics {


	ParticleContact* Rod::GetContact() {
		//get cur length
		float currLen = CurrentLength();

		//if cur elength is set lenth do nothing
		if (currLen == length) {
			return nullptr;
		}

		ParticleContact* ret = new ParticleContact();
		ret->particles[0] = particles[0];
		ret->particles[1] = particles[1];
		//get the direction of the contact
		MyVector dir = particles[1]->Position - particles[0]->Position;
		dir = dir.Direction(); //Noramlize->direciton

		if (currLen > length) {
			ret->contactNormal = dir;

			ret->Depth = currLen - length;

		}

		else {
			ret->contactNormal = dir * -1;

			ret->Depth = length - currLen;
		}

		//always set restitution, 0 for rods
		ret->restitution = restitution;

		return ret;

	}
}