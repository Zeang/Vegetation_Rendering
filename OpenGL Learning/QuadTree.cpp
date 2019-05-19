#include "QuadTree.h"

QuadNode *InitQuadNode()
{
	QuadNode *node = new QuadNode();
	node->Box.maxX = 0;
	node->Box.maxY = 0;
	node->Box.minX = 0;
	node->Box.minY = 0;

	for (int i = 0; i < 4; i++)
	{
		node->children[i] = NULL;
	}

	node->nChildCount = 0;

	return node;
}

QuadNode::QuadNode()
{
	this->Box.maxX = 0;
	this->Box.maxY = 0;
	this->Box.minX = 0;
	this->Box.minY = 0;

	for (int i = 0; i < 4; i++)
	{
		this->children[i] = NULL;
	}

	this->nChildCount = 0;
	this->Lod_level = 10000;
}

bool is_inBox(glm::vec2 pos, MapRect Box)
{
	if (pos.x >= Box.minX && pos.x < Box.maxX && pos.y >= Box.minY && pos.y < Box.maxY)
	{
		return true;
	}
	else
		return false;
}

//划分node成四个子树
void divide_quad(glm::vec2 pos, QuadNode* node)
{
	if (node->Lod_level <= 0)
		return;

	node->nChildCount = 4;
	for (int i = 0; i < 4; i++)
	{
		//Init children
		node->children[i] = InitQuadNode();

		int flagX = (i % 3 == 0) ? 1 : 0;
		node->children[i]->Box.minX = node->Box.minX + flagX * ((node->Box.minX + node->Box.maxX) / 2 - node->Box.minX);
		node->children[i]->Box.maxX = (node->Box.minX + node->Box.maxX) / 2 + flagX * ((node->Box.minX + node->Box.maxX) / 2 - node->Box.minX);
		int flagY = (i >= 2) ? 0 : 1;
		node->children[i]->Box.minY = node->Box.minY + flagY * ((node->Box.maxY - node->Box.minY) / 2);
		node->children[i]->Box.maxY = (node->Box.minY + node->Box.maxY) / 2 + flagY * ((node->Box.maxY - node->Box.minY) / 2);
		
		//The level of detail
		node->children[i]->Lod_level = node->Lod_level - 1;

		//The object allocation to children
		for (int j = 0; j < node->objInfo.size(); j++)
		{
			if (is_inBox(node->objInfo[j].pos, node->children[i]->Box))
			{
				node->children[i]->objInfo.push_back(node->objInfo[j]);
			}
		}
		
	}

}

QuadNode* search(glm::vec2 pos, QuadNode *node, int level_number)
{
	if (is_inBox(pos, node->Box) )
	{
		if (level_number == 0)
		{
			return node;
		}
		else
		{
			if (node->nChildCount == 0)
			{
				return node;
			}
			else
			{
				int flagX = pos.x >= (node->Box.minX + node->Box.maxX) / 2 ? 1 : 0;
				int flagY = pos.y >= (node->Box.minY + node->Box.maxY) / 2 ? 1 : 0;
				int index = 2 - flagY + flagX * (1 - 2 * flagY);

				return search(pos, node->children[index], level_number - 1);
			}
		}
		
	}
	else
	{
		return NULL;
	}
}

QuadTree::QuadTree(int depth, glm::vec2 Camera_pos, MapRect Box, int Tree_Num)
{
	root = InitQuadNode();
	this->depth = depth;
	root->Box = Box;
	root->Lod_level = depth;
	float offset = 6 * sqrt(Tree_Num);
	float radius = 15 * sqrt(Tree_Num);
	//对象
	for (int i = 0; i < Tree_Num; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);

		float angle = (float)i / (float)Tree_Num * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float pos_scale = (rand() % 100)*1.0 / 100;
		float x = sin(angle) * radius * pos_scale + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.2 / Tree_Num; // keep height of asteroid field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius * pos_scale + displacement;

		
		
		SHPMBRInfo obj;
		obj.pos = glm::vec2(x, z);
		model = glm::translate(model, glm::vec3(x, y, z));
		
		float rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.05f, 1.0f, 0.06f));

		float scale_x = (rand() % 100 + 50) * 1.0 / 100;
		float scale_y = (rand() % 21 + 90) * 1.0 / 100 * scale_x;
		float scale_z = (rand() % 21 + 90) * 1.0 / 100 * scale_x;
		model = glm::scale(model, glm::vec3(scale_x, scale_y, scale_z));
		
		obj.model_matrix = model;
		if (is_inBox(obj.pos, root->Box))
		{
			root->objInfo.push_back(obj);
		}
	}

	float width = (Box.maxX - Box.minX);
	float height = (Box.maxY - Box.minY);


	for (int i = 0; i < depth; i++)
	{
		if (i == 0 && is_inBox(Camera_pos, Box))
		{
			divide_quad(Camera_pos, root);
		}
		else
		{
			QuadNode_ptr* ptr = new QuadNode_ptr[4];
			for (int j = 0; j < 4; j++)
			{

				//Traversing four point about lod[depth-i]
				int Denominator = pow(2, i);
				float tempX = Camera_pos.x - width / (Denominator * 2) + (j % 2) * width / Denominator;
				float tempY = Camera_pos.y - height / (Denominator * 2) + (j / 2) * height / Denominator;
				glm::vec2 pos_new(tempX, tempY);
				QuadNode* node = search(pos_new, root, 1000);
				ptr[j] = node;
				if (node == NULL)
				{
					continue;
				}
				else if (node->Lod_level + i > depth - 1)
				{
					divide_quad(pos_new, node);
				}
			}
			index_node.push_back(ptr);
		}
	}

}

void QuadTree::UpdateQuadTree(glm::vec2 pos)
{
	QuadNode_ptr root_ptr = this->root;
	float width = (root_ptr->Box.maxX - root_ptr->Box.minX);
	float height = (root_ptr->Box.maxY - root_ptr->Box.minY);
	//对每一层的lod
	for (int i = 0; i < this->depth - 1; i++)
	{
		QuadNode_ptr *ptr = new QuadNode_ptr[4];
		for (int j = 0; j < 4; j++)
		{
			int Denominator = pow(2, i + 1);
			float tempX = pos.x - width / (Denominator * 2) + (j % 2) * width / Denominator;
			float tempY = pos.y - height / (Denominator * 2) + (j / 2) * height / Denominator;
			glm::vec2 pos_new = glm::vec2(tempX, tempY);
			QuadNode_ptr node = search(pos_new, this->root, i + 1);
			ptr[j] = node;
			bool flag_exitnode = false;
			for (int k = 0; k < 4; k++)
			{
				if (this->index_node[i][k] == node)
				{
					flag_exitnode = true;
					break;
				}
			}
			if (node == NULL)
				continue;
			else if (flag_exitnode == false)
			{
				divide_quad(pos_new, node);
				std::cout << "Run divide_quad." << std::endl;
			}

		}
		for (int j = 0; j < 4; j++)
		{
			bool flag = false;
			for (int k = 0; k < 4; k++)
			{
				if (ptr[k] == this->index_node[i][j])
				{
					flag = true;
					break;
				}
			}
			//如果存储的索引依旧存在
			if (flag)
			{
				continue;
			}
			else if (this->index_node[i][j] != NULL && this->index_node[i][j]->nChildCount > 0)//如果不存在,delete他的子树
			{
				for (int index = 0; index < 4; index++)
				{
					delete this->index_node[i][j]->children[index];
					std::cout << "Run delete lod. " << std::endl;
				}
				this->index_node[i][j]->nChildCount = 0;
			}

		}
		this->index_node[i] = ptr;
		/*for (int j = 0; j < 4; j++)
		{
			pQuadTree->index_node[i][j] = ptr[j];
		}*/
	}
}

void traverse_level(int depth, int* lod_index, QuadTree* p_Quadtree, std::vector<glm::mat4> lod[4])
{
	//层级遍历两个队列
	std::vector<QuadNode_ptr> temp_vec[2];

	for (int i = 0; i < depth; i++)
	{
		lod_index[i] = 0;
		lod[i].resize(0);
	}
	temp_vec[0].resize(0);
	for (int i = 0; i < 4; i++)
	{
		temp_vec[0].push_back(p_Quadtree->root->children[i]);
	}

	//层级遍历QuadTree
	for (int i = 0; i < depth; i++)
	{
		//栈区的对于已经存储的Matrix索引
		int inverse_index = depth - i - 1;
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
					lod[inverse_index].push_back(temp_vec[index][j]->objInfo[k].model_matrix);
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