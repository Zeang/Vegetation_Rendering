#ifndef LOD_H
#define LOD_H

#include "Model.h"
#include <assert.h>

struct LOD_model
{
	Model *model;
	glm::mat4 transform;
};

class lod
{
private:
	int level;
	int item_num;
	std::vector<Model*> lod_item;
	std::vector<GLfloat> distance_item;

	int Find_Min_Insertable_Level() const;
public:
	LOD_model* lod_model;

	lod(int max_level, int num);
	~lod();
	void Push_LOD_Item(int level, GLfloat distance, Model* m);
	void Pop_LOD_Item(int level);
	Model* Top_LOD_Item();
	Model* Bottom_LOD_Item();
	void Reset_Level(int level);
	Model* operator[](int index) const { return lod_item[index]; }
	int size() const { return item_num; }
	void Find_Item_3_Lod(const glm::vec3 c_pos, const glm::mat4* m, const std::vector<glm::vec3> pos, std::vector<glm::mat4> &lod0, std::vector<glm::mat4> &lod1, std::vector<glm::mat4>&lod2);
};
#endif // !LOD_H
