#include "Octree.h"
using namespace Simplex;

//declaring static variables
uint Octree::octCount;
uint Octree::maxLevel;
uint Octree::entCount;
uint Octree::leaf;

// private methods
void Simplex::Octree::Release()
{
	if (level == 0)
	{
		KillBranches();
	}

	children = 0;
	size = glm::vec3(0.0f);
	entList.clear();
	childVec.clear();
}

void Octree::Init()
{
	entMan = MyEntityManager::GetInstance();
	meshMngr = MeshManager::GetInstance();

	uID = octCount;
	octCount++;
}

Simplex::Octree::Octree(uint topLvl, uint bestEntCount)
{
	//creates the root octant
	Init();

	root = this;

	maxLevel = topLvl;
	entCount = bestEntCount;

	//sets the max/min to center
	v3Max = v3Min = entMan->GetRigidBody()->GetCenterGlobal();
	currentCount = entMan->GetEntityCount();
	for (int x = 0; x < currentCount; x++)
	{
		entList.push_back(x);

		//get min/max
		vector3 rb_min = entMan->GetRigidBody(x)->GetMinGlobal();
		vector3 rb_max = entMan->GetRigidBody(x)->GetMaxGlobal();

		// Setting min and max for X
		if (rb_min.x < v3Min.x)
		{
			v3Min.x = rb_min.x;
		}
		if (rb_max.x > v3Max.x)
		{
			v3Max.x = rb_max.x;
		}

		// Setting min and max for Y
		if (rb_min.y < v3Min.y)
		{
			v3Min.y = rb_min.y;
		}
		if (rb_max.y > v3Max.y)
		{
			v3Max.y = rb_max.y;
		}

		// Setting min and max for Z
		if (rb_min.z < v3Min.z)
		{
			v3Min.z = rb_min.z;
		}
		if (rb_max.z > v3Max.z)
		{
			v3Max.z = rb_max.z;
		}
	}

	//calculate center/size
	v3Center = (v3Min + v3Max) / 2.f;
	size = v3Max - v3Min;

	//create appropriate children
	Subdivide();

	//add leaf dimensions
	ConfigureDimensions();
}

Simplex::Octree::Octree(glm::vec3 center, glm::vec3 newSize)
{
	Init();

	v3Center = center;
	size = newSize;

	v3Max = center + (glm::vec3(newSize) / 2.0f);
	v3Min = center - (glm::vec3(newSize) / 2.0f);
}

Simplex::Octree::Octree(Octree const & other)
{
	Init();

	//copying data over
	level = other.level;
	size = other.size;
	v3Center = other.v3Center;
	v3Min = other.v3Min;
	v3Max = other.v3Max;
	parent = other.parent;
	//~Octree(void);
	Release();

	//loops through and recursively copy/create more nodes
	children = other.children;
	for (int x = 0; x < children; x++)
	{
		childArray[x] = new Octree(*other.childArray[x]);
	}

	//create a new entity list
	currentCount = other.currentCount;
	for (int x = 0; x < currentCount; x++)
	{
		entList.push_back(other.entList[x]);
	}

	root = other.root;
	if (this == root)
	{
		float childCount = other.childVec.size();
		for (int x = 0; x < children; x++)
		{
			childVec.push_back(other.childVec[x]);
		}
	}
}

Octree & Simplex::Octree::operator=(Octree const & other)
{
	Release();
	Init();
	Octree temp(other);
	Swap(temp);

	return *this;
}

// deconstructor
Simplex::Octree::~Octree()
{
	Release();
}

// methods
void Simplex::Octree::Swap(Octree & other)
{
	std::swap(children, other.children);

	std::swap(size, other.size);
	std::swap(uID, other.uID);
	std::swap(root, other.root);
	std::swap(childVec, other.childVec);

	std::swap(v3Center, other.v3Center);
	std::swap(v3Max, other.v3Max);
	std::swap(v3Min, other.v3Min);

	meshMngr = MeshManager::GetInstance();
	entMan = MyEntityManager::GetInstance();

	std::swap(level, other.level);
	std::swap(parent, other.parent);
	for (int x = 0; x < 8; x++)
	{
		std::swap(childArray[x], other.childArray[x]);
	}
}

glm::vec3 Simplex::Octree::GetSize()
{
	return size;
}

glm::vec3 Simplex::Octree::GetCenterGlobal()
{
	return v3Center;
}

glm::vec3 Simplex::Octree::GetMinGlobal()
{
	return v3Min;
}

glm::vec3 Simplex::Octree::GetMaxGlobal()
{
	return v3Max;
}

uint Simplex::Octree::GetOctCount()
{
	return octCount;
}

uint Simplex::Octree::GetLeafCount()
{
	return leaf;
}

Octree * Simplex::Octree::GetChild(uint index)
{
	if (children == 0 || index > 7)
		return nullptr;

	else
		return childArray[index];
}

Octree * Simplex::Octree::GetParent()
{
	return parent;
}

// display methods
void Simplex::Octree::Display(uint index, glm::vec3 color)
{
	if (index >= octCount)
	{
		DisplayAll();
		return;
	}
	childVec[index]->Display(color);
}

void Simplex::Octree::Display(glm::vec3 color)
{
	meshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, v3Center) * glm::scale(IDENTITY_M4, size), color);
}

void Simplex::Octree::DisplayAll(glm::vec3 color)
{
	if (IsLeaf())
	{
		Display(color);
	}
	else
	{
		for (int x = 0; x < children; x++)
		{
			childArray[x]->DisplayAll(color);
		}
	}
}


bool Simplex::Octree::IsColliding(uint index)
{
	uint objCount = entMan->GetEntityCount();
	if (index >= objCount)
	{
		return false;
	}

	MyRigidBody* rigid = entMan->GetRigidBody(index);

	glm::vec3 v3MinOcta = rigid->GetMinGlobal();
	glm::vec3 v3MaxOcta = rigid->GetMaxGlobal();

	//// check collision detections
	//// x check
	//if (v3Max.x < v3MinOcta.x)
	//	return false;
	//if (v3Min.x < v3MaxOcta.x)
	//	return false;
	//
	//// x check
	//if (v3Max.y < v3MinOcta.y)
	//	return false;
	//if (v3Min.y < v3MaxOcta.y)
	//	return false;
	//
	//// x check
	//if (v3Max.z < v3MinOcta.z)
	//	return false;
	//if (v3Min.z < v3MaxOcta.z)
	//	return false;
	//
	//return true;

	if (v3MaxOcta.x > v3Min.x &&
		v3MaxOcta.y > v3Min.y &&
		v3MaxOcta.z > v3Min.z &&
		v3MinOcta.x < v3Max.x &&
		v3MinOcta.y < v3Max.y &&
		v3MinOcta.z < v3Max.z)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Simplex::Octree::Subdivide()
{
	//// check if this is capable of becoming subdivided
	//if (level >= maxLevel || children != 0)
	//{
	//	return;
	//}

	//stops subdividing if at desired level of there's 5 or less entity in the octant
	if (level >= maxLevel || !ContainsMoreThan(entCount))
	{
		root->childVec.push_back(this);
		leaf += 1;
		return;
	}

	//prints out if someone is trying to subdivide an octant that's not a leaf
	if (children == 8)
	{
		return;
	}

	// it is not a leaf anymore
	children = 8;

		//glm::vec3 tempSize = size / 4.0f;
		//
		//// set start at bottom left
		//glm::vec3 center = v3Center;
		//center.x -= tempSize.x;
		//center.y -= tempSize.y;
		//center.z -= tempSize.z;
		//childArray[0] = new Octree(center, size * 2.0f);
		//
		//center.x += tempSize.x;
		//childArray[1] = new Octree(center, size * 2.0f);
		//
		//center.z += tempSize.z;
		//childArray[2] = new Octree(center, size * 2.0f);
		//
		//center.x -= tempSize.x;
		//childArray[3] = new Octree(center, size * 2.0f);
		//
		//center.y += tempSize.y;
		//childArray[4] = new Octree(center, size * 2.0f);
		//
		//center.z -= tempSize.z;
		//childArray[5] = new Octree(center, size * 2.0f);
		//
		//center.x += tempSize.x;
		//childArray[6] = new Octree(center, size * 2.0f);
		//
		//center.z += tempSize.z;
		//childArray[7] = new Octree(center, size * 2.0f);

	// create an octant at the correct position
	childArray[0] = new Octree(v3Center + vector3(-size.x / 4, size.y / 4, -size.z / 4), size / 2.f);
	childArray[1] = new Octree(v3Center + vector3(-size.x / 4, size.y / 4, size.z / 4), size / 2.f);

	childArray[2] = new Octree(v3Center + vector3(-size.x / 4, -size.y / 4, -size.z / 4), size / 2.f);
	childArray[3] = new Octree(v3Center + vector3(-size.x / 4, -size.y / 4, size.z / 4), size / 2.f);

	childArray[4] = new Octree(v3Center + vector3(size.x / 4, -size.y / 4, -size.z / 4), size / 2.f);
	childArray[5] = new Octree(v3Center + vector3(size.x / 4, -size.y / 4, size.z / 4), size / 2.f);

	childArray[6] = new Octree(v3Center + vector3(size.x / 4, size.y / 4, -size.z / 4), size / 2.f);
	childArray[7] = new Octree(v3Center + vector3(size.x / 4, size.y / 4, size.z / 4), size / 2.f);


	// set up the children
	for (int x = 0; x < children; x++)
	{
		childArray[x]->parent = this;
		childArray[x]->level = level + 1;
		childArray[x]->root = root;

		// get colliding rigid body
		for (int y = 0; y < currentCount; y++)
		{
			if (childArray[x]->IsColliding(entList[y]))
			{
				childArray[x]->entList.push_back(entList[y]);
			}
		}

		// update entity count
		childArray[x]->currentCount = childArray[x]->entList.size();


		childArray[x]->Subdivide();
	}
}

void Simplex::Octree::ClearEntityList()
{
	for (int x = 0; x < octCount; x++)
	{
		childArray[x]->ClearEntityList();
	}
	entList.clear();
}

bool Simplex::Octree::IsLeaf(void)
{
	return children == 0;
}

bool Simplex::Octree::ContainsMoreThan(uint entities)
{
	//int count = 0;
	//int objCount = entMan->GetEntityCount();
	//for (int x = 0; x < objCount; x++)
	//{
	//	if (IsColliding(x))
	//	{
	//		count++;
	//	}
	//	if (count > entities)
	//	{
	//		return true;
	//	}
	//}
	//return false;
	return (currentCount > entities);
}

void Simplex::Octree::KillBranches()
{
	if (IsLeaf())
	{
		return;
	}
	else
	{
		for (int x = 0; x < children; x++)
		{
			childArray[x]->KillBranches();
			SafeDelete(childArray[x]);
		}
	}
}

void Simplex::Octree::ConfigureDimensions()
{
	if (IsLeaf())
	{
		for (int x = 0; x < currentCount; x++)
		{
			entMan->AddDimension(entList[x], uID);
		}
	}
	else
	{
		for (int x = 0; x < children; x++)
		{
			childArray[x]->ConfigureDimensions();
		}
	}
}
