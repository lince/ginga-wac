#include <iostream>

#include "../include/AudioCapture.h"

extern "C"
{
#include <pthread.h>
}

using namespace br::ufscar::lince::util;

AudioCapture* i;
pthread_t captureThread;

void* audioCaptureCallback(void* param){
    i->run(param);

    pthread_exit(NULL);
}

int main()
{
    using namespace std;

    cout << "   AudioCapture Test" << endl;
    vector<const char*> create_destinations;
    vector<const char*> remove_destinations;
    vector<const char*> destinations;
    void* param;

    cout << "Allocating object..." << endl;
    i->AudioCaptureFactory();
    cout << "  OK" << endl;
    cout << "Please press any key to continue...";
    cin.get();

    cout << "Setting some parameters..." << endl;
    i->setSampleRate(22050);
    cout << " setSampleRate OK " << endl;
    i->setAudioBitrate(128000);
    cout << " setAudioBitrate OK " << endl;
    i->setAudioChannels(1);
    cout << " setAudioChannels OK " << endl;
    i->setAudioFormat("wav");
    cout << " setAudioFormat OK " << endl;
    i->setAudioCodec("mp2");
    cout << " setAudioCodec OK " << endl;
    cout << "Please press any key to continue...";
    cin.get();

    i->addDestination("teste01.wav");
    i->addDestination("teste02.wav");
    i->addDestination("teste03.wav");
    i->addDestination("teste04.wav");
    i->addDestination("teste05.wav");
    i->removeDestination("teste06.wav");
    i->removeDestination("teste03.wav");
    i->removeDestination("teste04.wav");

    create_destinations.push_back("10000.mp2");
    create_destinations.push_back("20000.mp2");
    create_destinations.push_back("30000.mp2");
    create_destinations.push_back("40000.mp2");
    create_destinations.push_back("50000.mp2");
    create_destinations.push_back("60000.mp2");
    create_destinations.push_back("70000.mp2");
    create_destinations.push_back("80000.mp2");
    i->addDestination(create_destinations);

    remove_destinations.push_back("10000.mp2");
    remove_destinations.push_back("30000.mp2");
    remove_destinations.push_back("50000.mp2");
    remove_destinations.push_back("60000");
    remove_destinations.push_back("80000.mp2");
    i->removeDestination(remove_destinations);

    cout << " setDestination / removeDestination OK ";
    cout << " RESULT: OK" << endl;
    cout << "Please press any key to continue...";
    cin.get();

    cout << "Printing parameters...";
    cout << "Sample Rate = " << i->getSampleRate() << endl
         << " Bitrate = " << i->getAudioBitrate() << endl
         << " Channels = " << i->getAudioChannels() << endl
         << " Format = " << i->getAudioFormat() << endl
         << " Codec = " << i->getAudioCodec() << endl
         << " Destinations = " << endl;

    destinations = i->getDestinations();

    for (int j = 0; j < destinations.size(); j++){
        cout << "  #" << j << ": " << destinations[j] << endl;
    }

    cout << "  OK" << endl;
    cout << "Please press any key to continue...";
    cin.get();

    cout << "Capturing audio..." << endl;

    param = NULL;
    pthread_create(&captureThread, NULL, audioCaptureCallback, NULL);

    int j = 0;
    while(j++ < 100000){
        if (j < 90000){
            cout << "Capturing audio..." << endl;
        } else if (j == 90000){
            i->stop();
            cout << "Audio capture end." << endl;
            cout << "Please press any key to continue...";
            cin.get();
        } else {
            cout << "Not recording anymore." << endl;
        }
    }  

    cout << "  OK" << endl;

    cout << "AudioCapture Test was sucessfull." << endl;

    pthread_join(captureThread, NULL);

    return 0;
}

