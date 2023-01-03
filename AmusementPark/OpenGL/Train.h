#pragma once
#include "GameHeader.h"
#include "BufferObject.h"
#include "Shader.h"
#include "Model.h"
#include "Track.h"

class Train
{
public:
	Train(Track* track);

	void Draw(Shader* shader);
	void AddTrainU(const float& num);
	void SetCarCount(const int& num);
	int GetCarCount();
public:

	Model* trainModel = nullptr;
	Model* trainHeadModel = nullptr;
private:
	void DrawTrainHead(float trainU, Shader* shader);
	void DrawTrainTruck(float trainU, Shader* shader);

	int m_carCount;
	float m_trainU;
	Track* m_track = nullptr;
};

