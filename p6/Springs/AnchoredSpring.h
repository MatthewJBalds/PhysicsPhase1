#pragma once
#include "../MyVector.h"
#include "../ForceGenerator.h"

namespace Physics {
	class AnchoredSpring : public ForceGenerator{
		
		private:
			MyVector anchorPoint;

			float springConstant;

			float restLength;

		public:

			AnchoredSpring(MyVector pos, float _springConst, float _restLen):
				anchorPoint(pos), springConstant(_springConst), restLength(_restLen){}

			//override te update force of the generator
			void UpdateForce(PhysicsParticle* particle, float time) override;


	};
}
