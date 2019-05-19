#pragma once
#include "Header.h"

/* 一个矩形区域的象限划分：:
UL(1)   |    UR(0)
----------|-----------
LL(2)   |    LR(3)
以下对该象限类型的枚举
*/

typedef enum
{
	UR = 0,
	UL = 1,
	LL = 2,
	LR = 3
}QuadrantEnum;


class MapRect
{
public:
	MapRect(int TREE_NUMBER)
	{

		this->minX = -21 * sqrt(TREE_NUMBER);
		this->maxX = 21 * sqrt(TREE_NUMBER);
		this->minY = -21 * sqrt(TREE_NUMBER);
		this->maxY = 21 * sqrt(TREE_NUMBER);
	}
	MapRect() {

	}
public:
	float maxX;
	float maxY;
	float minX;
	float minY;
};

typedef struct SHPMBRInfo
{
	glm::mat4 model_matrix;
	glm::vec2 pos;
}SHPMBRInfo;

//四叉树节点类型结构
class QuadNode
{
public:
	QuadNode();
	MapRect Box;			//包围的矩形区域
	std::vector<SHPMBRInfo> objInfo;	//空间对象数组
	int nChildCount;		//子节点个数
	QuadNode * children[4];	//指向节点的四个孩子
	
	int Lod_level;


};
typedef QuadNode *QuadNode_ptr;


//初始化四叉树节点
QuadNode *InitQuadNode();

//位置在Box块内
bool is_inBox(glm::vec2 pos, MapRect Box);

//划分node成四个子树
void divide_quad(glm::vec2 pos, QuadNode* node);


//查找
QuadNode* search(glm::vec2 pos, QuadNode *node, int level_number);


class QuadTree
{
public:
	QuadTree(int depth, glm::vec2 Camera_pos, MapRect Box, int Tree_Num);
	QuadTree();
	void UpdateQuadTree(glm::vec2 pos);

public:
	QuadNode *root;
	int depth;
	std::vector<QuadNode_ptr*> index_node;

};

void traverse_level(int depth, int* lod_index, QuadTree* p_Quadtree, std::vector<glm::mat4>* lod);