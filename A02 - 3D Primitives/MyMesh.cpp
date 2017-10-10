#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// variables
	vector3 topPoint(0.0f, a_fHeight, 0.0f);
	vector3 btmPoint(0.0f, 0.0f, 0.0f);
	std::vector<vector3> pointRing;

	// calculate positions around the base point using subdivisions and radius
	for (int p = 0; p < a_nSubdivisions; p++)
	{
		float pointX = a_fRadius * cos(((p * (2 * (float)PI)) / a_nSubdivisions) );
		float pointY = 0.0f;
		float pointZ = a_fRadius * sin((p * (2 * (float)PI)) / a_nSubdivisions);

		pointRing.push_back(vector3(pointX, pointY, pointZ));
	}

	// render out faces
	for (int x = 0; x < a_nSubdivisions; x++)
	{
		if (x == a_nSubdivisions - 1)
		{
			// addTri(top, x, 0)
			AddTri(pointRing[0], pointRing[x], topPoint);
			
			// addTri(bottom, x, 0)
			AddTri(btmPoint, pointRing[x], pointRing[0]);
		}
		else
		{
			// addTri(top, x, x++)
			AddTri(pointRing[x + 1], pointRing[x], topPoint);

			// addTri(bottom, x, x++)
			AddTri(btmPoint, pointRing[x], pointRing[x + 1]);
		}
	}


	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// variables
	vector3 topPoint(0.0f, a_fHeight, 0.0f);
	vector3 btmPoint(0.0f, 0.0f, 0.0f);
	std::vector<vector3> pointRingTop;
	std::vector<vector3> pointRingBtm;

	// calculate positions around the base point and top point using subdivisions and radius
	for (int p = 0; p < a_nSubdivisions; p++)
	{
		float pointX = a_fRadius * cos(((p * (2 * (float)PI)) / a_nSubdivisions));
		float pointZ = a_fRadius * sin((p * (2 * (float)PI)) / a_nSubdivisions);

		pointRingBtm.push_back(vector3(pointX, 0.0f, pointZ));
		pointRingTop.push_back(vector3(pointX, a_fHeight, pointZ));
	}

	// render out faces
	for (int x = 0; x < a_nSubdivisions; x++)
	{
		if (x == a_nSubdivisions - 1)
		{
			// addTri(top, x, 0)
			AddTri(pointRingTop[0], pointRingTop[x], topPoint);

			// addTri(bottom, x, 0)
			AddTri(btmPoint, pointRingBtm[x], pointRingBtm[0]);

			// addQuad(btm0, btm, top0, top)
			AddQuad(pointRingBtm[0], pointRingBtm[x], pointRingTop[0], pointRingTop[x]);
		}
		else
		{
			// addTri(top, x, x++)
			AddTri(pointRingTop[x + 1], pointRingTop[x], topPoint);

			// addTri(bottom, x, x++)
			AddTri(btmPoint, pointRingBtm[x], pointRingBtm[x + 1]);

			// addQuad(btm+1, btm, top+1, top)
			AddQuad(pointRingBtm[x + 1], pointRingBtm[x], pointRingTop[x + 1], pointRingTop[x]);
		}
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// variables
	vector3 topPoint(0.0f, a_fHeight, 0.0f);
	vector3 btmPoint(0.0f, 0.0f, 0.0f);
	std::vector<vector3> outerPointRingTop;
	std::vector<vector3> outerPointRingBtm;
	std::vector<vector3> innerPointRingTop;
	std::vector<vector3> innerPointRingBtm;

	// calculate positions around the base point and top point using subdivisions and radius
	for (int p = 0; p < a_nSubdivisions; p++)
	{
		// outer radius
		float oPointX = a_fOuterRadius * cos(((p * (2 * (float)PI)) / a_nSubdivisions));
		float oPointZ = a_fOuterRadius * sin((p * (2 * (float)PI)) / a_nSubdivisions);

		outerPointRingBtm.push_back(vector3(oPointX, 0.0f, oPointZ));
		outerPointRingTop.push_back(vector3(oPointX, a_fHeight, oPointZ));

		// inner radius
		float iPointX = a_fInnerRadius * cos(((p * (2 * (float)PI)) / a_nSubdivisions));
		float iPointZ = a_fInnerRadius * sin((p * (2 * (float)PI)) / a_nSubdivisions);

		innerPointRingBtm.push_back(vector3(iPointX, 0.0f, iPointZ));
		innerPointRingTop.push_back(vector3(iPointX, a_fHeight, iPointZ));
	}

	// render out faces
	for (int x = 0; x < a_nSubdivisions; x++)
	{
		if (x == a_nSubdivisions - 1)
		{
			// sides
			// addQuad(btm0, btm, top0, top)
			AddQuad(outerPointRingBtm[0], outerPointRingBtm[x], outerPointRingTop[0], outerPointRingTop[x]);

			// addQuad(top, top0, btm, btm0)
			AddQuad(innerPointRingTop[0], innerPointRingTop[x], innerPointRingBtm[0], innerPointRingBtm[x]);

			// top and bottom
			// addQuad(btm+1, btm, top+1, top)
			AddQuad(outerPointRingTop[0], outerPointRingTop[x], innerPointRingTop[0], innerPointRingTop[x]);

			// addQuad(top, top+1, btm, btm+1)
			AddQuad(innerPointRingBtm[0], innerPointRingBtm[x], outerPointRingBtm[0], outerPointRingBtm[x]);
		}
		else
		{
			// sidea
			// addQuad(btm+1, btm, top+1, top)
			AddQuad(outerPointRingBtm[x + 1], outerPointRingBtm[x], outerPointRingTop[x + 1], outerPointRingTop[x]);

			// addQuad(top, top+1, btm, btm+1)
			AddQuad(innerPointRingTop[x + 1], innerPointRingTop[x], innerPointRingBtm[x + 1], innerPointRingBtm[x]);

			// top and bottom
			// addQuad(btm+1, btm, top+1, top)
			AddQuad(outerPointRingTop[x + 1], outerPointRingTop[x], innerPointRingTop[x + 1], innerPointRingTop[x]);

			// addQuad(top, top+1, btm, btm+1)
			AddQuad(innerPointRingBtm[x + 1], innerPointRingBtm[x], outerPointRingBtm[x + 1], outerPointRingBtm[x]);
		}
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	GenerateCube(a_fOuterRadius * 2.0f, a_v3Color);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;

	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	GenerateCube(1.0f, a_v3Color);

	//// variables
	//vector3 topPoint(0.0f, (2 * a_fRadius), 0.0f);
	//vector3 btmPoint(0.0f, 0.0f, 0.0f);
	//std::vector<std::vector<vector3>> pointRing(a_nSubdivisions);
	//float ringPos = (2 * a_fRadius) / (a_nSubdivisions + 2);		// provides the height of the 
	//float RING_POS = (2 * a_fRadius) / (a_nSubdivisions + 2);		// a constant value of Ring Position
	//float ringScale = (2 * a_fRadius) / (a_nSubdivisions + 2);		
	//
	//// calculate positions around the base point and top point using subdivisions and radius
	//int halfDivisions = a_nSubdivisions / 2;
	//for (int p = 0; p < halfDivisions; p++)
	//{
	//	for (int q = 0; q < a_nSubdivisions; q++)
	//	{
	//		float pointX = a_fRadius * cos(((p * (2 * (float)PI)) / a_nSubdivisions));
	//		float pointZ = a_fRadius * sin((p * (2 * (float)PI)) / a_nSubdivisions);
	//
	//		pointRing[p].push_back(vector3(pointX * (((a_nSubdivisions + 1) * a_fRadius) / (a_nSubdivisions + 2)), ringPos, pointZ * (((a_nSubdivisions + 1) * a_fRadius) / (a_nSubdivisions + 2))));
	//	}
	//	ringPos = ringPos + RING_POS;
	//}
	//
	//for (int p = a_nSubdivisions-1; p > halfDivisions; p--)
	//{
	//	for (int q = 0; q < a_nSubdivisions; q++)
	//	{
	//		float pointX = a_fRadius * cos(((p * (2 * (float)PI)) / a_nSubdivisions));
	//		float pointZ = a_fRadius * sin((p * (2 * (float)PI)) / a_nSubdivisions);
	//
	//		pointRing[p].push_back(vector3(pointX * (((a_nSubdivisions + 1) * a_fRadius) / (a_nSubdivisions + 2)), ringPos, pointZ * (((a_nSubdivisions + 1) * a_fRadius) / (a_nSubdivisions + 2))));
	//	}
	//	ringPos = ringPos + RING_POS;
	//}
	//
	//// render out faces
	//for (int x = 0; x < a_nSubdivisions; x++)
	//{
	//	for (int y = 0; y < a_nSubdivisions; y++)
	//	{
	//		if (x == 0)
	//		{
	//			if (y == a_nSubdivisions - 1)
	//			{
	//				// addTri(bottom, x, 0)
	//				AddTri(btmPoint, pointRing[x][y], pointRing[x][0]);
	//			}
	//			else
	//			{
	//				// addTri(bottom, x, x++)
	//				AddTri(btmPoint, pointRing[x][y], pointRing[x][y + 1]);
	//			}
	//		}
	//		else if (x == a_nSubdivisions)
	//		{
	//			if (y == a_nSubdivisions - 1)
	//			{
	//				// addTri(top, x, 0)
	//				AddTri(pointRing[x][0], pointRing[x][y], topPoint);
	//			}
	//			else
	//			{
	//				// addTri(top, x, x++)
	//				AddTri(pointRing[x][y + 1], pointRing[x][y], topPoint);
	//			}
	//		}
	//		else
	//		{
	//			if (y == a_nSubdivisions - 1)
	//			{
	//				// addQuad(btm0, btm, top0, top)
	//				AddQuad(pointRing[x][0], pointRing[x][y], pointRing[x][0], pointRing[x][y]);
	//			}
	//			else
	//			{
	//				// addQuad(btm+1, btm, top+1, top)
	//				AddQuad(pointRing[x][y + 1], pointRing[x][y], pointRing[x][y + 1], pointRing[x][y]);
	//			}
	//		}
	//	}
	//}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}