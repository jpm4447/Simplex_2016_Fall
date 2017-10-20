#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	m_sProgrammer = "James Preston Meeks - jpm4447@g.rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUp(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)

	// fill the vector of vector of vector3s
	for (int x = 0; x < m_uOrbits; x++)
	{
		orbitSize.resize(m_uOrbits);
		//orbitSize[x].resize(3);
	}

	uint uSides = 3; //start with the minimal 3 sides
	
	// create the readable orbit points, this creates orbits given the number of orbits and points 
	for (int x = 0; x < m_uOrbits; x++)
	{
		for (int y = 0; y < m_uOrbits + uSides; y++)
		{
			float pointX = fSize * cos((y * (2 * (float)PI)) / (uSides + x));
			float pointY = fSize * sin((y * (2 * (float)PI)) / (uSides + x));
	
			orbitSize[x].push_back(vector3(pointX, pointY, 0.0f));
		}
		fSize += .5f;
	}

	// reset  sides and size values to original
	uSides = 3;
	fSize = 1.0f;
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager
		//orbitSize[i-uSides].push_back(glm::vec3(fSize, 0, 0));
		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	m4Offset = glm::rotate(IDENTITY_M4, -90.0f, AXIS_Z);

	// create a timer
	static float timer = 0;						//store the new timer
	static uint uClock = m_pSystem->GenClock(); //generate a new clock for that timer
	timer += m_pSystem->GetDeltaTime(uClock);	//get the delta time for that timer

	float percentage = MapValue(timer, 0.0f, 2.0f, 0.0f, 1.0f);	// will be used to calculate the completion of traveling between two points

	// draw a shapes
	uint points = 3;
	for (uint x = 0; x < m_uOrbits; x++)
	{
		for (uint y = 0; y < points; y++)
		{
			m_pMeshMngr->AddMeshToRenderList(m_shapeList[x], glm::rotate(m4Offset, 90.0f, AXIS_X));

			//calculate the current position
			vector3 v3CurrentPos = orbitSize[x][y];		// CHANGES start SPEHERE LOCATION
			v3CurrentPos = glm::lerp(orbitSize[x][y], orbitSize[x][(y + 1) % orbitSize[x].size()], percentage);

			if (percentage > 1.0f)
			{
				y++;
				timer = m_pSystem->GetDeltaTime(uClock);
				y %= orbitSize[x].size();	// make sure the node does not exceed bounds
			}

			//vector3 v3CurrentPos = glm::vec3(1.0f + (float)x, 0.0f, 0.0f);
			matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);

			//draw spheres
			m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
		}
		points++;
	}

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release GUI
	ShutdownGUI();
}