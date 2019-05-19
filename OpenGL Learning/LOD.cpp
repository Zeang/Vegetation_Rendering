#include "LOD.h"
int lod::Find_Min_Insertable_Level() const
{
	for (int i = 0; i < lod_item.size(); i++)
	{
		if (lod_item[i] == NULL)
			return i;
	}
	return -1;
}

lod::lod(int max_level, int num) :level(max_level),item_num(num)
{ 
	lod_item.resize(level);
	distance_item.resize(level);
	for (int i = 0; i < level; i++)
	{
		lod_item[i] = NULL;
		distance_item[i] = 99999.0f;
	}
	lod_model = new LOD_model[item_num];
}

lod::~lod()
{
	delete[] lod_model;
}

void lod::Push_LOD_Item(int level, GLfloat distance, Model * m)
{
	try
	{
		if (level >= this->level)
			throw std::length_error("ERROR::The model level is greater than max level");
		else if (m == NULL)
			throw std::exception("ERROR::Input model is null pointer");
		else if (distance < 0.0f)
			throw std::exception("ERROR::Input distance is less than 0");
		else if(level != -1)
			lod_item[level] = m;
		else
		{
			int index = Find_Min_Insertable_Level();
			if (index == -1)
				throw std::length_error("ERROR::No space to insert");
			else
				lod_item[index] = m;
		}
	}
	catch(std::length_error &le)
	{
		std::cout << le.what() << std::endl;
		return;
	}
	catch (std::exception &ex)
	{
		std::cout << ex.what() << std::endl;
		return;
	}
	distance_item[level] = distance;
}

void lod::Pop_LOD_Item(int level)
{
	try
	{
		if (level >= this->level)
			throw std::length_error("ERROR::The model level is greater than max level");
		else
			lod_item[level] = NULL;
	}
	catch (std::length_error le)
	{
		std::cout << le.what() << std::endl;
	}
}

Model * lod::Top_LOD_Item()
{
	for (std::vector<Model*>::reverse_iterator rit = lod_item.rbegin(); rit != lod_item.rend(); rit++)
	{
		if (*rit != NULL)
			return *rit;
	}
	std::cout << "ERROR::No item in LOD" << std::endl;
	return NULL;
}

Model * lod::Bottom_LOD_Item()
{
	for (std::vector<Model*>::iterator it = lod_item.begin(); it != lod_item.end(); it++)
	{
		if (*it != NULL)
			return *it;
	}
	std::cout << "ERROR::No item in LOD" << std::endl;
	return NULL;
}

void lod::Reset_Level(int level)
{
	if (level >= this->level)
	{
		this->level = level;
		lod_item.reserve(level);
	}
	else
	{
		this->level = level;
		lod_item.resize(level);
		lod_item.reserve(level);
	}
}

void lod::Find_Item_3_Lod(const glm::vec3 c_pos, const glm::mat4* m, const std::vector<glm::vec3> pos, std::vector<glm::mat4>& lod0, std::vector<glm::mat4>& lod1, std::vector<glm::mat4>& lod2)
{
	for (int i = 0; i < item_num; i++)
	{
		float dis = glm::distance(pos[i], c_pos);
		if (dis > distance_item[2])
			lod2.push_back(m[i]);
		else if (dis > distance_item[1])
			lod1.push_back(m[i]);
		else
			lod0.push_back(m[i]);
	}
}
