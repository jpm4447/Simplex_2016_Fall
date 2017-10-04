#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	//m_sProgrammer = "Preston Meeks - jpm4447@g.rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));

	//Make MyMesh object
	m_pMesh = new MyMesh[46];
	for(int x = 0; x < 46; x++)
		m_pMesh[x].GenerateCube(2.0f, C_WHITE);
	
	//Make MyMesh object
	m_pMesh1 = new MyMesh();
	m_pMesh1->GenerateCube(1.0f, C_WHITE);
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

	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	vector3 v3Position(-4, -2, 0);
	static matrix4 m4Model;

	m4Model = glm::translate(IDENTITY_M4, v3Position);
	//m4Model = glm::translate(IDENTITY_M4,  vector3(0, 0, 0.5));
	//m4Model = glm::rotate(m4Model, 1.0f, vector3(0, 0, 1));

	m_pMesh[0].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(0, 0, 0)));
	m_pMesh[1].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(2, 0, 0)));
	m_pMesh[2].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(6, 0, 0)));
	m_pMesh[3].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(8, 0, 0)));

	m_pMesh[4].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(-2, 2, 0)));
	m_pMesh[5].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(-6, 2, 0)));
	m_pMesh[6].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(10, 2, 0)));
	m_pMesh[7].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(14, 2, 0)));

	m_pMesh[8].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(-6, 4, 0)));
	m_pMesh[9].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(-2, 4, 0)));
	m_pMesh[10].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(0, 4, 0)));
	m_pMesh[11].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(2, 4, 0)));
	m_pMesh[12].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(4, 4, 0)));
	m_pMesh[13].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(6, 4, 0)));
	m_pMesh[14].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(8, 4, 0)));
	m_pMesh[15].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(10, 4, 0)));
	m_pMesh[15].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(14, 4, 0)));

	m_pMesh[16].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(-6, 6, 0)));
	m_pMesh[17].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(-4, 6, 0)));
	m_pMesh[18].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(-2, 6, 0)));
	m_pMesh[19].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(0, 6, 0)));
	m_pMesh[20].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(2, 6, 0)));
	m_pMesh[21].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(4, 6, 0)));
	m_pMesh[22].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(6, 6, 0)));
	m_pMesh[23].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(8, 6, 0)));
	m_pMesh[24].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(10, 6, 0)));
	m_pMesh[25].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(12, 6, 0)));
	m_pMesh[26].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(14, 6, 0)));

	m_pMesh[27].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(-4, 8, 0)));
	m_pMesh[28].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(-2, 8, 0)));
	m_pMesh[29].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(2, 8, 0)));
	m_pMesh[30].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(4, 8, 0)));
	m_pMesh[31].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(6, 8, 0)));
	m_pMesh[32].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(10, 8, 0)));
	m_pMesh[33].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(12, 8, 0)));

	m_pMesh[34].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(-2, 10, 0)));
	m_pMesh[35].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(0, 10, 0)));
	m_pMesh[36].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(2, 10, 0)));
	m_pMesh[37].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(4, 10, 0)));
	m_pMesh[38].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(6, 10, 0)));
	m_pMesh[39].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(8, 10, 0)));
	m_pMesh[40].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(10, 10, 0)));

	m_pMesh[34].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(-2, 10, 0)));
	m_pMesh[35].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(0, 10, 0)));
	m_pMesh[36].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(2, 10, 0)));
	m_pMesh[37].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(4, 10, 0)));
	m_pMesh[38].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(6, 10, 0)));
	m_pMesh[39].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(8, 10, 0)));
	m_pMesh[40].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(10, 10, 0)));

	m_pMesh[41].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(0, 12, 0)));
	m_pMesh[42].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(8, 12, 0)));

	m_pMesh[43].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(-2, 14, 0)));
	m_pMesh[44].Render(m4Projection, m4View, m4Model * glm::translate(IDENTITY_M4, vector3(10, 14, 0)));


	v3Position = v3Position + vector3(0, 0, 1);
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
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
	if (m_pMesh != nullptr)
	{
		delete m_pMesh;
		m_pMesh = nullptr;
	}
	SafeDelete(m_pMesh1);
	//release GUI
	ShutdownGUI();
}