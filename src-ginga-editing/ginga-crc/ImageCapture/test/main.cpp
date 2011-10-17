#include <iostream>
#include <sstream>
#include "../include/ImageCapture.h"

using namespace std;
using namespace br::ufscar::lince::util;

int main()
{
    cout << "   ImageCapture Test" << endl;
    ImageCapture* i;
    cout << "Allocating object...";
    i->ImageCaptureFactory();
    cout << "  OK" << endl;

    cout << "Setting some parameters...";
    i->setWidth(1024);
    cout << " setWidth OK ";
    i->setHeight(768);
    cout << " setHeight OK ";
    i->setImageFormat("gif");
    cout << " setImageFormat OK ";
    i->setDestination("teste.gif");
    cout << " setDestination OK ";
    cout << " RESULT: OK" << endl;

    cout << "Printing parameters...";
    cout << "w = " << i->getWidth() << " h = " << i->getHeight() << " Format = " << i->getImageFormat() << endl << " Destination = " << i->getDestination() << endl;
    cout << "  OK" << endl;

    cout << "Capturing image...";
    i->capture();
    cout << "  OK" << endl;

    cout << "Now trying to capture more images..." << endl;
    int imCount = 0;
    std::string imFilename;
    std::stringstream imConvert;

    for (imCount = 0; imCount < 10; imCount++){
        imConvert << imCount;
        imFilename = "teste" + imConvert.str() + ".png";

        i->setWidth(800);
        i->setHeight(600);
        i->setImageFormat("png");
        i->setDestination(imFilename.c_str());
        cout << "Filename: " << i->getDestination() << endl;
        i->capture();
        //i->removeDestination(imFilename.c_str());
    }

//    delete i;
    cout << "ImageCapture Test was sucessfull." << endl;

    return 0;
}

