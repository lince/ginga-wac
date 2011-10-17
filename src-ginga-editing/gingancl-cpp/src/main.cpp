/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licen�a Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribu�do na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

#include "../config.h"

#include "converter/NclDocumentManager.h"
using namespace ::br::pucrio::telemidia::converter::ncl;

#include "io/Window.h"
#include "io/OutputManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::io;

#include "player/AVPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#if HAVE_TUNER
#include "tuner/Tuner.h"
using namespace ::br::pucrio::telemidia::ginga::core::tuning;
#endif

#if HAVE_TSPARSER
#include "tsparser/Demuxer.h"
#include "tsparser/PipeFilter.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser;
#endif

#if HAVE_DSMCC
#include "dataprocessing/DataProcessor.h"
using namespace ::br::pucrio::telemidia::ginga::core::dataprocessing;
#endif

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "ncl/DeviceLayout.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "include/Formatter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl;

#include "include/PresentationEngineManager.h"
using namespace ::br::pucrio::telemidia::ginga::ncl;

#include <string>
#include <iostream>
using namespace std;

void printHelp() {
	cout << endl << "Usage: gingaNcl [OPTIONS]... [<--ncl> NCLFILE]" << endl;
	cout << "Example: gingaNcl --ncl test.ncl" << endl;
	cout << endl << "OPTIONS are:" << endl;
	cout << "-h, --help                  Display this information." << endl;
	cout << "-v, --version               Display version." << endl;
	cout << "    --enable-log [mode]     Enable log mode:" << endl;
	cout << "                              Turn on verbosity, gather all";
	cout << " output" << endl;
	cout << "                              messages and write them into a";
	cout << " device" << endl;
	cout << "                              according to mode." << endl;
	cout << "                                [mode] can be:" << endl;
	cout << "                                   'file'   write messages into";
	cout << " a file" << endl;
	cout << "                                           named logFile.txt.";
	cout << endl;
	cout << "                                   'stdout'  write messages into";
	cout << " /dev/stdout" << endl;
	cout << "                                   'null'   write messages into";
	cout << " /dev/null" << endl;
	cout << "                                            (default mode).";
	cout << endl;
	cout << "-v, --version               Display version." << endl;
}

void printVersion() {
	cout << endl;
	cout << "This is gingaNcl (open source version): a free " << endl;
	cout << " iTV middleware based on NCL language v0.9.28." << endl;
	cout << "(c) 2002-2007 The TeleMidia PUC-Rio Team." << endl << endl;
}

#if HAVE_TUNER
Tuner* tune(PresentationEngineManager* presentationManager) {

	// Create Main AV presentation Surface abstraction (Window class)
	Window* win;
	win = new Window(
		    0,
		    0,
		    OutputManager::getInstance()->getDeviceWidth(),
		    OutputManager::getInstance()->getDeviceHeight());

	win->draw();
	// Ginga-NCL Base modules
	Tuner* t;

	// Create Tuner to find Transport Stream (sync_byte = 0x47)
	t = new Tuner();

	cout << "gingaNcl tuner" << endl;
	presentationManager->setIsLocalNcl(false, t);

#if HAVE_TSPARSER
	Demuxer* demux;
	//int vPid = 0;
	//int aPid = 0;

	demux = new Demuxer();
	t->addListener(demux);

#if HAVE_DSMCC
	// Create DSM-CC module to receive SEs or mount OCs
	DataProcessor* dataProcessor;
	dataProcessor = new DataProcessor();

	dataProcessor->createSectionFilter(Pmt::STREAM_TYPE_DSMCC_TYPE_B, demux);
	dataProcessor->createSectionFilter(Pmt::STREAM_TYPE_DSMCC_TYPE_C, demux);

	dataProcessor->createSectionFilter(0x11, demux); //SDT
	dataProcessor->createSectionFilter(0x12, demux); //EIT
	dataProcessor->createSectionFilter(0x29, demux); //CDT

	// Add PresentationEngineManager as a listener of SEs and OCs
	dataProcessor->addEPGListener(presentationManager);
	dataProcessor->addSEListener("gingaEditingCommands", presentationManager);
	dataProcessor->addObjectListener(presentationManager);

	// Start Tunning
	t->start();

#endif //HAVE_DSMCC
#ifdef STx7100
	// Create Main AV
	AVPlayer* vp, * ap;
	int aPid, vPid;

	PipeFilter* audioFilter;
	PipeFilter* videoFilter;

	vPid = demux->getDefaultMainVideoPid();
	videoFilter = new PipeFilter(vPid);
	demux->addPidFilter(vPid, videoFilter);

	aPid = demux->getDefaultMainAudioPid();
	audioFilter = new PipeFilter(aPid);
	demux->addPidFilter(aPid, audioFilter);

	vp = new AVPlayer("sbtvd-ts://" + itos(vPid) + ".ts", true);
	vp->setVoutWindow(win);
	ProgramAV::getInstance()->setPlayer(vPid, vp);

	ap = new AVPlayer("sbtvd-ts://" + itos(aPid) + ".ts", false);
	ProgramAV::getInstance()->setPlayer(aPid, ap);

#else //!STx7100

	PipeFilter* mavFilter;
	mavFilter = new PipeFilter(0);
	demux->addTSFilter(0, mavFilter);

	// Create Main AV
	AVPlayer* mav;

	cout << "main create av" << endl;
	mav = new AVPlayer("sbtvd-ts://0.ts", true);
	mav->setVoutWindow(win);
	cout << "main create av ok" << endl;

	ProgramAV::getInstance()->setPlayer(0, mav);
	//vPid = demux->getDefaultMainVideoPid();
	//aPid = demux->getDefaultMainAudioPid();

#endif //STx7100
#endif //HAVE_TSPARSER

	win->show();
	return t;
}

#endif //HAVE_TUNER

int main(int argc, char *argv[]) {
	string nclFile = "";
	int i;

	setLogToNullDev();

	for (i = 1; i < argc; i++) {
		if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
			printHelp();
			return 0;
		}

		if ((strcmp(argv[i], "-v") == 0) ||
			    (strcmp(argv[i], "--version")) == 0) {

			printVersion();
			return 0;
		}

		if ((strcmp(argv[i], "--ncl") == 0) && ((i + 1) < argc)) {
			nclFile = (string)(argv[i+1]);
		}

		if ((strcmp(argv[i], "--enable-log") == 0) && ((i + 1) < argc)) {
			if (strcmp(argv[i+1], "file") == 0) {
				setLogToFile();

			} else if (strcmp(argv[i+1], "stdout") == 0) {
				setLogToStdoutDev();

			} else {
				setLogToNullDev();
			}
		}
	}

	initializeCurrentPath();
	DeviceLayout::getInstance()->addDevice(
			"default",
			OutputManager::getInstance()->getDeviceWidth(),
			OutputManager::getInstance()->getDeviceHeight());

	NclDocumentManager* documentManager;
    Formatter* formatter;
    PresentationEngineManager* presentationManager;

    documentManager = new NclDocumentManager("formatter");
    formatter = new Formatter("formatter", documentManager);
	presentationManager = new PresentationEngineManager(formatter, true);

	if (nclFile != "" && !isAbsolutePath(nclFile)) {
		nclFile = getCurrentPath() + nclFile;
	}

	cout << "NCLFILE = " << nclFile << endl;
	if (!fileExists(nclFile)) {
#if HAVE_TUNER
		Tuner* t;
		t = tune(presentationManager);
#endif

	} else {
		wclog << "NCLFILE = " << nclFile.c_str() << endl;
		wclog << "NCLFILE PATH = ";
		wclog << getCurrentPath().c_str();
		wclog << nclFile.c_str() << endl;

		presentationManager->openNclFile(nclFile);
		presentationManager->startPresentation();
		presentationManager->waitUnlockCondition();
	}

	pthread_mutex_t mutex;
	pthread_cond_t condition;

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&condition, NULL);

	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&condition, &mutex);
	pthread_mutex_unlock(&mutex);

	cout << "Releasing InputEventManager" << endl;
	InputManager::getInstance()->release();
	::usleep(1000000);
	cout << "Releasing DFB" << endl;
	OutputManager::getInstance()->release();

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&condition);
	cout << "all done!" << endl;
}
