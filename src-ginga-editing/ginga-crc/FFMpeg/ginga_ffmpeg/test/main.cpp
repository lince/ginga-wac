//OK
//#include "../ffmpeg_ginga.h"
//OK
#include <ffmpeg_ginga.h>

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

#define TESTEIMAGEM 0
#define TESTEVIDEO 0
#define TESTERTP 0
#define TESTEFBRTP 0
#define TESTERTPAUDIOVIDEO 0
#define TESTERTPFBAUDIOVIDEO 1
#define TESTELINHACOMANDO 0

using namespace br::ufscar::lince::util;

int main(int argc, char **argv)
{
	GingaFFMpeg capture;

	printf("   FFMPEG_GINGA_API: Teste de captura de screenshot.\n");  
	int width;
	int height;
	string imageFormat;
	string destination;

	width = 800;
	height = 640;
	imageFormat = "png";
	destination = "screenshot.png";

	char** args;
	int i;
	stringstream sw, sh;
	string resolution;
	sw << width;
	sh << height;
	resolution = sw.str() + "x" + sh.str();

    args = new char* [20];
    for (i = 0; i < 20; i++)
    {
        args[i] = new char[50];
    }

    strcpy(args[0], "ffmpeg");
    strcpy(args[1], "-f");
    strcpy(args[2], "fbgrab");
    strcpy(args[3], "-r");
    strcpy(args[4], "1");
    strcpy(args[5], "-i");
    strcpy(args[6], ":");
    strcpy(args[7], "-f");
    strcpy(args[8], "rawvideo");
	strcpy(args[9], "-s");
	strcpy(args[10], resolution.c_str());
    strcpy(args[11], "-vcodec");
    strcpy(args[12], imageFormat.c_str());//png gif
    strcpy(args[13], "-vframes");
    strcpy(args[14], "1");
    strcpy(args[15], "-an");
    strcpy(args[16], destination.c_str());//png gif

    argc = 17;

    capture.start_recording();
    capture.ffmpeg(argc, args);

    for (i = 0; i < 20; i++)
    {
        delete[] args[i];
    }
    delete[] args;
}

