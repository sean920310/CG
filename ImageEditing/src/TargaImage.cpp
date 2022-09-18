///////////////////////////////////////////////////////////////////////////////
//
//      TargaImage.cpp                          Author:     Stephen Chenney
//                                              Modified:   Eric McDaniel
//                                              Date:       Fall 2004
//
//      Implementation of TargaImage methods.  You must implement the image
//  modification functions.
//
///////////////////////////////////////////////////////////////////////////////

#include "Globals.h"
#include "TargaImage.h"
#include "libtarga.h"
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include <math.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <tuple>
#include <algorithm>

using namespace std;

// constants
const int           RED = 0;                // red channel
const int           GREEN = 1;                // green channel
const int           BLUE = 2;                // blue channel
const unsigned char BACKGROUND[3] = { 0, 0, 0 };      // background color


// Computes n choose s, efficiently
double Binomial(int n, int s)
{
	double        res;

	res = 1;
	for (int i = 1; i <= s; i++)
		res = (n - i + 1) * res / i;

	return res;
}// Binomial


///////////////////////////////////////////////////////////////////////////////
//
//      Constructor.  Initialize member variables.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage::TargaImage() : width(0), height(0), data(NULL)
{}// TargaImage

///////////////////////////////////////////////////////////////////////////////
//
//      Constructor.  Initialize member variables.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage::TargaImage(int w, int h) : width(w), height(h)
{
	data = new unsigned char[width * height * 4];
	ClearToBlack();
}// TargaImage



///////////////////////////////////////////////////////////////////////////////
//
//      Constructor.  Initialize member variables to values given.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage::TargaImage(int w, int h, unsigned char* d)
{
	int i;

	width = w;
	height = h;
	data = new unsigned char[width * height * 4];

	for (i = 0; i < width * height * 4; i++)
		data[i] = d[i];
}// TargaImage

///////////////////////////////////////////////////////////////////////////////
//
//      Copy Constructor.  Initialize member to that of input
//
///////////////////////////////////////////////////////////////////////////////
TargaImage::TargaImage(const TargaImage& image)
{
	width = image.width;
	height = image.height;
	data = NULL;
	if (image.data != NULL) {
		data = new unsigned char[width * height * 4];
		memcpy(data, image.data, sizeof(unsigned char) * width * height * 4);
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//      Destructor.  Free image memory.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage::~TargaImage()
{
	if (data)
		delete[] data;
}// ~TargaImage


///////////////////////////////////////////////////////////////////////////////
//
//      Converts an image to RGB form, and returns the rgb pixel data - 24 
//  bits per pixel. The returned space should be deleted when no longer 
//  required.
//
///////////////////////////////////////////////////////////////////////////////
unsigned char* TargaImage::To_RGB(void)
{
	unsigned char* rgb = new unsigned char[width * height * 3];
	int		    i, j;

	if (!data)
		return NULL;

	// Divide out the alpha
	for (i = 0; i < height; i++)
	{
		int in_offset = i * width * 4;
		int out_offset = i * width * 3;

		for (j = 0; j < width; j++)
		{
			RGBA_To_RGB(data + (in_offset + j * 4), rgb + (out_offset + j * 3));
		}
	}

	return rgb;
}// TargaImage


///////////////////////////////////////////////////////////////////////////////
//
//      Save the image to a targa file. Returns 1 on success, 0 on failure.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Save_Image(const char* filename)
{
	TargaImage* out_image = Reverse_Rows();

	if (!out_image)
		return false;

	if (!tga_write_raw(filename, width, height, out_image->data, TGA_TRUECOLOR_32))
	{
		cout << "TGA Save Error: %s\n", tga_error_string(tga_get_last_error());
		return false;
	}

	delete out_image;

	return true;
}// Save_Image


///////////////////////////////////////////////////////////////////////////////
//
//      Load a targa image from a file.  Return a new TargaImage object which 
//  must be deleted by caller.  Return NULL on failure.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage* TargaImage::Load_Image(char* filename)
{
	unsigned char* temp_data;
	TargaImage* temp_image;
	TargaImage* result;
	int		        width, height;

	if (!filename)
	{
		cout << "No filename given." << endl;
		return NULL;
	}// if

	temp_data = (unsigned char*)tga_load(filename, &width, &height, TGA_TRUECOLOR_32);
	if (!temp_data)
	{
		cout << "TGA Error: %s\n", tga_error_string(tga_get_last_error());
		width = height = 0;
		return NULL;
	}
	temp_image = new TargaImage(width, height, temp_data);
	free(temp_data);

	result = temp_image->Reverse_Rows();

	delete temp_image;

	return result;
}// Load_Image

int TargaImage::indexOfPixel(int x, int y)
{
	return (width * y + x) * 4;
}

int toValidColor(int color) {
	if (color > 255)
		return 255;
	else if (color < 0)
		return 0;
	else
		return color;
}


///////////////////////////////////////////////////////////////////////////////
//
//      Convert image to grayscale.  Red, green, and blue channels should all 
//  contain grayscale value.  Alpha channel shoould be left unchanged.  Return
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::To_Grayscale()
{
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			int index = (width * j + i) * 4;
			float y = 0.299 * data[index] + 0.587 * data[index + 1] + 0.114 * data[index + 2];
			for (int m = 0; m < 3; m++) {
				data[index + m] = y;
			}
		}
	}
	return true;
}// To_Grayscale


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the image to an 8 bit image using uniform quantization.  Return 
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Quant_Uniform()
{
	for (int i = 0; i < width * height * 4; i += 4) {
		data[i] = int(data[i] / 32) * 32;
		data[i + 1] = int(data[i + 1] / 32) * 32;
		data[i + 2] = int(data[i + 2] / 64) * 64;
	}
	return true;
}// Quant_Uniform


///////////////////////////////////////////////////////////////////////////////
//
//      Convert the image to an 8 bit image using populosity quantization.  
//  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Quant_Populosity()
{
	vector<unsigned char> origin(width * height * 4, 0);
	//set img to 15bit
	for (int i = 0; i < width * height * 4; i += 4) {
		origin[i] = int(data[i] / 8) * 8;
		origin[i + 1] = int(data[i + 1] / 8) * 8;
		origin[i + 2] = int(data[i + 2] / 8) * 8;
		origin[i + 3] = data[i + 3];
	}

	//count each color amount
	vector<pair<tuple<unsigned char, unsigned char, unsigned char>, int>> mostColor;
	for (int i = 0; i < width * height * 4; i += 4) {
		tuple<unsigned char, unsigned char, unsigned char> pixelColor(origin[i], origin[i + 1], origin[i + 2]);
		bool found = false;
		for (auto& v : mostColor) {
			if (v.first._Equals(pixelColor))
			{
				v.second++;
				found = true;
				break;
			}
		}
		if (!found)
		{
			mostColor.push_back({ pixelColor,1 });
		}
	}

	//sort
	for (int i = 0; i < mostColor.size() - 1; i++)
	{
		for (int j = 0; j < mostColor.size() - 1; j++)
		{
			if (mostColor[j].second < mostColor[j + 1].second)
			{
				auto temp = mostColor[j];
				mostColor[j] = mostColor[j + 1];
				mostColor[j + 1] = temp;
			}
		}
	}

	for (int i = 0; i < width * height * 4; i += 4) {
		//find closest color
		double dis = DBL_MAX;
		int closestIndex = 0, j;
		for (j = 0; j < 256; j++) {
			double temp = sqrt(pow(data[i] - get<0>(mostColor[j].first), 2) + pow(data[i + 1] - get<1>(mostColor[j].first), 2) + pow(data[i + 2] - get<2>(mostColor[j].first), 2));
			if (temp < dis) {
				dis = temp;
				closestIndex = j;
			}
			if (dis == 0)
				break;
		}
		data[i] = get<0>(mostColor[closestIndex].first);
		data[i + 1] = get<1>(mostColor[closestIndex].first);
		data[i + 2] = get<2>(mostColor[closestIndex].first);
	}

	return true;
}// Quant_Populosity


///////////////////////////////////////////////////////////////////////////////
//
//      Dither the image using a threshold of 1/2.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_Threshold()
{
	this->To_Grayscale();
	for (int i = 0; i < width * height * 4; i += 4) {
		if (data[i] > 127)
		{
			data[i] = 255;
			data[i + 1] = 255;
			data[i + 2] = 255;
		}
		else
		{
			data[i] = 0;
			data[i + 1] = 0;
			data[i + 2] = 0;
		}
	}
	return true;
}// Dither_Threshold


///////////////////////////////////////////////////////////////////////////////
//
//      Dither image using random dithering.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_Random()
{
	this->To_Grayscale();
	for (int i = 0; i < width * height * 4; i += 4) {
		float randValue = float(rand()) / (RAND_MAX) * 0.4 - 0.2;
		if (data[i] + randValue * 255 > 127)
		{
			data[i] = 255;
			data[i + 1] = 255;
			data[i + 2] = 255;
		}
		else
		{
			data[i] = 0;
			data[i + 1] = 0;
			data[i + 2] = 0;
		}
	}
	return true;
}// Dither_Random


///////////////////////////////////////////////////////////////////////////////
//
//      Perform Floyd-Steinberg dithering on the image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_FS()
{
	this->To_Grayscale();
	for (int j = 0; j < height; j++) {
		for (int i = (j % 2) ? width - 1 : 0; (j % 2) ? i >= 0 : i < width; (j % 2) ? i-- : i++)
		{
			int index = indexOfPixel(i, j);
			int oldPixel = data[index];
			if (data[index] > 127)
			{
				for (int n = 0; n < 3; n++)
					data[index + n] = 255;
			}
			else
			{
				for (int n = 0; n < 3; n++)
					data[index + n] = 0;
			}
			int error = oldPixel - data[index];
			if (j % 2)
			{
				if (i != 0) {
					for (int n = 0; n < 3; n++)
						data[indexOfPixel(i - 1, j) + n] = toValidColor(data[indexOfPixel(i - 1, j) + n] + (7.0f / 16 * error));
				}
				if (i != width - 1 && j != height - 1) {
					for (int n = 0; n < 3; n++)
						data[indexOfPixel(i + 1, j + 1) + n] = toValidColor(data[indexOfPixel(i + 1, j + 1) + n] + (3.0f / 16 * error));
				}
				if (j != height - 1) {
					for (int n = 0; n < 3; n++)
						data[indexOfPixel(i, j + 1) + n] = toValidColor(data[indexOfPixel(i, j + 1) + n] + (5.0f / 16 * error));
				}
				if (i != 0 && j != height - 1) {
					for (int n = 0; n < 3; n++)
						data[indexOfPixel(i - 1, j + 1) + n] = toValidColor(data[indexOfPixel(i - 1, j + 1) + n] + (1.0f / 16 * error));
				}
			}
			else
			{
				if (i != width - 1) {
					for (int n = 0; n < 3; n++)
						data[indexOfPixel(i + 1, j) + n] = toValidColor(data[indexOfPixel(i + 1, j) + n] + (7.0f / 16 * error));
				}
				if (i != 0 && j != height - 1) {
					for (int n = 0; n < 3; n++)
						data[indexOfPixel(i - 1, j + 1) + n] = toValidColor(data[indexOfPixel(i - 1, j + 1) + n] + (3.0f / 16 * error));
				}
				if (j != height - 1) {
					for (int n = 0; n < 3; n++)
						data[indexOfPixel(i, j + 1) + n] = toValidColor(data[indexOfPixel(i, j + 1) + n] + (5.0f / 16 * error));
				}
				if (i != width - 1 && j != height - 1) {
					for (int n = 0; n < 3; n++)
						data[indexOfPixel(i + 1, j + 1) + n] = toValidColor(data[indexOfPixel(i + 1, j + 1) + n] + (1.0f / 16 * error));
				}
			}
		}
	}
	return true;
}// Dither_FS


///////////////////////////////////////////////////////////////////////////////
//
//      Dither the image while conserving the average brightness.  Return 
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_Bright()
{
	this->To_Grayscale();

	unsigned long long int total = 0;
	vector<int> histogram(256, 0);
	for (int i = 0; i < width * height * 4; i += 4) {
		total += data[i];
		histogram[data[i]]++;
	}
	float avg = total / (width * height);

	int dark = (width * height) * (1 - avg / 255);
	int thresh = 0;

	for (int i = 0; i < 256; i++) {
		dark -= histogram[i];
		if (dark <= 0)
		{
			thresh = i - 1;
			break;
		}
	}

	for (int i = 0; i < width * height * 4; i += 4) {
		if (data[i] > thresh)
		{
			data[i] = 255;
			data[i + 1] = 255;
			data[i + 2] = 255;
		}
		else
		{
			data[i] = 0;
			data[i + 1] = 0;
			data[i + 2] = 0;
		}
	}
	return true;
}// Dither_Bright


///////////////////////////////////////////////////////////////////////////////
//
//      Perform clustered differing of the image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_Cluster()
{
	this->To_Grayscale();
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			float mask[4][4] = {
				{0.75,0.375,0.625,0.25},
				{0.0625,1,0.875,0.4375},
				{0.5,0.8125,0.9375,0.125},
				{0.1875,0.5625,0.3125,0.6875}
			};
			if (data[indexOfPixel(x,y)] > mask[y % 4][x % 4] * 255)
			{
				data[indexOfPixel(x, y)] = 255;
				data[indexOfPixel(x, y) + 1] = 255;
				data[indexOfPixel(x, y )+ 2] = 255;
			}
			else
			{
				data[indexOfPixel(x, y)] = 0;
				data[indexOfPixel(x, y) + 1] = 0;
				data[indexOfPixel(x, y) + 2] = 0;
			}
		}
	}
	return true;
}// Dither_Cluster


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the image to an 8 bit image using Floyd-Steinberg dithering over
//  a uniform quantization - the same quantization as in Quant_Uniform.
//  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Dither_Color()
{
	int error[3];
	const int rg[8] = { 0, 36, 73, 109, 146, 182, 219 , 255 };
	const int b[4] = { 0, 85, 170, 255 };
	for (int j = 0; j < height; j++) {
		for (int i = (j % 2) ? width - 1 : 0; (j % 2) ? i >= 0 : i < width; (j % 2) ? i-- : i++)
		{

			error[0] = data[indexOfPixel(i, j) + 0] - rg[int(data[indexOfPixel(i, j) + 0] / 32)];
			error[1] = data[indexOfPixel(i, j) + 1] - rg[int(data[indexOfPixel(i, j) + 1] / 32)];
			error[2] = data[indexOfPixel(i, j) + 2] - b[int(data[indexOfPixel(i, j) + 2] / 64)];
			data[indexOfPixel(i, j) + 0] = rg[int(data[indexOfPixel(i, j) + 0] / 32)];
			data[indexOfPixel(i, j) + 1] = rg[int(data[indexOfPixel(i, j) + 1] / 32)];
			data[indexOfPixel(i, j) + 2] = b[int(data[indexOfPixel(i, j) + 2] / 64)];

			if (j % 2)
			{
				if (i != 0) {
					for (int n = 0; n < 3; n++)
						data[indexOfPixel(i - 1, j) + n] = toValidColor(data[indexOfPixel(i - 1, j) + n] + (7.0f / 16 * error[n]));
				}
				if (i != width - 1 && j != height - 1) {
					for (int n = 0; n < 3; n++)
						data[indexOfPixel(i + 1, j + 1) + n] = toValidColor(data[indexOfPixel(i + 1, j + 1) + n] + (3.0f / 16 * error[n]));
				}
				if (j != height - 1) {
					for (int n = 0; n < 3; n++)
						data[indexOfPixel(i, j + 1) + n] = toValidColor(data[indexOfPixel(i, j + 1) + n] + (5.0f / 16 * error[n]));
				}
				if (i != 0 && j != height - 1) {
					for (int n = 0; n < 3; n++)
						data[indexOfPixel(i - 1, j + 1) + n] = toValidColor(data[indexOfPixel(i - 1, j + 1) + n] + (1.0f / 16 * error[n]));
				}
			}
			else
			{
				if (i != width - 1) {
					for (int n = 0; n < 3; n++)
						data[indexOfPixel(i + 1, j) + n] = toValidColor(data[indexOfPixel(i + 1, j) + n] + (7.0f / 16 * error[n]));
				}
				if (i != 0 && j != height - 1) {
					for (int n = 0; n < 3; n++)
						data[indexOfPixel(i - 1, j + 1) + n] = toValidColor(data[indexOfPixel(i - 1, j + 1) + n] + (3.0f / 16 * error[n]));
				}
				if (j != height - 1) {
					for (int n = 0; n < 3; n++)
						data[indexOfPixel(i, j + 1) + n] = toValidColor(data[indexOfPixel(i, j + 1) + n] + (5.0f / 16 * error[n]));
				}
				if (i != width - 1 && j != height - 1) {
					for (int n = 0; n < 3; n++)
						data[indexOfPixel(i + 1, j + 1) + n] = toValidColor(data[indexOfPixel(i + 1, j + 1) + n] + (1.0f / 16 * error[n]));
				}
			}
		}
	}
	return true;
}// Dither_Color


///////////////////////////////////////////////////////////////////////////////
//
//      Composite the current image over the given image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Comp_Over(TargaImage* pImage)
{
	if (width != pImage->width || height != pImage->height)
	{
		cout << "Comp_Over: Images not the same size\n";
		return false;
	}

	ClearToBlack();
	return false;
}// Comp_Over


///////////////////////////////////////////////////////////////////////////////
//
//      Composite this image "in" the given image.  See lecture notes for 
//  details.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Comp_In(TargaImage* pImage)
{
	if (width != pImage->width || height != pImage->height)
	{
		cout << "Comp_In: Images not the same size\n";
		return false;
	}

	ClearToBlack();
	return false;
}// Comp_In


///////////////////////////////////////////////////////////////////////////////
//
//      Composite this image "out" the given image.  See lecture notes for 
//  details.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Comp_Out(TargaImage* pImage)
{
	if (width != pImage->width || height != pImage->height)
	{
		cout << "Comp_Out: Images not the same size\n";
		return false;
	}

	ClearToBlack();
	return false;
}// Comp_Out


///////////////////////////////////////////////////////////////////////////////
//
//      Composite current image "atop" given image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Comp_Atop(TargaImage* pImage)
{
	if (width != pImage->width || height != pImage->height)
	{
		cout << "Comp_Atop: Images not the same size\n";
		return false;
	}

	ClearToBlack();
	return false;
}// Comp_Atop


///////////////////////////////////////////////////////////////////////////////
//
//      Composite this image with given image using exclusive or (XOR).  Return
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Comp_Xor(TargaImage* pImage)
{
	if (width != pImage->width || height != pImage->height)
	{
		cout << "Comp_Xor: Images not the same size\n";
		return false;
	}

	ClearToBlack();
	return false;
}// Comp_Xor


///////////////////////////////////////////////////////////////////////////////
//
//      Calculate the difference bewteen this imag and the given one.  Image 
//  dimensions must be equal.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Difference(TargaImage* pImage)
{
	if (!pImage)
		return false;

	if (width != pImage->width || height != pImage->height)
	{
		cout << "Difference: Images not the same size\n";
		return false;
	}// if

	for (int i = 0; i < width * height * 4; i += 4)
	{
		unsigned char        rgb1[3];
		unsigned char        rgb2[3];

		RGBA_To_RGB(data + i, rgb1);
		RGBA_To_RGB(pImage->data + i, rgb2);

		data[i] = abs(rgb1[0] - rgb2[0]);
		data[i + 1] = abs(rgb1[1] - rgb2[1]);
		data[i + 2] = abs(rgb1[2] - rgb2[2]);
		data[i + 3] = 255;
	}

	return true;
}// Difference


///////////////////////////////////////////////////////////////////////////////
//
//      Perform 5x5 box filter on this image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Filter_Box()
{
	float filter[5][5] = {
		{0.04,0.04,0.04,0.04,0.04},
		{0.04,0.04,0.04,0.04,0.04},
		{0.04,0.04,0.04,0.04,0.04},
		{0.04,0.04,0.04,0.04,0.04},
		{0.04,0.04,0.04,0.04,0.04}
	};
	vector<unsigned char> origin(width * height * 4, 0);

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			int index = indexOfPixel(i, j);

			float dst[3] = { 0,0,0 };
			for (int m = 0; m < 5; m++) {
				for (int n = 0; n < 5; n++) {
					int filterX = i + m - 2, filterY = j + n - 2;
					if (filterX < 0)
						filterX = -filterX;
					if (filterY < 0)
						filterY = -filterY;
					if (filterX > width - 1)
						filterX = width - 1 - (filterX - (width - 1));
					if (filterY > height - 1)
						filterY = height - 1 - (filterY - (height - 1));
					for (int c = 0; c < 3; c++)
						dst[c] += filter[n][m] * data[indexOfPixel(filterX, filterY) + c];
				}
			}
			for (int c = 0; c < 3; c++)
				origin[indexOfPixel(i, j) + c] = toValidColor(dst[c]);
		}
	}

	for (int i = 0; i < width * height * 4; i += 4) {
		data[i] = origin[i];
		data[i + 1] = origin[i + 1];
		data[i + 2] = origin[i + 2];
	}
	return true;
}// Filter_Box


///////////////////////////////////////////////////////////////////////////////
//
//      Perform 5x5 Bartlett filter on this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Filter_Bartlett()
{
	float filter[5][5] = {
		{1.0f / 81,2.0f / 81,3.0 / 81,2.0f / 81,1.0f / 81},
		{2.0f / 81,4.0f / 81,6.0 / 81,4.0f / 81,2.0f / 81},
		{3.0f / 81,6.0f / 81,9.0 / 81,6.0f / 81,3.0f / 81},
		{2.0f / 81,4.0f / 81,6.0 / 81,4.0f / 81,2.0f / 81},
		{1.0f / 81,2.0f / 81,3.0 / 81,2.0f / 81,1.0f / 81}
	};
	vector<unsigned char> origin(width * height * 4, 0);

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			float dst[3] = { 0,0,0 };
			for (int m = 0; m < 5; m++) {
				for (int n = 0; n < 5; n++) {
					int filterX = i + m - 2, filterY = j + n - 2;
					if (filterX < 0)
						filterX = -filterX;
					if (filterY < 0)
						filterY = -filterY;
					if (filterX > width - 1)
						filterX = width - 1 - (filterX - (width - 1));
					if (filterY > height - 1)
						filterY = height - 1 - (filterY - (height - 1));
					for (int c = 0; c < 3; c++)
						dst[c] += filter[n][m] * data[indexOfPixel(filterX, filterY) + c];
				}
			}
			for (int c = 0; c < 3; c++)
				origin[indexOfPixel(i, j) + c] = toValidColor(dst[c]);
		}
	}

	for (int i = 0; i < width * height * 4; i += 4) {
		data[i] = origin[i];
		data[i + 1] = origin[i + 1];
		data[i + 2] = origin[i + 2];
	}
	return true;
}// Filter_Bartlett


///////////////////////////////////////////////////////////////////////////////
//
//      Perform 5x5 Gaussian filter on this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Filter_Gaussian()
{
	float filter[5][5] = {
		{1.0f / 256,4.0f / 256,6.0 / 256,4.0f / 256,1.0f / 256},
		{4.0f / 256,16.0f / 256,24.0 / 256,16.0f / 256,4.0f / 256},
		{6.0f / 256,24.0f / 256,36.0 / 256,24.0f / 256,6.0f / 256},
		{4.0f / 256,16.0f / 256,24.0 / 256,16.0f / 256,4.0f / 256},
		{1.0f / 256,4.0f / 256,6.0 / 256,4.0f / 256,1.0f / 256}
	};
	vector<unsigned char> origin(width * height * 4, 0);

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			float dst[3] = { 0,0,0 };
			for (int m = 0; m < 5; m++) {
				for (int n = 0; n < 5; n++) {
					int filterX = i + m - 2, filterY = j + n - 2;
					if (filterX < 0)
						filterX = -filterX;
					if (filterY < 0)
						filterY = -filterY;
					if (filterX > width - 1)
						filterX = width - 1 - (filterX - (width - 1));
					if (filterY > height - 1)
						filterY = height - 1 - (filterY - (height - 1));
					for (int c = 0; c < 3; c++)
						dst[c] += filter[n][m] * data[indexOfPixel(filterX, filterY) + c];
				}
			}
			for (int c = 0; c < 3; c++)
				origin[indexOfPixel(i, j) + c] = toValidColor(dst[c]);
		}
	}

	for (int i = 0; i < width * height * 4; i += 4) {
		data[i] = origin[i];
		data[i + 1] = origin[i + 1];
		data[i + 2] = origin[i + 2];
	}
	return true;
}// Filter_Gaussian

///////////////////////////////////////////////////////////////////////////////
//
//      Perform NxN Gaussian filter on this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////

bool TargaImage::Filter_Gaussian_N(unsigned int N)
{
	vector<vector<float>> filter(N, vector<float>(N, 0));
	//Gaussian Function
	//N * N matrix
	float total = 0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			filter[i][j] = exp(-(pow(i - int(N / 2), 2) + pow(j - int(N / 2), 2)) / 2);
			total += filter[i][j];
		}
	}
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			filter[i][j] /= total;
			cout << filter[i][j] << " ";
		}
		cout << endl;
	}

	vector<unsigned char> origin(width * height * 4, 0);
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			float dst[3] = { 0,0,0 };
			for (int m = 0; m < N; m++) {
				for (int n = 0; n < N; n++) {
					int filterX = i + m - (N / 2), filterY = j + n - (N / 2);
					if (filterX < 0)
						filterX = -filterX;
					if (filterY < 0)
						filterY = -filterY;
					if (filterX > width - 1)
						filterX = width - 1 - (filterX - (width - 1));
					if (filterY > height - 1)
						filterY = height - 1 - (filterY - (height - 1));
					for (int c = 0; c < 3; c++)
						dst[c] += filter[n][m] * data[indexOfPixel(filterX, filterY) + c];
				}
			}
			for (int c = 0; c < 3; c++)
				origin[indexOfPixel(i, j) + c] = toValidColor(dst[c]);
		}
	}

	for (int i = 0; i < width * height * 4; i += 4) {
		data[i] = origin[i];
		data[i + 1] = origin[i + 1];
		data[i + 2] = origin[i + 2];
	}
	return true;
}// Filter_Gaussian_N


///////////////////////////////////////////////////////////////////////////////
//
//      Perform 5x5 edge detect (high pass) filter on this image.  Return 
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Filter_Edge()
{
	float lowPass[5][5] = {
		{1.0f / 256,4.0f / 256,6.0 / 256,4.0f / 256,1.0f / 256},
		{4.0f / 256,16.0f / 256,24.0 / 256,16.0f / 256,4.0f / 256},
		{6.0f / 256,24.0f / 256,36.0 / 256,24.0f / 256,6.0f / 256},
		{4.0f / 256,16.0f / 256,24.0 / 256,16.0f / 256,4.0f / 256},
		{1.0f / 256,4.0f / 256,6.0 / 256,4.0f / 256,1.0f / 256}
	};
	vector<unsigned char> origin(width * height * 4, 0);

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			float dst[3] = { 0,0,0 };
			for (int m = 0; m < 5; m++) {
				for (int n = 0; n < 5; n++) {
					int filterX = i + m - 2, filterY = j + n - 2;
					if (filterX < 0)
						filterX = -filterX;
					if (filterY < 0)
						filterY = -filterY;
					if (filterX > width - 1)
						filterX = width - 1 - (filterX - (width - 1));
					if (filterY > height - 1)
						filterY = height - 1 - (filterY - (height - 1));
					for (int c = 0; c < 3; c++)
						dst[c] += lowPass[n][m] * data[indexOfPixel(filterX, filterY) + c];
				}
			}
			for (int c = 0; c < 3; c++)
				origin[indexOfPixel(i, j) + c] = toValidColor(dst[c]);
		}
	}

	for (int i = 0; i < width * height * 4; i += 4) {
		data[i] = toValidColor(data[i] - origin[i]);
		data[i + 1] = toValidColor(data[i + 1] - origin[i + 1]);
		data[i + 2] = toValidColor(data[i + 2] - origin[i + 2]);
	}
	return true;
}// Filter_Edge


///////////////////////////////////////////////////////////////////////////////
//
//      Perform a 5x5 enhancement filter to this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Filter_Enhance()
{
	float filter[5][5] = {
		{1.0f / 256,4.0f / 256,6.0 / 256,4.0f / 256,1.0f / 256},
		{4.0f / 256,16.0f / 256,24.0 / 256,16.0f / 256,4.0f / 256},
		{6.0f / 256,24.0f / 256,36.0 / 256,24.0f / 256,6.0f / 256},
		{4.0f / 256,16.0f / 256,24.0 / 256,16.0f / 256,4.0f / 256},
		{1.0f / 256,4.0f / 256,6.0 / 256,4.0f / 256,1.0f / 256}
	};
	vector<unsigned char> origin(width * height * 4, 0);

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			float dst[3] = { 0,0,0 };
			for (int m = 0; m < 5; m++) {
				for (int n = 0; n < 5; n++) {
					int filterX = i + m - 2, filterY = j + n - 2;
					if (filterX < 0)
						filterX = -filterX;
					if (filterY < 0)
						filterY = -filterY;
					if (filterX > width - 1)
						filterX = width - 1 - (filterX - (width - 1));
					if (filterY > height - 1)
						filterY = height - 1 - (filterY - (height - 1));
					for (int c = 0; c < 3; c++)
						dst[c] += filter[n][m] * data[indexOfPixel(filterX, filterY) + c];
				}
			}
			for (int c = 0; c < 3; c++)
				origin[indexOfPixel(i, j) + c] = toValidColor(dst[c]);
		}
	}

	for (int i = 0; i < width * height * 4; i += 4) {
		data[i] = toValidColor(int(data[i]) + int(data[i] - origin[i]));
		data[i + 1] = toValidColor(int(data[i + 1]) + int(data[i + 1] - origin[i + 1]));
		data[i + 2] = toValidColor(int(data[i + 2]) + int(data[i + 2] - origin[i + 2]));
	}
	return true;
}// Filter_Enhance


///////////////////////////////////////////////////////////////////////////////
//
//      Run simplified version of Hertzmann's painterly image filter.
//      You probably will want to use the Draw_Stroke funciton and the
//      Stroke class to help.
// Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::NPR_Paint()
{
	ClearToBlack();
	return false;
}



///////////////////////////////////////////////////////////////////////////////
//
//      Halve the dimensions of this image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Half_Size()
{
	float filter[3][3] = {
		{1.0f / 16, 1.0f / 8, 1.0f / 16},
		{1.0f / 8, 1.0f / 4, 1.0f / 8},
		{1.0f / 16, 1.0f / 8, 1.0f / 16}
	};
	vector<unsigned char> origin(width * height * 4, 0);

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			float dst[3] = { 0,0,0 };
			for (int m = 0; m < 3; m++) {
				for (int n = 0; n < 3; n++) {
					int filterX = i + m - 1, filterY = j + n - 1;
					if (filterX < 0)
						filterX = -filterX;
					if (filterY < 0)
						filterY = -filterY;
					if (filterX > width - 1)
						filterX = width - 1 - (filterX - (width - 1));
					if (filterY > height - 1)
						filterY = height - 1 - (filterY - (height - 1));
					for (int c = 0; c < 3; c++)
						dst[c] += filter[n][m] * data[indexOfPixel(filterX, filterY) + c];
				}
			}
			for (int c = 0; c < 3; c++)
				origin[indexOfPixel(i, j) + c] = toValidColor(dst[c]);
		}
	}

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++)
		{
			int newIndex = ((width / 2) * ((j + 1) / 2) + (i + 1) / 2) * 4;
			data[newIndex] = origin[indexOfPixel(i, j)];
			data[newIndex + 1] = origin[indexOfPixel(i, j) + 1];
			data[newIndex + 2] = origin[indexOfPixel(i, j) + 2];
		}
	}
	width /= 2;
	height /= 2;
	return true;
}// Half_Size


///////////////////////////////////////////////////////////////////////////////
//
//      Double the dimensions of this image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Double_Size()
{
	double evenFilter[3][3] = {
		{1.0f / 16, 1.0f / 8, 1.0f / 16},
		{1.0f / 8, 1.0f / 4, 1.0f / 8},
		{1.0f / 16, 1.0f / 8, 1.0f / 16}
	},
		oddFilter[4][4] = {
			{1.0f / 64,3.0f / 64,3.0f / 64,1.0f / 64},
			{3.0f / 64,9.0f / 64,9.0f / 64,3.0f / 64},
			{3.0f / 64,9.0f / 64,9.0f / 64,3.0f / 64},
			{1.0f / 64,3.0f / 64,3.0f / 64,1.0f / 64}
	},
		mixFilterV[4][3] = {
			{1.0f / 32,2.0f / 32,1.0f / 32},
			{3.0f / 32,6.0f / 32,3.0f / 32},
			{3.0f / 32,6.0f / 32,3.0f / 32},
			{1.0f / 32,2.0f / 32,1.0f / 32}
	}, mixFilterH[3][4] = {
			{1.0f / 32,3.0f / 32,3.0f / 32,1.0f / 32},
			{2.0f / 32,6.0f / 32,6.0f / 32,2.0f / 32},
			{1.0f / 32,3.0f / 32,3.0f / 32,1.0f / 32}
	};
	//vector<unsigned char> origin(width * height * 4 * 4, 0);
	unsigned char* origin = new unsigned char[width * 2 * height * 2 * 4];

	for (int j = 0; j < height * 2; j++) {
		for (int i = 0; i < width * 2; i++) {
			int sizeX = 0, sizeY = 0;
			if (i % 2 == 0 && j % 2 == 0) {
				sizeX = 3;
				sizeY = 3;
			}
			else if (i % 2 && j % 2) {
				sizeX = 4;
				sizeY = 4;
			}
			else if (i % 2 == 0 && j % 2) {
				sizeX = 3;
				sizeY = 4;
			}
			else
			{
				sizeX = 4;
				sizeY = 3;
			}
			double dst[4] = { 0,0,0,0 };
			for (int m = 0; m < sizeX; m++) {
				for (int n = 0; n < sizeY; n++) {
					int filterX = i / 2 + m - 1, filterY = j / 2 + n - 1;
					if (filterX < 0)
						filterX = -filterX;
					if (filterY < 0)
						filterY = -filterY;
					if (filterX > width - 1)
						filterX = width - 1 - (filterX - (width - 1));
					if (filterY > height - 1)
						filterY = height - 1 - (filterY - (height - 1));

					if (i % 2 == 0 && j % 2 == 0) {
						for (int c = 0; c < 4; c++)
							dst[c] += evenFilter[n][m] * data[indexOfPixel(filterX, filterY) + c];
					}
					else if (i % 2 && j % 2) {
						for (int c = 0; c < 4; c++)
							dst[c] += oddFilter[n][m] * data[indexOfPixel(filterX, filterY) + c];
					}
					else if (i % 2 == 0 && j % 2) {
						for (int c = 0; c < 4; c++)
							dst[c] += mixFilterV[n][m] * data[indexOfPixel(filterX, filterY) + c];
					}
					else
					{
						for (int c = 0; c < 4; c++)
							dst[c] += mixFilterH[n][m] * data[indexOfPixel(filterX, filterY) + c];
					}
				}
			}
			for (int c = 0; c < 4; c++)
				origin[((width * 2) * j + i)*4 + c] = toValidColor(dst[c]);
		}
	}


	auto temp = data;
	data = origin;
	delete[] temp;

	width *= 2;
	height *= 2;
	return true;
}// Double_Size


///////////////////////////////////////////////////////////////////////////////
//
//      Scale the image dimensions by the given factor.  The given factor is 
//  assumed to be greater than one.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Resize(float scale)
{
	double filter[4][4] = {
			{1.0f / 64,3.0f / 64,3.0f / 64,1.0f / 64},
			{3.0f / 64,9.0f / 64,9.0f / 64,3.0f / 64},
			{3.0f / 64,9.0f / 64,9.0f / 64,3.0f / 64},
			{1.0f / 64,3.0f / 64,3.0f / 64,1.0f / 64}
	};
	//vector<unsigned char> origin(width * height * 4 * 4, 0);.
	int newWidth = width * scale ,newHeight = height * scale;
	unsigned char* origin = new unsigned char[newWidth * newHeight * 4];

	for (int j = 0; j < newHeight; j++) {
		for (int i = 0; i < newWidth; i++) {
			double dst[4] = { 0,0,0,0 };
			for (int m = 0; m < 4; m++) {
				for (int n = 0; n < 4; n++) {
					int filterX = i / scale + m - 1, filterY = j / scale + n - 1;
					if (filterX < 0)
						filterX = -filterX;
					if (filterY < 0)
						filterY = -filterY;
					if (filterX > width - 1)
						filterX = width - 1 - (filterX - (width - 1));
					if (filterY > height - 1)
						filterY = height - 1 - (filterY - (height - 1));

					
					for (int c = 0; c < 4; c++)
						dst[c] += filter[n][m] * data[indexOfPixel(filterX, filterY) + c];
					
				}
			}
			for (int c = 0; c < 4; c++)
				origin[((newWidth) * j + i) * 4 + c] = toValidColor(dst[c]);
		}
	}

	auto temp = data;
	data = origin;
	delete[] temp;

	width = newWidth;
	height = newHeight;
	return true;
}// Resize


//////////////////////////////////////////////////////////////////////////////
//
//      Rotate the image clockwise by the given angle.  Do not resize the 
//  image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool TargaImage::Rotate(float angleDegrees)
{
	ClearToBlack();
	return false;
}// Rotate


//////////////////////////////////////////////////////////////////////////////
//
//      Given a single RGBA pixel return, via the second argument, the RGB
//      equivalent composited with a black background.
//
///////////////////////////////////////////////////////////////////////////////
void TargaImage::RGBA_To_RGB(unsigned char* rgba, unsigned char* rgb)
{
	const unsigned char	BACKGROUND[3] = { 0, 0, 0 };

	unsigned char  alpha = rgba[3];

	if (alpha == 0)
	{
		rgb[0] = BACKGROUND[0];
		rgb[1] = BACKGROUND[1];
		rgb[2] = BACKGROUND[2];
	}
	else
	{
		float	alpha_scale = (float)255 / (float)alpha;
		int	val;
		int	i;

		for (i = 0; i < 3; i++)
		{
			val = (int)floor(rgba[i] * alpha_scale);
			if (val < 0)
				rgb[i] = 0;
			else if (val > 255)
				rgb[i] = 255;
			else
				rgb[i] = val;
		}
	}
}// RGA_To_RGB


///////////////////////////////////////////////////////////////////////////////
//
//      Copy this into a new image, reversing the rows as it goes. A pointer
//  to the new image is returned.
//
///////////////////////////////////////////////////////////////////////////////
TargaImage* TargaImage::Reverse_Rows(void)
{
	unsigned char* dest = new unsigned char[width * height * 4];
	TargaImage* result;
	int 	        i, j;

	if (!data)
		return NULL;

	for (i = 0; i < height; i++)
	{
		int in_offset = (height - i - 1) * width * 4;
		int out_offset = i * width * 4;

		for (j = 0; j < width; j++)
		{
			dest[out_offset + j * 4] = data[in_offset + j * 4];
			dest[out_offset + j * 4 + 1] = data[in_offset + j * 4 + 1];
			dest[out_offset + j * 4 + 2] = data[in_offset + j * 4 + 2];
			dest[out_offset + j * 4 + 3] = data[in_offset + j * 4 + 3];
		}
	}

	result = new TargaImage(width, height, dest);
	delete[] dest;
	return result;
}// Reverse_Rows


///////////////////////////////////////////////////////////////////////////////
//
//      Clear the image to all black.
//
///////////////////////////////////////////////////////////////////////////////
void TargaImage::ClearToBlack()
{
	memset(data, 0, width * height * 4);
}// ClearToBlack


///////////////////////////////////////////////////////////////////////////////
//
//      Helper function for the painterly filter; paint a stroke at
// the given location
//
///////////////////////////////////////////////////////////////////////////////
void TargaImage::Paint_Stroke(const Stroke& s) {
	int radius_squared = (int)s.radius * (int)s.radius;
	for (int x_off = -((int)s.radius); x_off <= (int)s.radius; x_off++) {
		for (int y_off = -((int)s.radius); y_off <= (int)s.radius; y_off++) {
			int x_loc = (int)s.x + x_off;
			int y_loc = (int)s.y + y_off;
			// are we inside the circle, and inside the image?
			if ((x_loc >= 0 && x_loc < width && y_loc >= 0 && y_loc < height)) {
				int dist_squared = x_off * x_off + y_off * y_off;
				if (dist_squared <= radius_squared) {
					data[(y_loc * width + x_loc) * 4 + 0] = s.r;
					data[(y_loc * width + x_loc) * 4 + 1] = s.g;
					data[(y_loc * width + x_loc) * 4 + 2] = s.b;
					data[(y_loc * width + x_loc) * 4 + 3] = s.a;
				}
				else if (dist_squared == radius_squared + 1) {
					data[(y_loc * width + x_loc) * 4 + 0] =
						(data[(y_loc * width + x_loc) * 4 + 0] + s.r) / 2;
					data[(y_loc * width + x_loc) * 4 + 1] =
						(data[(y_loc * width + x_loc) * 4 + 1] + s.g) / 2;
					data[(y_loc * width + x_loc) * 4 + 2] =
						(data[(y_loc * width + x_loc) * 4 + 2] + s.b) / 2;
					data[(y_loc * width + x_loc) * 4 + 3] =
						(data[(y_loc * width + x_loc) * 4 + 3] + s.a) / 2;
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//      Build a Stroke
//
///////////////////////////////////////////////////////////////////////////////
Stroke::Stroke() {}

///////////////////////////////////////////////////////////////////////////////
//
//      Build a Stroke
//
///////////////////////////////////////////////////////////////////////////////
Stroke::Stroke(unsigned int iradius, unsigned int ix, unsigned int iy,
	unsigned char ir, unsigned char ig, unsigned char ib, unsigned char ia) :
	radius(iradius), x(ix), y(iy), r(ir), g(ig), b(ib), a(ia)
{
}

