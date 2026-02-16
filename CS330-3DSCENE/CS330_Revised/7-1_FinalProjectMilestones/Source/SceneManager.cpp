///////////////////////////////////////////////////////////////////////////////
// scenemanager.cpp
// ============
// manage the preparing and rendering of 3D scenes - textures, materials, lighting
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>
#include <GLFW/glfw3.h>

// declaration of global variables
namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";
}




/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/
SceneManager::SceneManager(ShaderManager* pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
	m_pShaderManager = NULL;
	delete m_basicMeshes;
	m_basicMeshes = NULL;
}

/***********************************************************
 *  CreateGLTexture()
 *
 *  This method is used for loading textures from image files,
 *  configuring the texture mapping parameters in OpenGL,
 *  generating the mipmaps, and loading the read texture into
 *  the next available texture slot in memory.
 ***********************************************************/
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0;
	int height = 0;
	int colorChannels = 0;
	GLuint textureID = 0;

	// indicate to always flip images vertically when loaded
	stbi_set_flip_vertically_on_load(true);

	// try to parse the image data from the specified image file
	unsigned char* image = stbi_load(
		filename,
		&width,
		&height,
		&colorChannels,
		0);

	// if the image was successfully read from the image file
	if (image)
	{
		std::cout << "Successfully loaded image:" << filename << ", width:" << width << ", height:" << height << ", channels:" << colorChannels << std::endl;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if the loaded image is in RGB format
		if (colorChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		// if the loaded image is in RGBA format - it supports transparency
		else if (colorChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			std::cout << "Not implemented to handle image with " << colorChannels << " channels" << std::endl;
			return false;
		}

		// generate the texture mipmaps for mapping textures to lower resolutions
		glGenerateMipmap(GL_TEXTURE_2D);

		// free the image data from local memory
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		// register the loaded texture and associate it with the special tag string
		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;

		return true;
	}

	std::cout << "Could not load image:" << filename << std::endl;

	// Error loading the image
	return false;
}

/***********************************************************
 *  BindGLTextures()
 *
 *  This method is used for binding the loaded textures to
 *  OpenGL texture memory slots.  There are up to 16 slots.
 ***********************************************************/
void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  DestroyGLTextures()
 *
 *  This method is used for freeing the memory in all the
 *  used texture memory slots.
 ***********************************************************/
void SceneManager::DestroyGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glGenTextures(1, &m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  FindTextureID()
 *
 *  This method is used for getting an ID for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureID(std::string tag)
{
	int textureID = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureID = m_textureIDs[index].ID;
			bFound = true;
		}
		else
			index++;
	}

	return(textureID);
}

/***********************************************************
 *  FindTextureSlot()
 *
 *  This method is used for getting a slot index for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureSlot(std::string tag)
{
	int textureSlot = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureSlot = index;
			bFound = true;
		}
		else
			index++;
	}

	return(textureSlot);
}

/***********************************************************
 *  FindMaterial()
 *
 *  This method is used for getting a material from the previously
 *  defined materials list that is associated with the passed in tag.
 ***********************************************************/
bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
	if (m_objectMaterials.size() == 0)
	{
		return(false);
	}

	int index = 0;
	bool bFound = false;
	while ((index < m_objectMaterials.size()) && (bFound == false))
	{
		if (m_objectMaterials[index].tag.compare(tag) == 0)
		{
			bFound = true;
			material.diffuseColor = m_objectMaterials[index].diffuseColor;
			material.specularColor = m_objectMaterials[index].specularColor;
			material.shininess = m_objectMaterials[index].shininess;
		}
		else
		{
			index++;
		}
	}

	return(true);
}

/***********************************************************
 *  SetTransformations()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/
void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	// variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// set the scale value in the transform buffer
	scale = glm::scale(scaleXYZ);
	// set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
	// set the translation value in the transform buffer
	translation = glm::translate(positionXYZ);

	modelView = translation * rotationZ * rotationY * rotationX * scale;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

/***********************************************************
 *  SetShaderColor()
 *
 *  This method is used for setting the passed in color
 *  into the shader for the next draw command
 ***********************************************************/
void SceneManager::SetShaderColor(
	float redColorValue,
	float greenColorValue,
	float blueColorValue,
	float alphaValue)
{
	// variables for this method
	glm::vec4 currentColor;

	currentColor.r = redColorValue;
	currentColor.g = greenColorValue;
	currentColor.b = blueColorValue;
	currentColor.a = alphaValue;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/***********************************************************
 *  SetShaderTexture()
 *
 *  This method is used for setting the texture data
 *  associated with the passed in ID into the shader.
 ***********************************************************/
void SceneManager::SetShaderTexture(
	std::string textureTag)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, true);

		int textureID = -1;
		textureID = FindTextureSlot(textureTag);
		m_pShaderManager->setSampler2DValue(g_TextureValueName, textureID);
	}
}

/***********************************************************
 *  SetTextureUVScale()
 *
 *  This method is used for setting the texture UV scale
 *  values into the shader.
 ***********************************************************/
void SceneManager::SetTextureUVScale(float u, float v)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
	}
}

/***********************************************************
 *  SetShaderMaterial()
 *
 *  This method is used for passing the material values
 *  into the shader.
 ***********************************************************/
void SceneManager::SetShaderMaterial(
	std::string materialTag)
{
	if (m_objectMaterials.size() > 0)
	{
		OBJECT_MATERIAL material;
		bool bReturn = false;

		bReturn = FindMaterial(materialTag, material);
		if (bReturn == true)
		{
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

/***********************************************************
 *  SetupLights()
 *
 *  This method sets up your directional, point, and spot lights
 *  with initial positions, colors, and parameters.
 ***********************************************************/
void SceneManager::SetupSceneLights()
{
	// ===== Directional Light (sunlight simulation) =====
	m_pShaderManager->setVec3Value("directionalLight.direction", -0.5f, -1.0f, -0.3f);
	m_pShaderManager->setVec3Value("directionalLight.ambient", 0.1f, 0.1f, 0.1f);
	m_pShaderManager->setVec3Value("directionalLight.diffuse", 0.3f, 0.3f, 0.3f);
	m_pShaderManager->setVec3Value("directionalLight.specular", 0.5f, 0.5f, 0.5f);
	m_pShaderManager->setBoolValue("directionalLight.bActive", true);

	// ===== Point Light 0 (Overhead, brighter and larger area) =====
	m_pShaderManager->setVec3Value("pointLights[0].position", 0.0f, 5.0f, -0.5f); // higher up
	m_pShaderManager->setVec3Value("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);   // soft ambient
	m_pShaderManager->setVec3Value("pointLights[0].diffuse", 0.7f, 0.7f, 0.7f);   // brighter
	m_pShaderManager->setVec3Value("pointLights[0].specular", 0.8f, 0.8f, 0.8f);  // more shine
	m_pShaderManager->setFloatValue("pointLights[0].constant", 1.0f);
	m_pShaderManager->setFloatValue("pointLights[0].linear", 0.1f);     // smaller ? spreads farther
	m_pShaderManager->setFloatValue("pointLights[0].quadratic", 0.01f); // very small ? slow falloff
	m_pShaderManager->setBoolValue("pointLights[0].bActive", true);

	// ===== Point Light 1 (Fill light) =====
	m_pShaderManager->setVec3Value("pointLights[1].position", -3.0f, 3.0f, -2.0f);
	m_pShaderManager->setVec3Value("pointLights[1].ambient", 0.02f, 0.02f, 0.02f);
	m_pShaderManager->setVec3Value("pointLights[1].diffuse", 0.15f, 0.15f, 0.12f);
	m_pShaderManager->setVec3Value("pointLights[1].specular", 0.1f, 0.1f, 0.08f);
	m_pShaderManager->setFloatValue("pointLights[1].constant", 1.0f);
	m_pShaderManager->setFloatValue("pointLights[1].linear", 0.09f);
	m_pShaderManager->setFloatValue("pointLights[1].quadratic", 0.032f);
	m_pShaderManager->setBoolValue("pointLights[1].bActive", true);

	

	// ===== Spot Light =====
	m_pShaderManager->setVec3Value("spotLight.position", 0.0f, 4.5f, -0.5f);
	m_pShaderManager->setVec3Value("spotLight.direction", 0.0f, -1.0f, 0.0f);
	m_pShaderManager->setVec3Value("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	m_pShaderManager->setVec3Value("spotLight.diffuse", 0.4f, 0.3f, 0.25f);
	m_pShaderManager->setVec3Value("spotLight.specular", 0.3f, 0.25f, 0.2f);
	m_pShaderManager->setFloatValue("spotLight.cutOff", glm::cos(glm::radians(15.0f)));
	m_pShaderManager->setFloatValue("spotLight.outerCutOff", glm::cos(glm::radians(25.0f)));
	m_pShaderManager->setFloatValue("spotLight.constant", 1.0f);
	m_pShaderManager->setFloatValue("spotLight.linear", 1.0f);
	m_pShaderManager->setFloatValue("spotLight.quadratic", 1.5f);
	m_pShaderManager->setBoolValue("spotLight.bActive", true);


	

	

	

	// Enable lighting in shader
	m_pShaderManager->setBoolValue("bUseLighting", true);
}
/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/*** Please refer to the code in the OpenGL sample project  ***/
/*** for assistance.                                        ***/
/**************************************************************/

void SceneManager::DefineObjectMaterials()
{
	// Laptop material
	OBJECT_MATERIAL laptopMaterial;
	laptopMaterial.diffuseColor = glm::vec3(0.2f, 0.2f, 0.2f); // dark gray
	laptopMaterial.specularColor = glm::vec3(0.5f, 0.5f, 0.5f); // moderate highlights
	laptopMaterial.shininess = 32.0f;
	laptopMaterial.tag = "laptop";
	m_objectMaterials.push_back(laptopMaterial);

	// Mouse material
	OBJECT_MATERIAL mouseMaterial;
	mouseMaterial.diffuseColor = glm::vec3(0.88f, 0.88f, 0.88f);
	mouseMaterial.specularColor = glm::vec3(0.8f, 0.8f, 0.8f);
	mouseMaterial.shininess = 64.0f;
	mouseMaterial.tag = "mouse";
	m_objectMaterials.push_back(mouseMaterial);
}


/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene
 *  rendering
 ***********************************************************/
void SceneManager::PrepareScene()
{
	// Load all necessary meshes
	m_basicMeshes->LoadPlaneMesh();     // desk
	m_basicMeshes->LoadBoxMesh();       // computer and mouse
	m_basicMeshes->LoadCylinderMesh();  // mic arm
	m_basicMeshes->LoadSphereMesh();    // mic head and squirrel


	DefineObjectMaterials();

	// Laptop screen texture
	// Load wizard texture
	CreateGLTexture("textures/wizard_1.png", "wizard_1");

	//Load keyboard texture
	CreateGLTexture("textures/keyboard_2.jpg", "keyboard");

	// Load mic texture
	CreateGLTexture("textures/Mic4.jpg", "Mic");

	// Load plastic texture for table
	CreateGLTexture("textures/Table_Texture2.jpg", "Table");

	

	

	// Bind the textures so OpenGL knows about them
	BindGLTextures();
	SetupSceneLights();
}

void SceneManager::RenderScene()
{
	

	// ===== Toggle full scene =====
	bool bRenderFullScene = true;

	// ===== Enable lighting =====
	m_pShaderManager->setIntValue("bUseLighting", true);

	glClearColor(0.82f, 0.71f, 0.55f, 1.0f); // light brown / tan
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	



	// ===== Materials (applied globally for simplicity) =====
	m_pShaderManager->setVec3Value("material.diffuseColor", glm::vec3(0.8f, 0.8f, 0.8f));
	m_pShaderManager->setVec3Value("material.specularColor", glm::vec3(1.0f, 1.0f, 1.0f));
	m_pShaderManager->setFloatValue("material.shininess", 64.0f);



	// ----------- Table Top (White Plastic Box) -----------
	glm::vec3 tableTopScale = glm::vec3(16.0f, 0.3f, 8.0f); // width, thickness, depth
	glm::vec3 tableTopPos = glm::vec3(0.0f, -0.1f, -0.6f);
	SetTransformations(tableTopScale, 0.0f, 0.0f, 0.0f, tableTopPos);
	SetShaderTexture("Table"); // <-- apply plastic texture
	m_basicMeshes->DrawBoxMesh();

	// Rounded corners using cylinders
	float cornerRadius = 0.32f; // match thickness
	float cornerHeight = tableTopScale.y; // thickness of table top

	// Lower cylinders slightly so tops don't stick out
	float cornerY = tableTopPos.y - tableTopScale.y / 2 + cornerHeight / 2 - 0.02f;

	// Reduce inset to move cylinders outward
	float inset = 0.06f; // smaller than before

	glm::vec3 cornerOffsets[4] = {
		glm::vec3(tableTopScale.x / 2 - cornerRadius / 2 - inset, cornerY,  tableTopScale.z / 2 - cornerRadius / 2 - inset),
		glm::vec3(-tableTopScale.x / 2 + cornerRadius / 2 + inset, cornerY,  tableTopScale.z / 2 - cornerRadius / 2 - inset),
		glm::vec3(tableTopScale.x / 2 - cornerRadius / 2 - inset, cornerY, -tableTopScale.z / 2 + cornerRadius / 2 + inset),
		glm::vec3(-tableTopScale.x / 2 + cornerRadius / 2 + inset, cornerY, -tableTopScale.z / 2 + cornerRadius / 2 + inset)
	};

	for (int i = 0; i < 4; i++)
	{
		glm::vec3 cornerPos = tableTopPos + cornerOffsets[i];
		glm::vec3 cornerScale = glm::vec3(cornerRadius, cornerHeight, cornerRadius); // height = table top thickness
		SetTransformations(cornerScale, 0.0f, 0.0f, 0.0f, cornerPos);
		SetShaderTexture("Table"); // <-- apply plastic texture
		m_basicMeshes->DrawCylinderMesh();
	}


	// ----------- Table Legs (Grey Metal) -----------
	glm::vec3 legScale = glm::vec3(0.3f, 4.0f, 0.3f); // thickness and height

	// Leg height placement: top flush with bottom of table top
	float legTopY = tableTopPos.y - tableTopScale.y / 2;
	float legCenterY = legTopY - legScale.y / 2;

	glm::vec3 legPositions[4];
	for (int i = 0; i < 4; i++)
	{
		// Use the same X/Z offsets as the corner cylinders
		legPositions[i] = glm::vec3(
			cornerOffsets[i].x,
			legCenterY,
			cornerOffsets[i].z
		) + glm::vec3(tableTopPos.x, 0.0f, tableTopPos.z); // account for table position
	}

	// Draw legs
	for (int i = 0; i < 4; i++)
	{
		SetTransformations(legScale, 0.0f, 0.0f, 0.0f, legPositions[i]);
		SetShaderColor(0.5f, 0.5f, 0.5f, 1.0f);
		m_basicMeshes->DrawBoxMesh();
	}



	if (bRenderFullScene)
	{
		// ----------- Laptop Base -----------
		SetTransformations(glm::vec3(4.0f, 0.3f, 2.5f), 0.0f, 0.0f, 0.0f, glm::vec3(0.0f, 0.15f, 0.0f));
		SetShaderColor(0.0f, 0.0f, 0.0f, 1.0f);
		SetShaderMaterial("laptop");
		m_basicMeshes->DrawBoxMesh();

		// ----------- Laptop Keyboard -----------
		SetTransformations(glm::vec3(3.5f, 0.05f, 1.8f), 0.0f, 0.0f, 0.0f, glm::vec3(0.0f, 0.325f, 0.0f));
		SetShaderMaterial("laptop");
		SetShaderTexture("keyboard");
		m_basicMeshes->DrawBoxMesh();

		// ----------- Laptop Screen -----------
		SetTransformations(glm::vec3(4.0f, 3.0f, 0.15f), -20.0f, 0.0f, 0.0f, glm::vec3(0.0f, 1.65f, -1.6f));
		// Before drawing the screen
		m_pShaderManager->setBoolValue("bUseLighting", false);
		SetShaderTexture("wizard_1");
		m_basicMeshes->DrawBoxMesh();

		// Re-enable lighting for the rest
		m_pShaderManager->setBoolValue("bUseLighting", true);

		// ----------- Laptop Screen Frame -----------
		SetTransformations(glm::vec3(4.1f, 3.1f, 0.2f), -20.0f, 0.0f, 0.0f, glm::vec3(0.0f, 1.65f, -1.65f));
		SetShaderColor(0.0f, 0.0f, 0.0f, 1.0f);
		m_basicMeshes->DrawBoxMesh();

		// ----------- Gaming Mouse Body  -----------
		glm::vec3 bodyScale = glm::vec3(0.35f, 0.18f, 0.6f); // slightly shorter oval
		glm::vec3 bodyPos = glm::vec3(3.3f, 0.2f, 0.0f);
		
		SetTransformations(bodyScale, 0.0f, 0.0f, 0.0f, bodyPos);
		SetShaderColor(0.88f, 0.88f, 0.88f, 1.0f);
		SetShaderMaterial("mouse");
		m_basicMeshes->DrawSphereMesh();

		
		// ----------- Scroll Wheel -----------
		glm::vec3 wheelScale = glm::vec3(0.08f, 0.04f, 0.04f); // wider along X-axis, thin along Y and Z
		glm::vec3 wheelPos = bodyPos + glm::vec3(0.0f, 0.15f, -0.30f); // forward along Z, on top of mouse
		SetTransformations(wheelScale, 90.0f, 90.0f, 0.0f, wheelPos); // rotate to horizontal
		SetShaderColor(0.8f, 0.8f, 0.8f, 1.0f); // light grey m_basicMeshes->DrawCylinderMesh();
		m_basicMeshes->DrawCylinderMesh();


		// =================== Microphone ===================

// ----------- Microphone Base -----------
		glm::vec3 micBaseScale = glm::vec3(0.3f, 0.15f, 0.3f);
		glm::vec3 micBasePos = glm::vec3(-3.5f, 0.075f, -3.0f);
		SetTransformations(micBaseScale, 0.0f, 0.0f, 0.0f, micBasePos);
		SetShaderColor(0.0f, 0.0f, 0.0f, 1.0f);
		m_basicMeshes->DrawCylinderMesh();

		// ----------- Base Joint Sphere -----------
		glm::vec3 baseJointScale = glm::vec3(0.15f);
		SetTransformations(baseJointScale, 0.0f, 0.0f, 0.0f, micBasePos);
		SetShaderColor(0.0f, 0.0f, 0.0f, 1.0f);
		m_basicMeshes->DrawSphereMesh();

		// ----------- First Arm Segment -----------
		glm::vec3 arm1Scale = glm::vec3(0.12f, 3.0f, 0.12f);
		SetTransformations(arm1Scale, -30.0f, 180.0f, 0.0f, micBasePos);
		SetShaderColor(0.0f, 0.0f, 0.0f, 1.0f);
		m_basicMeshes->DrawCylinderMesh();

		// ----------- Mid Pivot Sphere -----------
		glm::vec3 pivotPos = micBasePos + glm::vec3(0.0f, 2.5f, 1.5f);
		glm::vec3 midJointScale = glm::vec3(0.2f);
		SetTransformations(midJointScale, 0.0f, 0.0f, 0.0f, pivotPos);
		SetShaderColor(0.0f, 0.0f, 0.0f, 1.0f);
		m_basicMeshes->DrawSphereMesh();

		// ----------- Second Arm Segment -----------
		glm::vec3 arm2Scale = glm::vec3(0.12f, 2.5f, 0.12f);
		SetTransformations(arm2Scale, 110.0f, 0.0f, 0.0f, pivotPos);
		SetShaderColor(0.0f, 0.0f, 0.0f, 1.0f);
		m_basicMeshes->DrawCylinderMesh();

		// ----------- Mic Frame / Holder (Red Cylinder) -----------
		glm::vec3 micFrameScale = glm::vec3(0.3f, 0.7f, 0.3f);
		glm::vec3 micFramePos = pivotPos + glm::vec3(0.0f, -1.0f, 2.5f);
		SetTransformations(micFrameScale, 0.0f, 0.0f, 0.0f, micFramePos);
		SetShaderTexture("Mic");
		//SetShaderColor(1.0f, 0.0f, 0.0f, 1.0f); // red base color
		m_basicMeshes->DrawCylinderMesh();

		// ----------- Mic Head (Cylinder, Red) -----------
		glm::vec3 micBodyScale = glm::vec3(0.15f, 0.5f, 0.15f);
		SetTransformations(micBodyScale, 0.0f, 0.0f, 0.0f, micFramePos);

		SetShaderColor(1.0f, 0.0f, 0.0f, 1.0f); // red base color
		m_basicMeshes->DrawCylinderMesh();

		

		// ----------- Top Cylinder Casing (Black) -----------
		glm::vec3 topCasingScale = glm::vec3(0.35f, 0.2f, 0.35f); // taller cylinder
		glm::vec3 topCasingPos = micFramePos + glm::vec3(0.0f, 0.60f, 0.0f); // sits on top
		SetTransformations(topCasingScale, 0.0f, 0.0f, 0.0f, topCasingPos);
		SetShaderColor(0.0f, 0.0f, 0.0f, 1.0f);
		m_basicMeshes->DrawCylinderMesh();

		// ----------- Bottom Cylinder Casing (Black) -----------
		glm::vec3 bottomCasingScale = glm::vec3(0.35f, 0.5f, 0.35f); // taller cylinder
		glm::vec3 bottomCasingPos = micFramePos + glm::vec3(0.0f, -0.35f, 0.0f); // sits under
		SetTransformations(bottomCasingScale, 0.0f, 0.0f, 0.0f, bottomCasingPos);
		SetShaderColor(0.0f, 0.0f, 0.0f, 1.0f);
		m_basicMeshes->DrawCylinderMesh();

		


		

// Close if-block
}

	// Close RenderScene()
}