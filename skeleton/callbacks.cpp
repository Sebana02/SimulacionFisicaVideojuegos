#include "callbacks.hpp"

extern void onCollision(physx::PxActor* actor1, physx::PxActor* actor2);

physx::PxFilterFlags contactReportFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
{
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(filterData0);
	PX_UNUSED(filterData1);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);

	// all initial and persisting reports for everything, with per-point data
	pairFlags = physx::PxPairFlag::eSOLVE_CONTACT | physx::PxPairFlag::eDETECT_DISCRETE_CONTACT
		| physx::PxPairFlag::eNOTIFY_TOUCH_FOUND
		| physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS
		| physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;
	return physx::PxFilterFlag::eDEFAULT;
}

void ContactReportCallback::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
	PX_UNUSED(pairs);
	PX_UNUSED(nbPairs);
	physx::PxActor* actor1 = pairHeader.actors[0];
	physx::PxActor* actor2 = pairHeader.actors[1];
	onCollision(actor1, actor2);
}


void MyCollisionCallback::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
	PxRigidActor* actor1 = (PxRigidActor*)pairHeader.actors[0];
	PxRigidActor* actor2 = (PxRigidActor*)pairHeader.actors[1];
	
	Rigidbody* rb1 = (Rigidbody*)actor1->userData;
	Rigidbody* rb2 = (Rigidbody*)actor2->userData;
	
	
	//gestion de todas las colisiones
	

	//clear
	if (rb1->_type == Rigidbody::type::LEAVING_PAINT || rb1->_type == Rigidbody::type::LEAVING_PAINT)
		;

	
	//canvas con pintura
	else if (rb1->_type == Rigidbody::type::ARRIVING_PAINT && rb2->_type == Rigidbody::type::CANVAS)	
		rb1->onCollision(Rigidbody::TO_STOP);

	else if (rb1->_type == Rigidbody::type::CANVAS && rb2->_type == Rigidbody::type::ARRIVING_PAINT) 
		rb2->onCollision(Rigidbody::TO_STOP);



	//pintura con pintura estatica
	else if (rb1->_type == Rigidbody::type::ARRIVING_PAINT && rb2->_type == Rigidbody::type::STATIC_PAINT) 
	{
		rb1->onCollision(Rigidbody::TO_STOP);
		rb2->onCollision(Rigidbody::TO_DELETE);
	}
	else if (rb1->_type == Rigidbody::type::STATIC_PAINT && rb2->_type == Rigidbody::type::ARRIVING_PAINT) 
	{
		rb2->onCollision(Rigidbody::TO_STOP);
		rb1->onCollision(Rigidbody::TO_DELETE);
	}

	
	//pintura con pintura
	else if (rb1->_type == Rigidbody::type::ARRIVING_PAINT && rb2->_type == Rigidbody::type::ARRIVING_PAINT) 
		rb1->onCollision(Rigidbody::TO_DELETE);
	
	

	
	//borrador con pintura
	else if (rb1->_type == Rigidbody::type::ERASING_PAINT && (rb2->_type == Rigidbody::type::STATIC_PAINT || rb2->_type == Rigidbody::type::ARRIVING_PAINT) 
		|| (rb1->_type == Rigidbody::type::STATIC_PAINT || rb1->_type == Rigidbody::type::ARRIVING_PAINT) && rb2->_type == Rigidbody::type::ERASING_PAINT)
	{
		rb1->onCollision(Rigidbody::TO_DELETE);
		rb2->onCollision(Rigidbody::TO_DELETE);
	}

	
	//borrador con canvas
	else if (rb1->_type == Rigidbody::type::ERASING_PAINT && rb2->_type == Rigidbody::type::CANVAS)  
		rb1->onCollision(Rigidbody::TO_DELETE);
	
	else if (rb2->_type == Rigidbody::type::ERASING_PAINT && rb1->_type == Rigidbody::type::CANVAS) 
		rb2->onCollision(Rigidbody::TO_DELETE);
	
	
	//borrador con borrador
	else if (rb1->_type == Rigidbody::type::ERASING_PAINT && rb2->_type == Rigidbody::type::ERASING_PAINT)  
		rb1->onCollision(Rigidbody::TO_DELETE);
	
		
}
