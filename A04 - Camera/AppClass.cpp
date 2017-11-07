#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	////Change this to your name and email
	m_sProgrammer = "James Preston Meeks - jpm4447@g.rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));

	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUp(AXIS_Z * 10.0f, ZERO_V3, AXIS_Y);

	// initialize variables
	m_v3Pos = vector3(0.0f, 2.0f, 0.0f);
	angleX = 0.0f;

	//init the camera
	m_pCamera = new MyCamera();
	//vector3(0.0f, 3.0f, 20.0f)
	m_pCamera->SetPositionTargetAndUp(
			m_v3Pos, //Where my eyes are
			vector3(0.0f, 3.0f, 19.0f), //where what I'm looking at is
			AXIS_Y);					//what is up

	//Get the singleton
	m_pMyMeshMngr = MyMeshManager::GetInstance();
	m_pMyMeshMngr->SetCamera(m_pCamera);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Add objects to the Manager
	for (int j = -50; j < 50; j += 2)
	{
		for (int i = -50; i < 50; i += 2)
		{
			m_pMyMeshMngr->AddConeToRenderList(glm::translate(vector3(i, 0.0f, j)));
		}
	}
}
void Application::Display(void)
{
	//Clear the screen
	ClearScreen();
	
	// debug line, comment before submission
	//std::cout << angleY << std::endl;

	// set the position of the camera
	m_pCamera->SetPosition(m_v3Pos);

	// update the camera
	//m_qQuaternion;

	// calculate the forward and up using a combination of quaternions and vector4
	vector4 f = ToMatrix4(m_qQuaternionY * m_qQuaternionX) * vector4(0.0f, 0.0f, 1.0f, 1.0f);
	vector4 u = ToMatrix4(m_qQuaternionY * m_qQuaternionX) * vector4(0.0f, 1.0f, 0.0f, 1.0f);

	// translate forward and up values into vector3s so that glm::lookAt can read them
	vector3 forward = vector3(f.x, f.y, f.z);
	vector3 up = vector3(u.x, u.y, u.z);

	//vector4 lookAt = vector4(0.0f, 0.0f, 1.0f, 1.0f) * glm::lookAt(m_v3Pos, forward, up);
	vector4 lookAt = vector4(m_v3Pos + forward, 1.0f);
	
	// Set the target for the camera using lookat
	//lookAt += vector4(m_v3Pos, 1.0f);
	m_pCamera->SetTarget(vector3(lookAt.x, lookAt.y, lookAt.z));



	//clear the render list
	m_pMeshMngr->ClearRenderList();

	//Render the list of MyMeshManager
	m_pMyMeshMngr->Render();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the MyMeshManager list
	m_pMyMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release the singleton
	MyMeshManager::ReleaseInstance();

	//release the camera
	SafeDelete(m_pCamera);

	//release GUI
	ShutdownGUI();
}
