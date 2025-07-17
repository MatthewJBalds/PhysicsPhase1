#include "ContactResolver.h"
#include <iostream>

namespace Physics
{

	void Physics::ContactResolver::ResolveContacts(std::vector<ParticleContact*> contacts, float time)
	{
		// reset iteration counter
		current_iteration = 0;

		// no contacts
		if (contacts.empty()) return;

		// continue resolving contacts until iteration limit
		while (current_iteration < max_iterations) {
			// find contact with least separating speed
			float min_separating_speed = 0;
			unsigned index = contacts.size();

			for (unsigned i = 0; i < contacts.size(); i++) {
				float separating_speed = contacts[i]->GetSeparatingSpeed();

				if (separating_speed < min_separating_speed) {
					min_separating_speed = separating_speed;
					index = i;
				}
			}

			if (index == contacts.size()) break;

			// resolve contact
			contacts[index]->Resolve(time);


			// increment iteration counter
			current_iteration++;
		}
	}

}
