// readWrite-bmp.cc
//
// extracts pixel data from user-specified .bmp file
// inserts data back into new .bmp file
//
// gw

// uncomment for MSVS#
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <cmath>
#include <ctime>
#include <omp.h>

using namespace std;

#pragma pack(1)
typedef struct {
	char id[2];
	int file_size;
	int reserved;
	int offset;
}  header_type;

#pragma pack(1)
typedef struct {
	int header_size;
	int width;
	int height;
	unsigned short int color_planes;
	unsigned short int color_depth;
	unsigned int compression;
	int image_size;
	int xresolution;
	int yresolution;
	int num_colors;
	int num_important_colors;
} information_type;

int main(int argc, char* argv[])
{
	header_type header;
	information_type information;
	string imageFileName, newImageFileName;
	unsigned char tempData[3];
	int row, col, row_bytes, padding;
	vector <vector <int> > data, newData;

	// prepare files
	cout << "Original imagefile? ";
	cin >> imageFileName;
	ifstream imageFile;
	imageFile.open(imageFileName.c_str(), ios::binary);
	if (!imageFile) {
		cerr << "file not found" << endl;
		exit(-1);
	}
	cout << "New imagefile name? ";
	cin >> newImageFileName;
	ofstream newImageFile;
	newImageFile.open(newImageFileName.c_str(), ios::binary);

	// read file header
	imageFile.read((char *)&header, sizeof(header_type));
	if (header.id[0] != 'B' || header.id[1] != 'M') {
		cerr << "Does not appear to be a .bmp file.  Goodbye." << endl;
		exit(-1);
	}
	// read/compute image information
	imageFile.read((char *)&information, sizeof(information_type));
	row_bytes = information.width * 3;
	padding = row_bytes % 4;
	if (padding)
		padding = 4 - padding;

	// extract image data, initialize vectors
	for (row = 0; row < information.height; row++) {
		data.push_back(vector <int>());
		for (col = 0; col < information.width; col++) {
			imageFile.read((char *)tempData, 3 * sizeof(unsigned char));
			data[row].push_back((int)tempData[0]);
		}
		if (padding)
			imageFile.read((char *)tempData, padding * sizeof(unsigned char));
	}
	cout << imageFileName << ": " << information.width << " x " << information.height << endl;


	// this loop shows how to simply recreate the original Black-and-White image
	for (row = 0; row < information.height; row++) {
		newData.push_back(vector <int>());
		for (col = 0; col < information.width; col++) {
			newData[row].push_back(data[row][col]);
		}
	}



	// pixel info is now stored in the 2D vector called 'data'
	// at this point you could, for example, print it to a file
	// your code (in any language) could then perform the image transformation
	// the transformed data would need to be re-inserted into an image-formatted file for display

	int dx[3][3] = { { 1,0,-1 },{ 2,0,-2 },{ 1,0,-1 } };
	int SUM;

	for (int y = 0; y < information.height - 2; y++)
	{
		for (int x = 0; x < information.width - 2; x++)
		{
			if (y == 0 || y >= information.height - 1 || x == 0 || x >= information.width - 1)
			{
				newData[y][x] = data[y][x];
				continue;
			}

			int sumX = 0;
			int sumY = 0;

			for (int i = -1; i < 2; i++)
			{
				for (int j = -1; j < 2; j++)
				{
					sumX = sumX + dx[j + 1][i + 1] * (int)data[y + j][x + i];
					sumY = sumY + dx[j + 1][i + 1] * (int)data[y + j][x + i];
				}
			}
			/*Convolution for Y*/

			//for (int i = -1; i < 2; i++)
			//{
				//for (int j = -1; j < 2; j++)
				//{
					//sumY = sumY + dx[j + 1][i + 1] * (int)data[y + j][x + i];
				//}
			//}
			/*Edge strength*/
			SUM = sqrt(pow((double)sumX, 2) + pow((double)sumY, 2));

			if (SUM > 255) SUM = 255;
			if (SUM < 0) SUM = 0;


			newData[y][x] = SUM;
			//SUM = sumX + sumY;
		}

			//unsigned char newPixel = (255 - (unsigned char)(SUM));
			
	}


	// if you wish to use this C++ skeleton program for image wrangling,
	// insert your transformation code here...


	// write header to new image file
	newImageFile.write((char *)&header, sizeof(header_type));
	newImageFile.write((char *)&information, sizeof(information_type));

	// write new image data to new image file
	for (row = 0; row < information.height; row++) {
		for (col = 0; col < information.width; col++) {
			tempData[0] = (unsigned char)newData[row][col];
			tempData[1] = (unsigned char)newData[row][col];
			tempData[2] = (unsigned char)newData[row][col];
			newImageFile.write((char *)tempData, 3 * sizeof(unsigned char));
		}
		if (padding) {
			tempData[0] = 0;
			tempData[1] = 0;
			tempData[2] = 0;
			newImageFile.write((char *)tempData, padding * sizeof(unsigned char));
		}
	}
	cout << newImageFileName << " done." << endl;
	imageFile.close();
	newImageFile.close();

	return 0;
}
