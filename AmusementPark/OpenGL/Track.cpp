#include "Track.h"

Track::Track(const char* filename)
{
	ReadPoints(filename);
	Init();
}

//****************************************************************************
//
// * Handy utility to break a string into a list of words
//============================================================================
void breakString(char* str, std::vector<const char*>& words)
//============================================================================
{
	// start with no words
	words.clear();

	// scan through the string, starting at the beginning
	char* p = str;

	// stop when we hit the end of the string
	while (*p) {
		// skip over leading whitespace - stop at the first character or end of string
		while (*p && *p <= ' ') p++;

		// now we're pointing at the first thing after the spaces
		// make sure its not a comment, and that we're not at the end of the string
		// (that's actually the same thing)
		if (!(*p) || *p == '#')
			break;

		// so we're pointing at a word! add it to the word list
		words.push_back(p);

		// now find the end of the word
		while (*p > ' ') p++;	// stop at space or end of string

		// if its ethe end of the string, we're done
		if (!*p) break;

		// otherwise, turn this space into and end of string (to end the word)
		// and keep going
		*p = 0;
		p++;
	}
}

void Track::ReadPoints(const char* filename)
{
	FILE* fp = fopen(filename, "r");
	if (!fp) {
		std::cout<<"TRACK::Can't Open File!\n";
	}
	else {
		char buf[512];

		// first line = number of points
		fgets(buf, 512, fp);
		size_t npts = (size_t)atoi(buf);

		if ((npts < 4) || (npts > 65535)) {
			std::cout << "TRACK::Illegal Number of Points Specified in File\n";
		}
		else {
			points.clear();
			// get lines until EOF or we have enough points
			while ((points.size() < npts) && fgets(buf, 512, fp)) {
				glm::vec3 pos, orient;
				std::vector<const char*> words;
				breakString(buf, words);
				if (words.size() >= 3) {
					pos.x = (float)strtod(words[0], 0);
					pos.y = (float)strtod(words[1], 0);
					pos.z = (float)strtod(words[2], 0);
				}
				else {
					pos.x = 0;
					pos.y = 0;
					pos.z = 0;
				}
				if (words.size() >= 6) {
					orient.x = (float)strtod(words[3], 0);
					orient.y = (float)strtod(words[4], 0);
					orient.z = (float)strtod(words[5], 0);
				}
				else {
					orient.x = 0;
					orient.y = 1;
					orient.z = 0;
				}
				orient = glm::normalize(orient);
				points.push_back(ControlPoint(pos, orient));
			}
		}
		fclose(fp);
	}
}

void Track::Init()
{
	arcLength = std::vector<float>(points.size());
	length = std::vector<std::vector<float>>(points.size(), std::vector<float>(TRACK_DIVIDE_LINE));

	float sleeperSpaceCount = 0.0f;
	std::vector<float> trackVerticesAndNormal;
	std::vector<float> sleeperVerticesAndNormal;

	for (size_t i = 0; i < points.size(); ++i) {
		// pos
		glm::vec3 pos[4];
		for (int n = 0; n < 4; n++)
			pos[n] = points[(i + n) % points.size()].pos;
		// orient
		glm::vec3 orient[4];
		for (int n = 0; n < 4; n++)
			orient[n] = points[(i + n) % points.size()].orient;

		float percent = 1.0f / TRACK_DIVIDE_LINE;
		float t = 0;
		glm::vec3 qt = cubicSpline(pos, t);

		float arcLen = 0;
		glm::vec3 preCrossR[2], preCrossL[2];
		//initialize…
		for (size_t j = 0; j < TRACK_DIVIDE_LINE; j++) {
			glm::vec3 qt0 = qt;
			t += percent;
			qt = cubicSpline(pos, t);
			glm::vec3 qt1 = qt;

			double tempLen = sqrt(pow((qt0.x - qt1.x), 2) + pow((qt0.y - qt1.y), 2) + pow((qt0.z - qt1.z), 2));
			arcLen += tempLen;
			length[i][j] = tempLen;
			sleeperSpaceCount += tempLen;

			// cross
			glm::vec3 orient_t = cubicSpline(orient, t);
			orient_t = glm::normalize(orient_t);
			glm::vec3 cross_t = glm::cross((qt1 - qt0), orient_t);
			cross_t = glm::normalize(cross_t);
			cross_t = cross_t * 2.5f;

			float railWidth = 0.2;
			glm::vec3 cross_t_R = cross_t * (1 + railWidth);
			glm::vec3 cross_t_L = cross_t * (1 - railWidth);

			glm::vec3 crossR0[2] = { qt0 + cross_t_R, qt0 - cross_t_R }, crossL0[2] = { qt0 + cross_t_L, qt0 - cross_t_L };
			glm::vec3 crossR1[2] = { qt1 + cross_t_R, qt1 - cross_t_R }, crossL1[2] = { qt1 + cross_t_L, qt1 - cross_t_L };

			//glNormal3f(0, -1, 0); //我也不知道為甚麼
			for (int n = 0; n < 2; n++)
			{
				//if (!doingShadows)
					//glColor3f(0.5f, 0.3f, 0.1f);
				//v
				trackVerticesAndNormal.push_back(crossR0[n].x); trackVerticesAndNormal.push_back(crossR0[n].y); trackVerticesAndNormal.push_back(crossR0[n].z);
				//n
				trackVerticesAndNormal.push_back(orient_t.x); trackVerticesAndNormal.push_back(orient_t.y); trackVerticesAndNormal.push_back(orient_t.z);
				//v
				trackVerticesAndNormal.push_back(crossR1[n].x); trackVerticesAndNormal.push_back(crossR1[n].y); trackVerticesAndNormal.push_back(crossR1[n].z);
				//n
				trackVerticesAndNormal.push_back(orient_t.x); trackVerticesAndNormal.push_back(orient_t.y); trackVerticesAndNormal.push_back(orient_t.z);
				//v
				trackVerticesAndNormal.push_back(crossL0[n].x); trackVerticesAndNormal.push_back(crossL0[n].y); trackVerticesAndNormal.push_back(crossL0[n].z);
				//n
				trackVerticesAndNormal.push_back(orient_t.x); trackVerticesAndNormal.push_back(orient_t.y); trackVerticesAndNormal.push_back(orient_t.z);

				//v
				trackVerticesAndNormal.push_back(crossL1[n].x); trackVerticesAndNormal.push_back(crossL1[n].y); trackVerticesAndNormal.push_back(crossL1[n].z);
				//n
				trackVerticesAndNormal.push_back(orient_t.x); trackVerticesAndNormal.push_back(orient_t.y); trackVerticesAndNormal.push_back(orient_t.z);
				//v
				trackVerticesAndNormal.push_back(crossR1[n].x); trackVerticesAndNormal.push_back(crossR1[n].y); trackVerticesAndNormal.push_back(crossR1[n].z);
				//n
				trackVerticesAndNormal.push_back(orient_t.x); trackVerticesAndNormal.push_back(orient_t.y); trackVerticesAndNormal.push_back(orient_t.z);
				//v
				trackVerticesAndNormal.push_back(crossL0[n].x); trackVerticesAndNormal.push_back(crossL0[n].y); trackVerticesAndNormal.push_back(crossL0[n].z);
				//n
				trackVerticesAndNormal.push_back(orient_t.x); trackVerticesAndNormal.push_back(orient_t.y); trackVerticesAndNormal.push_back(orient_t.z);

				if (j != 0)
				{
					//補縫隙
					//glBegin(GL_POLYGON);
					//glNormal3f(0, -1, 0);
					//if (!doingShadows)
						//glColor3f(0.5f, 0.3f, 0.1f);

					//v
					trackVerticesAndNormal.push_back(crossR0[n].x); trackVerticesAndNormal.push_back(crossR0[n].y); trackVerticesAndNormal.push_back(crossR0[n].z);
					//n
					trackVerticesAndNormal.push_back(orient_t.x); trackVerticesAndNormal.push_back(orient_t.y); trackVerticesAndNormal.push_back(orient_t.z);
					//v
					trackVerticesAndNormal.push_back(preCrossR[n].x); trackVerticesAndNormal.push_back(preCrossR[n].y); trackVerticesAndNormal.push_back(preCrossR[n].z);
					//n
					trackVerticesAndNormal.push_back(orient_t.x); trackVerticesAndNormal.push_back(orient_t.y); trackVerticesAndNormal.push_back(orient_t.z);
					//v
					trackVerticesAndNormal.push_back(crossL0[n].x); trackVerticesAndNormal.push_back(crossL0[n].y); trackVerticesAndNormal.push_back(crossL0[n].z);
					//n
					trackVerticesAndNormal.push_back(orient_t.x); trackVerticesAndNormal.push_back(orient_t.y); trackVerticesAndNormal.push_back(orient_t.z);

					//v
					trackVerticesAndNormal.push_back(preCrossL[n].x); trackVerticesAndNormal.push_back(preCrossL[n].y); trackVerticesAndNormal.push_back(preCrossL[n].z);
					//n
					trackVerticesAndNormal.push_back(orient_t.x); trackVerticesAndNormal.push_back(orient_t.y); trackVerticesAndNormal.push_back(orient_t.z);
					//v
					trackVerticesAndNormal.push_back(preCrossR[n].x); trackVerticesAndNormal.push_back(preCrossR[n].y); trackVerticesAndNormal.push_back(preCrossR[n].z);
					//n
					trackVerticesAndNormal.push_back(orient_t.x); trackVerticesAndNormal.push_back(orient_t.y); trackVerticesAndNormal.push_back(orient_t.z);
					//v
					trackVerticesAndNormal.push_back(crossL0[n].x); trackVerticesAndNormal.push_back(crossL0[n].y); trackVerticesAndNormal.push_back(crossL0[n].z);
					//n
					trackVerticesAndNormal.push_back(orient_t.x); trackVerticesAndNormal.push_back(orient_t.y); trackVerticesAndNormal.push_back(orient_t.z);
				}
			}

			preCrossR[0] = crossR1[0];
			preCrossR[1] = crossR1[1];
			preCrossL[0] = crossL1[0];
			preCrossL[1] = crossL1[1];


			//sleeper
			if (sleeperSpaceCount >= sleeperSpace)
			{
				float sleeperLen = 8.f, sleeperWidth = 4.f, sleeperHeight = 1.0f;
				cross_t = glm::normalize(cross_t);

				glm::vec3 headOrient = qt1 - qt0;
				headOrient = glm::normalize(headOrient);

				glm::vec3 heightOrient = glm::cross(headOrient, cross_t);
				heightOrient = -glm::normalize(heightOrient);

				glm::vec3 u = headOrient; u = glm::normalize(u);
				glm::vec3 w = cross_t; w = glm::normalize(w);
				glm::vec3 v = heightOrient; v = glm::normalize(v);

				glm::vec3 FRT(qt0 + u * sleeperWidth * 0.5f + w * sleeperLen * 0.5f - v * 0.1f);				
				glm::vec3 BRT(qt0 - u * sleeperWidth * 0.5f + w * sleeperLen * 0.5f - v * 0.1f);
				glm::vec3 FLT(qt0 + u * sleeperWidth * 0.5f - w * sleeperLen * 0.5f - v * 0.1f);
				glm::vec3 BLT(qt0 - u * sleeperWidth * 0.5f - w * sleeperLen * 0.5f - v * 0.1f);
				glm::vec3 FRD(qt0 + u * sleeperWidth * 0.5f + w * sleeperLen * 0.5f - v * sleeperHeight - v * 0.1f);
				glm::vec3 BRD(qt0 - u * sleeperWidth * 0.5f + w * sleeperLen * 0.5f - v * sleeperHeight - v * 0.1f);
				glm::vec3 FLD(qt0 + u * sleeperWidth * 0.5f - w * sleeperLen * 0.5f - v * sleeperHeight - v * 0.1f);
				glm::vec3 BLD(qt0 - u * sleeperWidth * 0.5f - w * sleeperLen * 0.5f - v * sleeperHeight - v * 0.1f);

				//TOP
				//v
				sleeperVerticesAndNormal.push_back(FRT.x); sleeperVerticesAndNormal.push_back(FRT.y); sleeperVerticesAndNormal.push_back(FRT.z);
				//n
				sleeperVerticesAndNormal.push_back(heightOrient.x); sleeperVerticesAndNormal.push_back(heightOrient.y); sleeperVerticesAndNormal.push_back(heightOrient.z);
				//v
				sleeperVerticesAndNormal.push_back(BRT.x); sleeperVerticesAndNormal.push_back(BRT.y); sleeperVerticesAndNormal.push_back(BRT.z);
				//n
				sleeperVerticesAndNormal.push_back(heightOrient.x); sleeperVerticesAndNormal.push_back(heightOrient.y); sleeperVerticesAndNormal.push_back(heightOrient.z);
				//v
				sleeperVerticesAndNormal.push_back(BLT.x); sleeperVerticesAndNormal.push_back(BLT.y); sleeperVerticesAndNormal.push_back(BLT.z);
				//n
				sleeperVerticesAndNormal.push_back(heightOrient.x); sleeperVerticesAndNormal.push_back(heightOrient.y); sleeperVerticesAndNormal.push_back(heightOrient.z);
				
				//v
				sleeperVerticesAndNormal.push_back(FRT.x); sleeperVerticesAndNormal.push_back(FRT.y); sleeperVerticesAndNormal.push_back(FRT.z);
				//n
				sleeperVerticesAndNormal.push_back(heightOrient.x); sleeperVerticesAndNormal.push_back(heightOrient.y); sleeperVerticesAndNormal.push_back(heightOrient.z);
				//v
				sleeperVerticesAndNormal.push_back(FLT.x); sleeperVerticesAndNormal.push_back(FLT.y); sleeperVerticesAndNormal.push_back(FLT.z);
				//n
				sleeperVerticesAndNormal.push_back(heightOrient.x); sleeperVerticesAndNormal.push_back(heightOrient.y); sleeperVerticesAndNormal.push_back(heightOrient.z);
				//v
				sleeperVerticesAndNormal.push_back(BLT.x); sleeperVerticesAndNormal.push_back(BLT.y); sleeperVerticesAndNormal.push_back(BLT.z);
				//n
				sleeperVerticesAndNormal.push_back(heightOrient.x); sleeperVerticesAndNormal.push_back(heightOrient.y); sleeperVerticesAndNormal.push_back(heightOrient.z);

				//DOWN
				//v
				sleeperVerticesAndNormal.push_back(FRD.x); sleeperVerticesAndNormal.push_back(FRD.y); sleeperVerticesAndNormal.push_back(FRD.z);
				//n
				sleeperVerticesAndNormal.push_back(-heightOrient.x); sleeperVerticesAndNormal.push_back(-heightOrient.y); sleeperVerticesAndNormal.push_back(-heightOrient.z);
				//v
				sleeperVerticesAndNormal.push_back(BRD.x); sleeperVerticesAndNormal.push_back(BRD.y); sleeperVerticesAndNormal.push_back(BRD.z);
				//n
				sleeperVerticesAndNormal.push_back(-heightOrient.x); sleeperVerticesAndNormal.push_back(-heightOrient.y); sleeperVerticesAndNormal.push_back(-heightOrient.z);
				//v
				sleeperVerticesAndNormal.push_back(BLD.x); sleeperVerticesAndNormal.push_back(BLD.y); sleeperVerticesAndNormal.push_back(BLD.z);
				//n
				sleeperVerticesAndNormal.push_back(-heightOrient.x); sleeperVerticesAndNormal.push_back(-heightOrient.y); sleeperVerticesAndNormal.push_back(-heightOrient.z);

				//v
				sleeperVerticesAndNormal.push_back(FRD.x); sleeperVerticesAndNormal.push_back(FRD.y); sleeperVerticesAndNormal.push_back(FRD.z);
				//n
				sleeperVerticesAndNormal.push_back(-heightOrient.x); sleeperVerticesAndNormal.push_back(-heightOrient.y); sleeperVerticesAndNormal.push_back(-heightOrient.z);
				//v
				sleeperVerticesAndNormal.push_back(FLD.x); sleeperVerticesAndNormal.push_back(FLD.y); sleeperVerticesAndNormal.push_back(FLD.z);
				//n
				sleeperVerticesAndNormal.push_back(-heightOrient.x); sleeperVerticesAndNormal.push_back(-heightOrient.y); sleeperVerticesAndNormal.push_back(-heightOrient.z);
				//v
				sleeperVerticesAndNormal.push_back(BLD.x); sleeperVerticesAndNormal.push_back(BLD.y); sleeperVerticesAndNormal.push_back(BLD.z);
				//n
				sleeperVerticesAndNormal.push_back(-heightOrient.x); sleeperVerticesAndNormal.push_back(-heightOrient.y); sleeperVerticesAndNormal.push_back(-heightOrient.z);

				//RIGHT
				//v
				sleeperVerticesAndNormal.push_back(FRT.x); sleeperVerticesAndNormal.push_back(FRT.y); sleeperVerticesAndNormal.push_back(FRT.z);
				//n
				sleeperVerticesAndNormal.push_back(cross_t.x); sleeperVerticesAndNormal.push_back(cross_t.y); sleeperVerticesAndNormal.push_back(cross_t.z);
				//v
				sleeperVerticesAndNormal.push_back(BRT.x); sleeperVerticesAndNormal.push_back(BRT.y); sleeperVerticesAndNormal.push_back(BRT.z);
				//n
				sleeperVerticesAndNormal.push_back(cross_t.x); sleeperVerticesAndNormal.push_back(cross_t.y); sleeperVerticesAndNormal.push_back(cross_t.z);
				//v
				sleeperVerticesAndNormal.push_back(BRD.x); sleeperVerticesAndNormal.push_back(BRD.y); sleeperVerticesAndNormal.push_back(BRD.z);
				//n
				sleeperVerticesAndNormal.push_back(cross_t.x); sleeperVerticesAndNormal.push_back(cross_t.y); sleeperVerticesAndNormal.push_back(cross_t.z);

				//v
				sleeperVerticesAndNormal.push_back(FRT.x); sleeperVerticesAndNormal.push_back(FRT.y); sleeperVerticesAndNormal.push_back(FRT.z);
				//n
				sleeperVerticesAndNormal.push_back(cross_t.x); sleeperVerticesAndNormal.push_back(cross_t.y); sleeperVerticesAndNormal.push_back(cross_t.z);
				//v
				sleeperVerticesAndNormal.push_back(FRD.x); sleeperVerticesAndNormal.push_back(FRD.y); sleeperVerticesAndNormal.push_back(FRD.z);
				//n
				sleeperVerticesAndNormal.push_back(cross_t.x); sleeperVerticesAndNormal.push_back(cross_t.y); sleeperVerticesAndNormal.push_back(cross_t.z);
				//v
				sleeperVerticesAndNormal.push_back(BRD.x); sleeperVerticesAndNormal.push_back(BRD.y); sleeperVerticesAndNormal.push_back(BRD.z);
				//n
				sleeperVerticesAndNormal.push_back(cross_t.x); sleeperVerticesAndNormal.push_back(cross_t.y); sleeperVerticesAndNormal.push_back(cross_t.z);

				//LEFT
				//v
				sleeperVerticesAndNormal.push_back(FLT.x); sleeperVerticesAndNormal.push_back(FLT.y); sleeperVerticesAndNormal.push_back(FLT.z);
				//n
				sleeperVerticesAndNormal.push_back(-cross_t.x); sleeperVerticesAndNormal.push_back(-cross_t.y); sleeperVerticesAndNormal.push_back(-cross_t.z);
				//v
				sleeperVerticesAndNormal.push_back(BLT.x); sleeperVerticesAndNormal.push_back(BLT.y); sleeperVerticesAndNormal.push_back(BLT.z);
				//n
				sleeperVerticesAndNormal.push_back(-cross_t.x); sleeperVerticesAndNormal.push_back(-cross_t.y); sleeperVerticesAndNormal.push_back(-cross_t.z);
				//v
				sleeperVerticesAndNormal.push_back(BLD.x); sleeperVerticesAndNormal.push_back(BLD.y); sleeperVerticesAndNormal.push_back(BLD.z);
				//n
				sleeperVerticesAndNormal.push_back(-cross_t.x); sleeperVerticesAndNormal.push_back(-cross_t.y); sleeperVerticesAndNormal.push_back(-cross_t.z);

				//v
				sleeperVerticesAndNormal.push_back(FLT.x); sleeperVerticesAndNormal.push_back(FLT.y); sleeperVerticesAndNormal.push_back(FLT.z);
				//n
				sleeperVerticesAndNormal.push_back(-cross_t.x); sleeperVerticesAndNormal.push_back(-cross_t.y); sleeperVerticesAndNormal.push_back(-cross_t.z);
				//v
				sleeperVerticesAndNormal.push_back(FLD.x); sleeperVerticesAndNormal.push_back(FLD.y); sleeperVerticesAndNormal.push_back(FLD.z);
				//n
				sleeperVerticesAndNormal.push_back(-cross_t.x); sleeperVerticesAndNormal.push_back(-cross_t.y); sleeperVerticesAndNormal.push_back(-cross_t.z);
				//v
				sleeperVerticesAndNormal.push_back(BLD.x); sleeperVerticesAndNormal.push_back(BLD.y); sleeperVerticesAndNormal.push_back(BLD.z);
				//n
				sleeperVerticesAndNormal.push_back(-cross_t.x); sleeperVerticesAndNormal.push_back(-cross_t.y); sleeperVerticesAndNormal.push_back(-cross_t.z);

				//FRONT
				//v
				sleeperVerticesAndNormal.push_back(FRT.x); sleeperVerticesAndNormal.push_back(FRT.y); sleeperVerticesAndNormal.push_back(FRT.z);
				//n
				sleeperVerticesAndNormal.push_back(headOrient.x); sleeperVerticesAndNormal.push_back(headOrient.y); sleeperVerticesAndNormal.push_back(headOrient.z);
				//v
				sleeperVerticesAndNormal.push_back(FLT.x); sleeperVerticesAndNormal.push_back(FLT.y); sleeperVerticesAndNormal.push_back(FLT.z);
				//n
				sleeperVerticesAndNormal.push_back(headOrient.x); sleeperVerticesAndNormal.push_back(headOrient.y); sleeperVerticesAndNormal.push_back(headOrient.z);
				//v
				sleeperVerticesAndNormal.push_back(FRD.x); sleeperVerticesAndNormal.push_back(FRD.y); sleeperVerticesAndNormal.push_back(FRD.z);
				//n
				sleeperVerticesAndNormal.push_back(headOrient.x); sleeperVerticesAndNormal.push_back(headOrient.y); sleeperVerticesAndNormal.push_back(headOrient.z);

				//v
				sleeperVerticesAndNormal.push_back(FLT.x); sleeperVerticesAndNormal.push_back(FLT.y); sleeperVerticesAndNormal.push_back(FLT.z);
				//n
				sleeperVerticesAndNormal.push_back(headOrient.x); sleeperVerticesAndNormal.push_back(headOrient.y); sleeperVerticesAndNormal.push_back(headOrient.z);
				//v
				sleeperVerticesAndNormal.push_back(FLD.x); sleeperVerticesAndNormal.push_back(FLD.y); sleeperVerticesAndNormal.push_back(FLD.z);
				//n
				sleeperVerticesAndNormal.push_back(headOrient.x); sleeperVerticesAndNormal.push_back(headOrient.y); sleeperVerticesAndNormal.push_back(headOrient.z);
				//v
				sleeperVerticesAndNormal.push_back(FRD.x); sleeperVerticesAndNormal.push_back(FRD.y); sleeperVerticesAndNormal.push_back(FRD.z);
				//n
				sleeperVerticesAndNormal.push_back(headOrient.x); sleeperVerticesAndNormal.push_back(headOrient.y); sleeperVerticesAndNormal.push_back(headOrient.z);

				//BACK
				//v
				sleeperVerticesAndNormal.push_back(BRT.x); sleeperVerticesAndNormal.push_back(BRT.y); sleeperVerticesAndNormal.push_back(BRT.z);
				//n
				sleeperVerticesAndNormal.push_back(-headOrient.x); sleeperVerticesAndNormal.push_back(-headOrient.y); sleeperVerticesAndNormal.push_back(-headOrient.z);
				//v
				sleeperVerticesAndNormal.push_back(BLT.x); sleeperVerticesAndNormal.push_back(BLT.y); sleeperVerticesAndNormal.push_back(BLT.z);
				//n
				sleeperVerticesAndNormal.push_back(-headOrient.x); sleeperVerticesAndNormal.push_back(-headOrient.y); sleeperVerticesAndNormal.push_back(-headOrient.z);
				//v
				sleeperVerticesAndNormal.push_back(BRD.x); sleeperVerticesAndNormal.push_back(BRD.y); sleeperVerticesAndNormal.push_back(BRD.z);
				//n
				sleeperVerticesAndNormal.push_back(-headOrient.x); sleeperVerticesAndNormal.push_back(-headOrient.y); sleeperVerticesAndNormal.push_back(-headOrient.z);

				//v
				sleeperVerticesAndNormal.push_back(BLT.x); sleeperVerticesAndNormal.push_back(BLT.y); sleeperVerticesAndNormal.push_back(BLT.z);
				//n
				sleeperVerticesAndNormal.push_back(-headOrient.x); sleeperVerticesAndNormal.push_back(-headOrient.y); sleeperVerticesAndNormal.push_back(-headOrient.z);
				//v
				sleeperVerticesAndNormal.push_back(BLD.x); sleeperVerticesAndNormal.push_back(BLD.y); sleeperVerticesAndNormal.push_back(BLD.z);
				//n
				sleeperVerticesAndNormal.push_back(-headOrient.x); sleeperVerticesAndNormal.push_back(-headOrient.y); sleeperVerticesAndNormal.push_back(-headOrient.z);
				//v
				sleeperVerticesAndNormal.push_back(BRD.x); sleeperVerticesAndNormal.push_back(BRD.y); sleeperVerticesAndNormal.push_back(BRD.z);
				//n
				sleeperVerticesAndNormal.push_back(-headOrient.x); sleeperVerticesAndNormal.push_back(-headOrient.y); sleeperVerticesAndNormal.push_back(-headOrient.z);
				
				//glPushMatrix();
				//glTranslatef(qt0.x, qt0.y, qt0.z);
				//glMultMatrixf(rotation);
				//glScalef(sleeperWidth, sleeperHeight, sleeperLen);
				//glTranslatef(-0.5f, 0.0f, -0.5f);
				//drawHexahedron(color, doingShadows);
				//glPopMatrix();


				sleeperSpaceCount -= sleeperSpace;
			}

		}

		arcLength[i] = arcLen;
	}

	{
		trackVAO = new VAO;
		trackVAO->Bind();
		trackVerticeCount = trackVerticesAndNormal.size() / 2;
		VBO vbo(&trackVerticesAndNormal[0], trackVerticesAndNormal.size() * sizeof(float));

		trackVAO->LinkAttrib(vbo, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
		trackVAO->LinkAttrib(vbo, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		//trackVAO->LinkAttrib(vbo, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

		trackVAO->Unbind();
		vbo.Unbind();
		vbo.Delete();
	}

	{
		sleeperVAO = new VAO;
		sleeperVAO->Bind();
		sleeperVerticeCount = sleeperVerticesAndNormal.size() / 2;
		VBO vbo(&sleeperVerticesAndNormal[0], sleeperVerticesAndNormal.size() * sizeof(float));
		
		sleeperVAO->LinkAttrib(vbo, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
		sleeperVAO->LinkAttrib(vbo, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		
		sleeperVAO->Unbind();
		vbo.Unbind();
		vbo.Delete();
	}

	shader = new Shader("./Asset/Shader/default.vert", "./Asset/Shader/default.frag");
}

void Track::Draw(Shader* shader)
{
	shader->Use();

	shader->setFloat3("u_color", glm::vec3(0.5f, 0.3f, 0.1f));

	trackVAO->Bind();
	glDrawArrays(GL_TRIANGLES, 0, trackVerticeCount);

	shader->setFloat3("u_color", glm::vec3(0.58f, 0.5f, 0.3f));
	sleeperVAO->Bind();
	glDrawArrays(GL_TRIANGLES, 0, sleeperVerticeCount);
}


float Track::AddArcLen(float trainU, float len)
{
	const float nct = points.size();
	float result = trainU;
	int i = trainU;
	float t = trainU - i;
	const int DIVIDTIME = length[0].size();
	const float percent = ((float)1 / DIVIDTIME);
	int j = t * DIVIDTIME;
	if (len >= 0)
	{
		while (len >= length[i][j])
		{
			len -= length[i][j];
			j++;
			if (j >= DIVIDTIME)
			{
				j = 0;
				i++;
				if (i >= nct)i = 0;
			}
			result += percent;
		}
	}
	else
	{
		while (len < 0)
		{
			len += length[i][j];
			j--;
			if (j < 0)
			{
				j = DIVIDTIME - 1;
				i--;
				if (i < 0)i = nct - 1;
			}
			result -= percent;
		}
	}

	result += (len / arcLength[i]);
	if (result >= nct) result -= nct;
	if (result < 0) result += nct;

	return (result - trainU);
}

glm::vec3 Track::cubicSpline(glm::vec3 g[4], float t)
{
	glm::mat4x4 M
	(
		-1, 3, -3, 1,
		3, -6, 0, 4,
		-3, 3, 3, 1,
		1, 0, 0, 0
	);
	M /= 6.0f;
	glm::vec4 T(std::pow(t, 3), std::pow(t, 2), t, 1);
	glm::mat3x4 G(
		g[0].x, g[1].x, g[2].x, g[3].x,
		g[0].y, g[1].y, g[2].y, g[3].y,
		g[0].z, g[1].z, g[2].z, g[3].z);
	auto Q = T * (M * G);
	glm::vec3 qt(Q[0], Q[1], Q[2]);
	return qt;
}