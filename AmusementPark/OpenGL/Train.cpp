#include "Train.h"

Train::Train(Track* track) :m_trainU(0), m_carCount(2)
{
	trainModel = new Model("./Asset/Model/train v3.obj");
	trainHeadModel = new Model("./Asset/Model/trainHead v3.obj");

	m_track = track;
}

void Train::Draw(Shader* shader)
{
	DrawTrainHead(m_trainU, shader);

	for (int i = 1; i < m_carCount; i++)
		DrawTrainTruck(m_trainU + m_track->AddArcLen(m_trainU, -16.f + (-14.f) * (i - 1)), shader);
		//DrawTrainTruck(m_trainU + (-0.2f) * i, shader);
}

void Train::AddTrainU(const float& num)
{
	m_trainU += m_track->AddArcLen(m_trainU, num);
	float nct = m_track->points.size();
	if (m_trainU >= nct) m_trainU -= nct;
	if (m_trainU < 0) m_trainU += nct;
}

void Train::SetCarCount(const int& num)
{
	if (num < 1)
		m_carCount = 1;
	else
		m_carCount = num;
}

int Train::GetCarCount()
{
	return m_carCount;
}

void Train::DrawTrainHead(float trainU, Shader* shader)
{
	//train head
	{
		float nct = m_track->points.size();
		if (trainU >= nct) trainU -= nct;
		if (trainU < 0) trainU += nct;
		int i = trainU;
		float t = trainU - i;

		glm::vec3 g[4];
		for (int n = 0; n < 4; n++)
			g[n] = m_track->points[(i + n) % m_track->points.size()].pos;
		glm::vec3 trainPos0 = Track::cubicSpline(g, t);
		glm::vec3 trainPos1 = Track::cubicSpline(g, t + (1.0f / TRACK_DIVIDE_LINE));

		for (int n = 0; n < 4; n++)
			g[n] = m_track->points[(i + n) % m_track->points.size()].orient;
		glm::vec3 trainOrient = Track::cubicSpline(g, t);
		trainOrient = glm::normalize(trainOrient);

		glm::vec3 trainHead = (trainPos1 - trainPos0);
		trainHead = glm::normalize(trainHead);

		glm::vec3 trainCross = glm::cross(trainHead, trainOrient);
		trainCross = glm::normalize(trainCross);

		trainOrient = glm::cross(trainHead, trainCross) * -1.0f;
		trainOrient = glm::normalize(trainOrient);

		glm::vec3 u = trainHead;
		glm::vec3 w = trainCross;
		glm::vec3 v = trainOrient;

		trainPos0 *= 0.1f;

		float rotation[16] = {
		u.x, u.y, u.z, 0.0,
		v.x, v.y, v.z, 0.0,
		w.x, w.y, w.z, 0.0,
		trainPos0.x, trainPos0.y,trainPos0.z, 1.0
		};

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 rotate = glm::make_mat4(rotation);
		model = model * rotate;

		//head
		shader->Use();

		//glUniform1f(glGetUniformLocation(shader->ID, "scale"), 0.6f);
		model = glm::scale(model, glm::vec3(0.06f));
		model = glm::rotate(model, glm::radians(180.f), glm::vec3(0, 1, 0));

		shader->setFloat3("u_color", glm::vec3(0.6f, 0.6f, 0.6f));
		shader->setMat4("u_model", model);

		trainHeadModel->Draw(*shader);
	}
}

void Train::DrawTrainTruck(float trainU, Shader* shader)
{
	//train 
	{
		float nct = m_track->points.size();
		if (trainU >= nct) trainU -= nct;
		if (trainU < 0) trainU += nct;
		int i = trainU;
		float t = trainU - i;

		glm::vec3 g[4];
		for (int n = 0; n < 4; n++)
			g[n] = m_track->points[(i + n) % m_track->points.size()].pos;
		glm::vec3 trainPos0 = Track::cubicSpline(g, t);
		glm::vec3 trainPos1 = Track::cubicSpline(g, t + (1.0f / TRACK_DIVIDE_LINE));

		for (int n = 0; n < 4; n++)
			g[n] = m_track->points[(i + n) % m_track->points.size()].orient;
		glm::vec3 trainOrient = Track::cubicSpline(g, t);
		trainOrient = glm::normalize(trainOrient);

		glm::vec3 trainHead = (trainPos1 - trainPos0);
		trainHead = glm::normalize(trainHead);

		glm::vec3 trainCross = glm::cross(trainHead, trainOrient);
		trainCross = glm::normalize(trainCross);

		trainOrient = glm::cross(trainHead, trainCross) * -1.0f;
		trainOrient = glm::normalize(trainOrient);

		glm::vec3 u = trainHead;
		glm::vec3 w = trainCross;
		glm::vec3 v = trainOrient;

		trainPos0 *= 0.1f;

		float rotation[16] = {
		u.x, u.y, u.z, 0.0,
		v.x, v.y, v.z, 0.0,
		w.x, w.y, w.z, 0.0,
		trainPos0.x, trainPos0.y,trainPos0.z, 1.0
		};

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 rotate = glm::make_mat4(rotation);
		model = model * rotate;

		//head
		shader->Use();

		//glUniform1f(glGetUniformLocation(shader->ID, "scale"), 0.6f);
		model = glm::scale(model, glm::vec3(0.06f));
		model = glm::rotate(model, glm::radians(180.f), glm::vec3(0, 1, 0));

		shader->setFloat3("u_color", glm::vec3(0.6f, 0.6f, 0.6f));
		shader->setMat4("u_model", model);

		trainModel->Draw(*shader);
	}
}
