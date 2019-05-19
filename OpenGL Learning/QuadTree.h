#pragma once
#include "Header.h"

/* һ��������������޻��֣�:
UL(1)   |    UR(0)
----------|-----------
LL(2)   |    LR(3)
���¶Ը��������͵�ö��
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

//�Ĳ����ڵ����ͽṹ
class QuadNode
{
public:
	QuadNode();
	MapRect Box;			//��Χ�ľ�������
	std::vector<SHPMBRInfo> objInfo;	//�ռ��������
	int nChildCount;		//�ӽڵ����
	QuadNode * children[4];	//ָ��ڵ���ĸ�����
	
	int Lod_level;


};
typedef QuadNode *QuadNode_ptr;


//��ʼ���Ĳ����ڵ�
QuadNode *InitQuadNode();

//λ����Box����
bool is_inBox(glm::vec2 pos, MapRect Box);

//����node���ĸ�����
void divide_quad(glm::vec2 pos, QuadNode* node);


//����
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