/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
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

#include "../../../../../include/io/DeviceManager.h"
#include "../../../../../include/io/interface/content/audio/FusionSoundAudioProvider.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace io {
	IFusionSound* FusionSoundAudioProvider::sound = NULL;
	set<IFusionSoundStream*>* FusionSoundAudioProvider::pool = NULL;
	bool FusionSoundAudioProvider::initialized = false;

	FusionSoundAudioProvider::FusionSoundAudioProvider(const char* mrl) {
		FSStreamDescription   s_desc;
		FSTrackDescription    t_dsc;

		if (sound == NULL) {
			initialize();
		}

		sound->CreateMusicProvider(sound, mrl, &(decoder));
		decoder->GetStreamDescription(decoder, &s_desc);

		s_desc.flags         = (FSStreamDescriptionFlags)(
				s_desc.flags | FSSDF_SAMPLEFORMAT | FSSDF_CHANNELS);

		s_desc.sampleformat  = FSSF_S16;
		s_desc.channels      = 2;

		sound->CreateStream(sound, &s_desc, &stream);
		//pool->insert(stream);
		stream->GetPlayback(stream, &playback);
	}

	FusionSoundAudioProvider::~FusionSoundAudioProvider() {
		if (decoder != NULL) {
			decoder->Stop(decoder);
			decoder->Release(decoder);
		}

		if (playback != NULL) {
			playback->Stop(playback);
			playback->Release(playback);
		}

		if (stream != NULL) {
			/*if (pool->count(stream) != 0) {
				pool->erase(pool->find(stream));
			}*/

			stream->Flush(stream);
			stream->Release(stream);
		}

		/*if (pool->empty() && sound != NULL) {
			sound->Release(sound);
			sound = NULL;
		}*/
	}

	void FusionSoundAudioProvider::initialize(int numArgs, char* args[]) {
		if (sound == NULL) {
			if (!initialized) {
				initialized = true;
				//pool = new set<IFusionSoundStream*>;
				FusionSoundInit(&numArgs, &args);
			}
			FusionSoundCreate(&sound);
		}
	}

	void* FusionSoundAudioProvider::getContent() {
		return decoder;
	}

	ISurface* FusionSoundAudioProvider::getPerfectSurface() {
		return NULL;
	}

	double FusionSoundAudioProvider::getTotalMediaTime() {
		double totalTime;

		if (decoder == NULL) {
			return 0;
		}

		decoder->GetLength(decoder, &totalTime);
		return totalTime;
	}

	double FusionSoundAudioProvider::getMediaTime() {
		double currentTime;

		if (decoder == NULL) {
			return -1;
		}

		decoder->GetPos(decoder, &currentTime);
		return currentTime;
	}

	void FusionSoundAudioProvider::setMediaTime(double pos) {
		if (decoder == NULL) {
			return;
		}

		decoder->SeekTo(decoder, pos);
	}

	void FusionSoundAudioProvider::playOver(
			ISurface* surface, bool hasVisual, IProviderListener* listener) {

		if (decoder == NULL) {
			cout << "FusionSoundAudioProvider::playOver decoder = NULL" << endl;
			return;
		}

		decoder->PlayToStream(decoder, stream);
	}

	void FusionSoundAudioProvider::pause() {
		stop();
	}

	void FusionSoundAudioProvider::resume(ISurface* surface, bool hasVisual) {
		if (decoder == NULL) {
			return;
		}

		decoder->PlayToStream(decoder, stream);
	}

	void FusionSoundAudioProvider::stop() {
		if (decoder == NULL) {
			return;
		}

		decoder->Stop(decoder);
	}

	void FusionSoundAudioProvider::setSoundLevel(float level) {
		if (playback == NULL) {
			return;
		}

		playback->SetVolume(playback, level);
	}

	void FusionSoundAudioProvider::getOriginalResolution(
			int* height, int* width) {

		*height = 0;
		*width = 0;
	}

	bool FusionSoundAudioProvider::releaseAll() {
		//TODO: release all structures

		cout << "FusionSoundAudioProvider::releaseAll" << endl;
		/*if (sound != NULL) {
			sound->Release(sound);
			sound = NULL;
		}*/
	}
}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::system::io::
		IContinuousMediaProvider* createFSAudioProvider(const char* mrl) {

	return (new ::br::pucrio::telemidia::ginga::core::system::io::
			FusionSoundAudioProvider(mrl));
}

extern "C" void destroyFSAudioProvider(
		::br::pucrio::telemidia::ginga::core::system::io::
		IContinuousMediaProvider* cmp) {

	delete cmp;
}
