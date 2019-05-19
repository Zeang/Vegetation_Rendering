#include <glad.h>
#include <glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "shader.h"
#include "Model.h"
#include "CascadedShadowMap.h"
#include "gBuffer.h"
#include "SSAO.h"
#include "SkyBox.h"
#include "Instance.h"
#include "mywindow.h"
#include "QuadTree.h"

#include <iostream>
#include <vector>

#include <fstream>
const GLuint SCR_WIDTH = 1600;
const GLuint SCR_HEIGHT = 900;

// Function prototypes

void renderQuad();
void Do_Movement();
unsigned int loadTexture(char const * path, bool gammaCorrection);
unsigned int loadCubemap(std::vector<std::string> faces);

// Camera
Camera camera(glm::vec3(10.0f, 30.0f, 20.0f));

// Delta
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f; 

// Radius
float radius = 5.0f;
float IfLeave = 1.0f;

const int SHADOW_NUM = 3;
const int DEPTH = 4;
const int TREE_NUMBER = 1000;

bool Update_Quadtree = false;
// The MAIN function, from here we start our application and run our Game loop
int main()
{
	Window mywindow("VegetationRendering", SCR_WIDTH, SCR_HEIGHT);
	
	glm::vec2 pos_camera(camera.Position.x, camera.Position.z);
	MapRect Box(TREE_NUMBER);
	QuadTree p_Quadtree(DEPTH, pos_camera, Box, TREE_NUMBER);
	

	/*glm::vec2 pos_debug(50.0f, 20.0f);
	p_Quadtree.UpdateQuadTree(pos_debug);*/
	// Setup and compile our shaders
	Shader TreeShader("vertex/test_deffer.vs", "fragment/test_deffer.fs");
	Shader LeafShader("vertex/leaves.vs", "fragment/leaves.fs");
	Shader DefferShader("vertex/8.g_buffer.vs", "fragment/8.g_buffer.fs");
	Shader ShadowShader("vertex/shadow_mapping_depth.vs", "fragment/shadow_mapping_depth.fs");
	Shader SkyShader("vertex/6.1.skybox.vs", "fragment/6.1.skybox.fs");
	Shader debugDepthQuad("vertex/shadow_mapping_debug.vs", "fragment/shadow_mapping_debug.fs");
	Shader SSAOShader("vertex/9.ssao.vs", "fragment/9.ssao.fs");
	Shader ssaoBlurShader("vertex/9.ssao.vs", "fragment/ssao_blur.fs");

	const float side_length = 21 * sqrt(TREE_NUMBER);
	GLfloat planeVertices[] = {
		// Positions          // Normals         // Texture Coords
		side_length, 0.5f, side_length, 0.0f, 1.0f, 0.0f, side_length, 0.0f,
		-side_length, 0.5f, -side_length, 0.0f, 1.0f, 0.0f, 0.0f, side_length,
		-side_length, 0.5f, side_length, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

		side_length, 0.5f, side_length, 0.0f, 1.0f, 0.0f, side_length, 0.0f,
		side_length, 0.5f, -side_length, 0.0f, 1.0f, 0.0f, side_length, side_length,
		-side_length, 0.5f, -side_length, 0.0f, 1.0f, 0.0f, 0.0f, side_length
	};
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	// Setup plane VAO
	GLuint planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glBindVertexArray(0);

	// Setup skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// Load textures
	unsigned int grassTexture = loadTexture("Grass.jpg",false);

	std::vector<std::string> faces
	{
		"skybox/right.png",
		"skybox/left.png",
		"skybox/top.png",
		"skybox/bottom.png",
		"skybox/front.png",
		"skybox/back.png"
	};
	unsigned int skyboxTexture = loadCubemap(faces);
	SkyBox sky;

	// Load models
	std::vector<Model> model_vec;
	Model treeModel1_0("ExportedObj/tree1/lod0/1_lod0.obj");
	Model treeModel1_1("ExportedObj/tree1/lod1/1_lod1.obj");
	Model treeModel1_2("ExportedObj/tree1/lod2/1_lod2.obj");
	Model treeModel1_3("ExportedObj/tree1/lod2/1_lod2.obj");
	Model treeModel1_4("ExportedObj/tree1/lod2/1_lod2.obj");
	model_vec.push_back(treeModel1_0);
	model_vec.push_back(treeModel1_1);
	model_vec.push_back(treeModel1_2);
	model_vec.push_back(treeModel1_3);
	model_vec.push_back(treeModel1_4);


	unsigned int buffer_lod[DEPTH];
	for (int i = 0; i < DEPTH; i++)
	{
		glGenBuffers(1, &buffer_lod[i]);
	}
	
	// gBuffer object
	gBuffer gb;
	gb.Init(SCR_WIDTH, SCR_HEIGHT);
	SSAO sao;
	sao.Init(SCR_WIDTH, SCR_HEIGHT);

	const GLuint SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// - Create depth texture
	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// CSM object
	// CSM csm(SCR_HEIGHT, SCR_WIDTH);
	// csm.Init(SCR_WIDTH, SCR_HEIGHT);
	// shader configuration
	TreeShader.use();        
	TreeShader.setInt("diffuseTexture", 0);
	TreeShader.setInt("shadowMap", 1);
	TreeShader.setInt("gPosition", 2);
	TreeShader.setInt("gNormal", 3);
	TreeShader.setInt("gColorSpec", 4);
	//TreeShader.setInt("ssao", 5);
	//debugDepthQuad.setInt("depthMap", 0);
	SkyShader.use();
	SkyShader.setInt("skybox", 0);
	SSAOShader.use();
	SSAOShader.setInt("gPosition", 0);
	SSAOShader.setInt("gNormal", 1);
	SSAOShader.setInt("texNoise", 2);
	ssaoBlurShader.use();
	ssaoBlurShader.setInt("ssaoInput", 0);
	LeafShader.use();
	LeafShader.setFloat("WindSpeed", 0.8f);
	LeafShader.setFloat("WindIntensity", 0.25f);
	LeafShader.setFloat("WindWeight", 0.5f);
	// lighting info
	glm::vec3 lightPos(10.0f, 300.0f, 20.0f);
	
	//The Matrix of the lod
	//注意一万颗树的时候栈区大小可能报错，可以分lod为多个
	glm::mat4 lod[DEPTH][TREE_NUMBER];
	//0 对应lod DEPTH-1
	int lod_index[DEPTH] = { 0 };

	//层级遍历两个队列
	std::vector<QuadNode_ptr> temp_vec[2];

	/*GLfloat time1 = glfwGetTime();
	tree_lod.Find_Item_3_Lod(camera.Position, modelMatrices, pos, lod0, lod1, lod2);
	GLfloat time2 = glfwGetTime();
	std::cout << "The FPS is: " << 1.0 / (time2 - time1) << std::endl;*/
	for (int i = 0; i < DEPTH; i++)
	{
		lod_index[i] = 0;
	}
	temp_vec[0].resize(0);
	for (int i = 0; i < 4; i++)
	{
		temp_vec[0].push_back(p_Quadtree.root->children[i]);
	}

	//层级遍历QuadTree
	for (int i = 0; i < DEPTH; i++)
	{
		//栈区的对于已经存储的Matrix索引
		int inverse_index = DEPTH - i - 1;
		/*lod_index[inverse_index] = 0;*/
		int index = i % 2;
		int index_save = (i + 1) % 2;
		temp_vec[index_save].resize(0);
		for (int j = 0; j < temp_vec[index].size(); j++)
		{
			//如果已经是子节点
			if (temp_vec[index][j]->nChildCount == 0)
			{
				for (int k = 0; k < temp_vec[index][j]->objInfo.size(); k++)
				{
					//存储每个Matrix
					lod[inverse_index][lod_index[inverse_index]] = temp_vec[index][j]->objInfo[k].model_matrix;
					lod_index[inverse_index]++;
				}
			}
			//否则把孩子放入下一vector
			else
			{
				for (int k = 0; k < temp_vec[index][j]->nChildCount; k++)
				{
					temp_vec[index_save].push_back(temp_vec[index][j]->children[k]);
				}
			}
		}

	}
	// Game loop
	while (!glfwWindowShouldClose(mywindow.window))
	{
		
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		unsigned int FPS = 1.0 / deltaTime;
		static float Time = 0;
		Time += deltaTime;
		if (Time > 1)
		{
			std::cout << FPS << std::endl;
			Time = 0;
		}

	/*	lightPos.x = sin(currentFrame / 4.0f)*radius;
		lightPos.z = cos(currentFrame / 4.0f)*radius;*/

		// Check and call events
		if (Update_Quadtree)
		{
			Update_Quadtree = false;
			glm::vec2 pos_camera(camera.Position.x, camera.Position.z);
			p_Quadtree.UpdateQuadTree(pos_camera);
			for (int i = 0; i < DEPTH; i++)
			{
				lod_index[i] = 0;
			}
			temp_vec[0].resize(0);
			for (int i = 0; i < 4; i++)
			{
				temp_vec[0].push_back(p_Quadtree.root->children[i]);
			}

			//层级遍历QuadTree
			for (int i = 0; i < DEPTH; i++)
			{
				//栈区的对于已经存储的Matrix索引
				int inverse_index = DEPTH - i - 1;
				/*lod_index[inverse_index] = 0;*/
				int index = i % 2;
				int index_save = (i + 1) % 2;
				temp_vec[index_save].resize(0);
				for (int j = 0; j < temp_vec[index].size(); j++)
				{
					//如果已经是子节点
					if (temp_vec[index][j]->nChildCount == 0)
					{
						for (int k = 0; k < temp_vec[index][j]->objInfo.size(); k++)
						{
							//存储每个Matrix
							lod[inverse_index][lod_index[inverse_index]] = temp_vec[index][j]->objInfo[k].model_matrix;
							lod_index[inverse_index]++;
						}
					}
					//否则把孩子放入下一vector
					else
					{
						for (int k = 0; k < temp_vec[index][j]->nChildCount; k++)
						{
							temp_vec[index_save].push_back(temp_vec[index][j]->children[k]);
						}
					}
				}

			}
		}
		
		glfwPollEvents();
		Do_Movement();

		for (int j = 0; j < DEPTH; j++)
		{
			if (lod_index[j])
			{
				glBindBuffer(GL_ARRAY_BUFFER, buffer_lod[j]);
				glBufferData(GL_ARRAY_BUFFER, lod_index[j] * sizeof(glm::mat4), &lod[j], GL_STATIC_DRAW);
				for (unsigned int i = 0; i < model_vec[j].meshes.size(); i++)
				{
					unsigned int VAO = model_vec[j].meshes[i].VAO;
					glBindVertexArray(VAO);
					// set attribute pointers for matrix (4 times vec4)
					glEnableVertexAttribArray(3);
					glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
					glEnableVertexAttribArray(4);
					glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
					glEnableVertexAttribArray(5);
					glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
					glEnableVertexAttribArray(6);
					glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

					glVertexAttribDivisor(3, 1);
					glVertexAttribDivisor(4, 1);
					glVertexAttribDivisor(5, 1);
					glVertexAttribDivisor(6, 1);

					glBindVertexArray(0);
				}

			}
		}


		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1. Geometry pass: render scene's geometry/color data into gbuffer
		glBindFramebuffer(GL_FRAMEBUFFER, gb.GetBuffer(GBUFFER::G_Buffer));
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4();
		DefferShader.use();
		DefferShader.setMat4("projection", projection);
		DefferShader.setMat4("view", view);
		DefferShader.setMat4("model", model);
		DefferShader.setFloat("IfPlane", 1.0f);
		glBindVertexArray(planeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		DefferShader.setFloat("IfPlane", 0.0f);
		if(DEPTH == 5)
			for (unsigned int i = 0; i < model_vec[4].meshes.size(); i++)
			{
				if (i!=0) {
					glBindVertexArray(model_vec[4].meshes[i].VAO);
					DefferShader.setInt("texture_diffuse", 0);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, model_vec[4].meshes[i].textures[0].id);
					glDrawElementsInstanced(GL_TRIANGLES, model_vec[4].meshes[i].indices.size(), GL_UNSIGNED_INT, 0, lod_index[4]);
					//std::cout << lod_index[2] << std::endl;
				}
			}
		for (unsigned int i = 0; i < model_vec[3].meshes.size(); i++)
		{
			glBindVertexArray(model_vec[3].meshes[i].VAO);
			DefferShader.setInt("texture_diffuse", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, model_vec[3].meshes[i].textures[0].id);
			glDrawElementsInstanced(GL_TRIANGLES, model_vec[3].meshes[i].indices.size(), GL_UNSIGNED_INT, 0, lod_index[3]);
		}

		for (unsigned int i = 0; i < model_vec[2].meshes.size(); i++)
		{
			LeafShader.use();
			LeafShader.setFloat("IfLeave", 1.0f);
			glBindVertexArray(model_vec[2].meshes[i].VAO);
			/*DefferShader.setInt("texture_diffuse", 0);
			glActiveTexture(GL_TEXTURE0);*/
			glBindTexture(GL_TEXTURE_2D, model_vec[2].meshes[i].textures[0].id);
			glDrawElementsInstanced(GL_TRIANGLES, model_vec[2].meshes[i].indices.size(), GL_UNSIGNED_INT, 0, lod_index[2]);
			//std::cout << lod_index[2] << std::endl;
		}
		for (unsigned int i = 0; i < model_vec[1].meshes.size(); i++)
		{
			LeafShader.use();
			LeafShader.setFloat("IfLeave", 1.0f);
			glBindVertexArray(model_vec[1].meshes[i].VAO);
			glBindTexture(GL_TEXTURE_2D, model_vec[1].meshes[i].textures[0].id);
			glDrawElementsInstanced(GL_TRIANGLES, model_vec[1].meshes[i].indices.size(), GL_UNSIGNED_INT, 0, lod_index[1]);
		}


		LeafShader.use();
		LeafShader.setMat4("projection", projection);
		LeafShader.setMat4("view", view);
		LeafShader.setVec3("Time", glm::vec3(1.0f) * currentFrame);
		LeafShader.setVec3("lightPos", lightPos);
		LeafShader.setVec3("cameraPos", camera.Position);
		LeafShader.setFloat("IfLeave", 1.0f);

		glBindVertexArray(model_vec[0].meshes[0].VAO);
		glBindTexture(GL_TEXTURE_2D, model_vec[0].meshes[0].textures[0].id);
		glDrawElementsInstanced(GL_TRIANGLES, model_vec[0].meshes[0].indices.size(), GL_UNSIGNED_INT, 0, lod_index[0]);

		DefferShader.use();
		glBindVertexArray(model_vec[0].meshes[1].VAO);
		glBindTexture(GL_TEXTURE_2D, model_vec[0].meshes[1].textures[0].id);
		glDrawElementsInstanced(GL_TRIANGLES, model_vec[0].meshes[1].indices.size(), GL_UNSIGNED_INT, 0, lod_index[0]);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//// create ssao texture
		//glBindFramebuffer(GL_FRAMEBUFFER, sao.ssaoFBO);
		//glClear(GL_COLOR_BUFFER_BIT);
		//SSAOShader.use();
		//for (unsigned int i = 0; i < 64; i++)
		//	SSAOShader.setVec3("samples[" + std::to_string(i) + "]", sao.ssaoKernel[i]);
		//SSAOShader.setMat4("projection", projection);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, gb.GetBuffer(GBUFFER::G_Position));
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, gb.GetBuffer(GBUFFER::G_Normal));
		//glActiveTexture(GL_TEXTURE2);
		//glBindTexture(GL_TEXTURE_2D, sao.noiseTexture);
		//renderQuad();
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//// Blur SSAO texture to remove noise
		//glBindFramebuffer(GL_FRAMEBUFFER, sao.ssaoBlurFBO);
		//glClear(GL_COLOR_BUFFER_BIT);
		//ssaoBlurShader.use();
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, sao.ssaoColorBuffer);
		//renderQuad();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Clear the colorbuffer
		glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		float near_plane = 0.5f;
		float far_plane = 500.0f;
		glm::mat4 lightProjection = glm::ortho(-500.0f, 500.0f, -500.0f, 500.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f,1.0f,0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		// render scene from light's point of view
		ShadowShader.use();
		ShadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		ShadowShader.setFloat("WindSpeed", 0.8f);
		ShadowShader.setFloat("WindIntensity", 0.25f);
		ShadowShader.setFloat("WindWeight", 0.5f);
		ShadowShader.setVec3("Time", glm::vec3(1.0f) * currentFrame);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		for (unsigned int i = 0; i < model_vec[1].meshes.size(); i++)
		{
			glBindVertexArray(model_vec[1].meshes[i].VAO);
			glBindTexture(GL_TEXTURE_2D, model_vec[1].meshes[i].textures[0].id);
			glDrawElementsInstanced(GL_TRIANGLES, model_vec[1].meshes[i].indices.size(), GL_UNSIGNED_INT, 0, lod_index[1]);
		}
		for (unsigned int i = 0; i < model_vec[0].meshes.size(); i++)
		{
			glBindVertexArray(model_vec[0].meshes[i].VAO);
			glBindTexture(GL_TEXTURE_2D, model_vec[0].meshes[i].textures[0].id);
			glDrawElementsInstanced(GL_TRIANGLES, model_vec[0].meshes[i].indices.size(), GL_UNSIGNED_INT, 0, lod_index[0]);
		}
		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// lighting pass
		// Render scene as normal
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		TreeShader.use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, grassTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gb.GetBuffer(GBUFFER::G_Position));
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, gb.GetBuffer(GBUFFER::G_Normal));
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, gb.GetBuffer(GBUFFER::G_ColorSpec));
		//glActiveTexture(GL_TEXTURE5);
		//glBindTexture(GL_TEXTURE_2D, sao.ssaoColorBufferBlur);
		
		TreeShader.setVec3("viewPos", camera.Position);
		TreeShader.setVec3("lightPos", lightPos);
		TreeShader.setMat4("lightSpcaeMatrix", lightSpaceMatrix);
		renderQuad();
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gb.GetBuffer(GBUFFER::G_Buffer));
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); 
		glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//debugDepthQuad.use();
		//debugDepthQuad.setFloat("near_plane", near_plane);
		//debugDepthQuad.setFloat("near_plane", far_plane);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, depthMap);
		//renderQuad();
		// draw skybox as last
		glDepthFunc(GL_LEQUAL);
		SkyShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		SkyShader.setMat4("view", view);
		SkyShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, sky.GetTexture());
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		// Swap the buffers
		glfwSwapBuffers(mywindow.window);
	}
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &skyboxVBO);
	glDeleteBuffers(1, &planeVBO);

	glfwTerminate();
	return 0;
}

unsigned int loadTexture(char const * path, bool gammaCorrection)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum internalFormat;
		GLenum dataFormat;
		if (nrComponents == 1)
		{
			internalFormat = dataFormat = GL_RED;
		}
		else if (nrComponents == 3)
		{
			internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
			dataFormat = GL_RGB;
		}
		else if (nrComponents == 4)
		{
			internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
			dataFormat = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

bool keys[1024];
bool keysPressed[1024];
// Moves/alters the camera positions based on user input
void Do_Movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);


}

GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	/*if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		static int flag = false;
		if (!flag)
		{
			glfwSetWindowSize(window, 1920, 1080);
			glfwSetWindowPos(window, 0, 0);
			glfwMakeContextCurrent(window);

			glViewport(0, 0, 1920, 1080);
			flag = true;
		}
		else
		{
			glfwSetWindowSize(window, SCR_WIDTH, SCR_HEIGHT);
			glfwSetWindowPos(window, (1920 - SCR_WIDTH) / 2, (1080 - SCR_WIDTH) / 2);
			glfwMakeContextCurrent(window);
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			flag = false;
		}
	}*/
	

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key <= 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
			keysPressed[key] = false;
		}
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}