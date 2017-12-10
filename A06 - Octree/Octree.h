// Programmer: James Preston Meeks - JPM4447@g.rit.edu
// A class that calculates and controls octants for use in eliminating unnessecary collison checking by subdividing the area that is being checked

#ifndef __MYOCTANTCLASS_H_
#define __MYOCTANTCLASS_H_

#include "MyEntityManager.h"

namespace Simplex
{
	//System Class
	class Octree
	{
		static uint octCount;	// will store the number of octants instantiated
		static uint maxLevel;	// will store the maximum level and octant can go to
		static uint entCount;	// will tell the best number of entities this subject can contain
		static uint leaf;		// gives how many leaves connected to this octant

		uint uID = 0;		// will store the current ID for this octant
		uint level = 0;		// will store the minimum vector of the octant
		uint children = 0;	// stores the number of children on the octant

		glm::vec3 size = glm::vec3(0.0f);	// size of the octant

		MeshManager* meshMngr = nullptr;	// mesh manager singleton
		MyEntityManager* entMan = nullptr;	// entity manager singleton

		glm::vec3 v3Center = glm::vec3(0.0f);	// will store the center point of the octant
		glm::vec3 v3Min = glm::vec3(0.0f);		// stores the minimum vector of the octant
		glm::vec3 v3Max = glm::vec3(0.0f);		// stores the maxiumu vector of the octant

		Octree* parent = nullptr;	// will store the parent of the current octant
		Octree* childArray[8];		// stores the child of the current octant

		std::vector<uint> entList;	// list of entities under this octant, index in the entity manager
		uint currentCount = 0;			

		Octree* root = nullptr;			// root octant
		std::vector<Octree*> childVec;	// list of nodes that contain objects, only applied to root

	public:
		// variables
		//bool wireframe = true;

		// constructors
		Octree(uint p_MaxLevel = 2, uint p_EntityCount = 5);
		Octree(glm::vec3 p_Center, glm::vec3 p_Size);

		// rule of three
		Octree(Octree const& other);
		Octree& operator=(Octree const& other);
		~Octree();

		// helper methods
		void Swap(Octree& other);	// will swap the value of this Octant with a given other
		glm::vec3 GetSize();				// returns this octant's size
		glm::vec3 GetCenterGlobal();		// gets the global center of the octant
		glm::vec3 GetMinGlobal();			// gets the global minimum corner  of the octant
		glm::vec3 GetMaxGlobal();			// gets the global maximum corner  of the octant
		uint GetOctCount();					// returns the OctCount
		uint GetLeafCount();				// returns the leafCount

		Octree* GetChild(uint index);	// returns the child specified by the index
		Octree* GetParent();			// returns the parent of the octant

		void Display(uint index, glm::vec3 color = C_BLUE);		// displays the OctantClass volume specified by index including te objects underneath
		void Display(glm::vec3 color = C_BLUE);					// displays the Octant class in the color specified
		void DisplayAll(glm::vec3 color = C_BLUE);				// displays the non-empty leaves in the octree

		bool IsColliding(uint index);	// checks if there is a collision given the index of an Entity in EntityManager
		void Subdivide();				// allocates 8 smaller octants in the child pointers
		void ClearEntityList();			// clears the entity list for each node

		bool IsLeaf();							// Asks the octant if it does not contain any children (Is it a leaf?)
		bool ContainsMoreThan(uint entities);	// checks if the octant contains more than a given amount of Bounding Objects

		void KillBranches();					// deletes all children and the children of their children
		void ConfigureDimensions();				// recursive method to set the dimensions for all leaves
		// void ConstructTree(uint maxLvl = 3);	// creates a tree using subdivisions, the max number of objects and levels
		// void AssignIDToEntity();				// traverse the tree up to the leaves and set the object in them to the index 

	private:
		void Release();			// dealloc member fields
		void Init();			// alloc member fields
	};
} 

#endif


