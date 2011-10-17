#include <iostream>

#include "../include/VideoCapture.h"

extern "C"
{
#include <pthread.h>
}

using namespace std;
using namespace br::ufscar::lince::util;

VideoCapture* i;
pthread_t captureThread;

void* videoCaptureCallback(void* param){
    i->run(param);

    pthread_exit(NULL);
}

int main() {
    cout << "   VideoCapture Test" << endl;
    vector<const char*> create_destinations;
    vector<const char*> remove_destinations;
    vector<const char*> destinations;
    void* param;

    cout << "Allocating object..." << endl;
    i->VideoCaptureFactory();
    cout << "  OK" << endl;
    cout << "Please press any key to continue...";
    cin.get();

    cout << "Setting some parameters..." << endl;
    i->setFrameRate(25);
    cout << " setFrameRate OK" << endl;
    i->setVideoBitrate(200000);
    cout << " setVideoBitrate OK" << endl;
    i->setSampleRate(22050);
    cout << " setSampleRate OK " << endl;
    i->setAudioBitrate(128000);
    cout << " setAudioBitrate OK " << endl;
    i->setAudioChannels(1);
    cout << " setAudioChannels OK " << endl;
    i->setAudioCodec("mp2");
    cout << " setAudioCodec OK " << endl;
    i->setVideoCodec("mpeg2video");
    cout << " setVideoCodec OK " << endl;
    i->setVideoFormat("rtp");
    cout << " setVideoFormat OK " << endl;
    i->addDestination("rtp://192.168.98.169:10000");
    i->addDestination("rtp://192.168.98.169:20000");
    i->addDestination("teste01.mpg");
    i->addDestination("teste02.mpg");
    i->addDestination("teste03.mpg");
    i->addDestination("teste04.mpg");
    i->addDestination("teste05.mpg");
    i->removeDestination("teste06.mpg");
    i->removeDestination("teste03");
    i->removeDestination("teste04.mpg");

    create_destinations.push_back("10000.avi");
    create_destinations.push_back("20000.avi");
    create_destinations.push_back("30000.avi");
    create_destinations.push_back("40000.avi");
    create_destinations.push_back("50000.avi");
    create_destinations.push_back("60000.avi");
    create_destinations.push_back("70000.avi");
    create_destinations.push_back("80000.avi");
    i->addDestination(create_destinations);

    remove_destinations.push_back("10000.avi");
    remove_destinations.push_back("30000.avi");
    remove_destinations.push_back("50000.avi");
    remove_destinations.push_back("60000");
    remove_destinations.push_back("80000.avi");
    i->removeDestination(remove_destinations);

    cout << " setDestination / removeDestination OK " << endl;

    cout << " RESULT: OK" << endl;
    cout << "Please press any key to continue...";
    cin.get();

    cout << "Printing parameters...";
    cout << " Video Format = " << i->getVideoFormat() << endl
         << " Video Codec = " << i->getVideoCodec() << endl
         << " Frame Rate = " << i->getFrameRate() << endl
         << " Video Bitrate = " << i->getVideoBitrate() << endl
         << " Audio Codec = " << i->getAudioCodec() << endl
         << " Sample Rate = " << i->getSampleRate() << endl
         << " Audio Bitrate = " << i->getAudioBitrate() << endl
         << " Channels = " << i->getAudioChannels() << endl
         << " Codec = " << i->getAudioCodec() << endl
         << " Destinations = " << endl;

    destinations = i->getDestinations();

    for (int j = 0; j < destinations.size(); j++){
        cout << "  #" << j << ": " << destinations[j] << endl;
    }

    cout << "  OK" << endl;
    cout << "Please press any key to continue...";
    cin.get();

    cout << "Capturing video...";

    param = NULL;
    pthread_create(&captureThread, NULL, videoCaptureCallback, NULL);

    int j = 0;
    while(j++ < 100000){
        if (j < 90000){
            cout << "Capturing video..." << endl;
        } else if (j == 90000){
            i->stop();
            cout << "Video capture end." << endl;
            cout << "Please press any key to continue...";
            cin.get();
        } else {
            cout << "Not recording anymore." << endl;
        }
    }

    cout << "  OK" << endl;

    cout << "VideoCapture Test was sucessfull." << endl;

    pthread_join(captureThread, NULL);

/*
    i->removeDestination("rtp://192.168.98.169:10000");
    i->removeDestination("rtp://192.168.98.169:20000");

    i->setVideoFormat("rtp");
    i->addDestination("rtp://192.168.98.169:10000");
    i->addDestination("rtp://192.168.98.169:20000");
    i->setFrameRate(25);
    i->setVideoBitrate(200000);
    i->setSampleRate(22050);
    i->setAudioBitrate(128000);
    i->setAudioChannels(2);
    i->setAudioCodec("mp2");
    i->setVideoCodec("mpeg2video");

    param = NULL;
    i->run(param);
*/
    return 0;
}

