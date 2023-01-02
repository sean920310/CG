#include "Track.h"

Track::Track()
{
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
void Track::readPoints(const char* filename)
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
	trainU = 0;
}