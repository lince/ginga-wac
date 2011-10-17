#include "../include/VideoCapture.h"

namespace br {
    namespace ufscar {
        namespace lince {
            namespace util {

                VideoCapture* VideoCapture::instance = NULL;
                GingaFFMpeg VideoCapture::ffmpegCapture;

                /**
                 * Factory.
                 * Devolve uma instância de VideoCapture.
                 * @return instância de VideoCapture.
                 */
                VideoCapture* VideoCapture::VideoCaptureFactory() {
                    if (instance == NULL) {
                        instance = new VideoCapture();
                    }
                    return instance;
                }

                /**
                 * Destrutor.
                 */
                VideoCapture::~VideoCapture() {
                    if (instance != NULL) {
                        delete instance;
                    }
                    instance = NULL;
                }

                /**
                 * Configura a taxa de quadros por segundo (fps) do vídeo a ser capturado.
                 * @param frameRate taxa de quadros (fps) do vídeo destino (valor padrão: 25).
                 */
                void VideoCapture::setFrameRate(int FrameRate) {
                    if (FrameRate <= 0) {
                        instance->frameRate = 25;
                    } else {
                        instance->frameRate = FrameRate;
                    }
                }

                /**
                 * Retorna a taxa de quadros por segundo (fps) do vídeo a ser capturado.
                 * @return taxa de quadros (fps) do vídeo destino.
                 */
                int VideoCapture::getFrameRate() {
                    return instance->frameRate;
                }

                /**
                 * Configura a taxa de amostragem do áudio do vídeo a ser capturado.
                 * @param sampleRate taxa de amostragem do áudio do vídeo destino (valor padrão: 44100).
                 */
                void VideoCapture::setSampleRate(int SampleRate) {
                    if (SampleRate <= 0) {
                        instance->sampleRate = 44100;
                    } else {
                        instance->sampleRate = SampleRate;
                    }
                }

                /**
                 * Retorna a taxa de amostragem do áudio do vídeo a ser capturado.
                 * @return taxa de amostragem do áudio do vídeo destino.
                 */
                int VideoCapture::getSampleRate() {
                    return instance->sampleRate;
                }

                /**
                 * Configura a taxa de bits (bit/s) do vídeo a ser capturado.
                 * @param videoBitrate taxa de bits do vídeo destino (valor padrão: 200000).
                 */
                void VideoCapture::setVideoBitrate(int VideoBitrate) {
                    if (VideoBitrate <= 0) {
                        instance->videoBitrate = 200000;
                    } else {
                        instance->videoBitrate = VideoBitrate;
                    }
                }

                /**
                 * Retorna a taxa de bits (bit/s) do vídeo a ser capturado.
                 * @return taxa de bits do vídeo destino.
                 */
                int VideoCapture::getVideoBitrate() {
                    return instance->videoBitrate;
                }

                /**
                 * Configura a taxa de bits (bit/s) do áudio do vídeo a ser capturado.
                 * @param audioBitrate taxa de bits do áudio do vídeo destino (valor padrão: 64000).
                 */
                void VideoCapture::setAudioBitrate(int AudioBitrate) {
                    if (AudioBitrate <= 0) {
                        instance->audioBitrate = 64000;
                    } else {
                        instance->audioBitrate = AudioBitrate;
                    }
                }

                /**
                 * Retorna a taxa de bits (bit/s) do áudio do vídeo a ser capturado.
                 * @return taxa de bits do áudio do vídeo destino.
                 */
                int VideoCapture::getAudioBitrate() {
                    return instance->audioBitrate;
                }

                /**
                 * Configura a quantidade de canais do áudio do vídeo a ser capturado.
                 * @param audioChannels quantidade de canais do áudio do vídeo destino (valor padrão: 1).
                 */
                void VideoCapture::setAudioChannels(int AudioChannels) {
                    if (AudioChannels <= 0) {
                        instance->audioChannels = 1;
                    } else {
                        instance->audioChannels = AudioChannels;
                    }
                }

                /**
                 * Retorna a quantidade de canais do áudio do vídeo a ser capturado.
                 * @return quantidade de canais do áudio do vídeo destino.
                 */
                int VideoCapture::getAudioChannels() {
                    return instance->audioChannels;
                }

                /**
                 * Configura a largura (em pixels) do vídeo a ser capturado.
                 * @param width largura do vídeo destino.
                 */
                void VideoCapture::setWidth(int Width) {
                    if (Width > 0) {
                        instance->width = Width;
                    }
                }

                /**
                 * Retorna a largura (em pixels) do vídeo a ser capturado.
                 * @return largura do vídeo destino.
                 */
                int VideoCapture::getWidth() {
                    return instance->width;
                }

                /**
                 * Configura a altura (em pixels) do vídeo a ser capturado.
                 * @param height altura do vídeo destino.
                 */
                void VideoCapture::setHeight(int Height) {
                    if (Height > 0) {
                        instance->height = Height;
                    }
                }

                /**
                 * Retorna a altura (em pixels) do vídeo a ser capturado.
                 * @return altura do vídeo destino.
                 */
                int VideoCapture::getHeight() {
                    return instance->height;
                }

                /**
                 * Configura o formato do vídeo a ser capturado.
                 * @param videoFormat formato do vídeo destino (possíveis valores: mpeg2video, mpegts, mp4, rtp, ...).
                 */
                void VideoCapture::setVideoFormat(const char* VideoFormat) {
                    if (VideoFormat != NULL) {
                        instance->videoFormat = VideoFormat;
                    }
                    else{
                        instance->videoFormat = "avi";
                    }
                }

                /**
                 * Retorna o formato do vídeo a ser capturado.
                 * @return formato do vídeo destino.
                 */
                const char* VideoCapture::getVideoFormat() {
                    return instance->videoFormat.c_str();
                }

                /**
                 * Configura o codec (compressão) do vídeo a ser capturado.
                 * @param videoCodec codec do vídeo destino (possíveis valores: mpeg2video, mpeg4, theora, h264, ...).
                 */
                void VideoCapture::setVideoCodec(const char* VideoCodec) {
                    if (VideoCodec != NULL) {
                        instance->videoCodec = VideoCodec;
                    } else {
                        instance->videoCodec = "mpeg2video";
                    }
                }

                /**
                 * Retorna o codec (compressão) do vídeo a ser capturado.
                 * @return codec do vídeo destino.
                 */
                const char* VideoCapture::getVideoCodec() {
                    return instance->videoCodec.c_str();
                }

                /**
                 * Configura o codec (compressão) do áudio do vídeo a ser capturado.
                 * @param audioCodec codec do áudio do vídeo destino (possíveis valores: mp2, mp3, vorbis, aac, ...).
                 */
                void VideoCapture::setAudioCodec(const char* AudioCodec) {
                    if (AudioCodec != NULL) {
                        instance->audioCodec = AudioCodec;
                    } else {
                        instance->audioCodec = "mp2";
                    }
                }

                /**
                 * Retorna o codec (compressão) do áudio do vídeo a ser capturado.
                 * @return codec do áudio do vídeo destino.
                 */
                const char* VideoCapture::getAudioCodec() {
                    return instance->audioCodec.c_str();
                }

                /**
                 * Adiciona um destino (arquivo local ou rede) do vídeo a ser capturado.
                 * Caso não possa escrever no arquivo de destino, lança exception (FileWriteException)
                 * Caso não possa escrever no fluxo de saída (OutputStream) lança exception (StreamWriteException)
                 * Caso não possa escrever no formato informado (mpeg2video, mpegts, mp4, etc) lança exception (EncondingException)
                 * Caso não possa enviar streaming pela rede lança exception (NetworkingException)
                 * Caso não consiga reconhecer a URI lança exception (URIException)
                 * @param destination destino do vídeo (possíveis valores: "/home/user/file.mpg", "rtp://239.255.255.250:5123").
                 */
                void VideoCapture::addDestination(const char* Destination) throw () {//TODO: adicionar exceções; quebrar destination em protocolo, etc.
                    string* s;
                    s = new string;
                    *s = Destination;
                    instance->destinations.push_back(s);
                    s = NULL;
                }

                /**
                 * Remove um destino (arquivo local ou rede) do vídeo a ser capturado.
                 * Caso não exista o destino informado lança exception (URIException)
                 * @param destination destino do vídeo (possíveis valores: "/home/user/file.mpg", "rtp://239.255.255.250:5123").
                 */
                void VideoCapture::removeDestination(const char* Destination) throw () {
                    for (int i = 0; i < instance->destinations.size(); i++){
                        if (!strcmp(instance->destinations[i]->c_str(), Destination)){
                            string* s;
                            s = instance->destinations[i];
                            instance->destinations.erase(instance->destinations.begin() + i--);
                            delete s;
                            s = NULL;
                        }
                    }
                }

                /**
                 * Adiciona um conjunto de destinos (arquivo local ou rede) do vídeo a ser capturado.
                 * Caso não possa escrever no arquivo de destino, lança exception (FileWriteException)
                 * Caso não possa escrever no fluxo de saída (OutputStream) lança exception (StreamWriteException)
                 * Caso não possa escrever no formato informado (mpeg2video, mpegts, mp4, etc) lança exception (EncondingException)
                 * Caso não possa enviar streaming pela rede lança exception (NetworkingException)
                 * Caso não consiga reconhecer a URI lança exception (URIException)
                 * @param destination conjunto de destinos do vídeo (possíveis valores: "/home/user/file.mpg", "rtp://239.255.255.250:5123").
                 */
                void VideoCapture::addDestination(vector<const char*> Destination) throw () {
                    for (int i = 0; i < Destination.size(); i++){
                        try{
                            addDestination(Destination[i]);
                        } catch(...){
                            throw;
                        }
                    }
                }

                /**
                 * Remove um conjunto de destinos (arquivo local ou rede) do vídeo a ser capturado.
                 * Caso não exista o destino informado lança exception (URIException)
                 * @param destination conjunto de destinos do vídeo (possíveis valores: "/home/user/file.mpg", "rtp://239.255.255.250:5123").
                 */
                void VideoCapture::removeDestination(vector<const char*> Destination) throw () {
                    for (int i = 0; i < Destination.size(); i++){
                        try{
                            removeDestination(Destination[i]);
                        } catch(...){
                            throw;
                        }
                    }
                }

                /**
                 * Retorna os destinos do vídeo a ser capturado.
                 * @return destinos do vídeo.
                 */
                vector<const char*> VideoCapture::getDestinations() {
                    vector<const char*> dest;

                    for (int i = 0; i < instance->destinations.size(); i++){
                        dest.push_back(instance->destinations[i]->c_str());
                    }

                    return dest;
                }

#if OUTPUT_STREAM

                /**
                 * Adiciona um fluxo de saída (OutputStream) à captura.
                 * Quando adicionado algum fluxo de saída (OutputStream), o método write() deve ser usado.
                 * @param stream fluxo de saída.
                 * @return true, se a ação for executada com sucesso, ou false, caso contrário.
                 */
                bool VideoCapture::addOutputStream(OutputStream* Stream) {

                }

                /**
                 * Remove um fluxo de saída (OutputStream) da captura.
                 * @param stream fluxo de saída.
                 * @return true, se a ação for executada com sucesso, ou false, caso contrário.
                 */
                bool VideoCapture::removeOutputStream(OutputStream* Stream) {

                }
#endif

                /**
                 * Construtor.
                 */
                VideoCapture::VideoCapture() {

                }

                /**
                 * Inicia a captura de vídeo do framebuffer conforme os parâmetros préviamente configurados.
                 * @return true, caso a ação ocorra com sucesso, ou false, caso contrário.
                 */
                bool VideoCapture::start() {
                    int i;
                    vector<char*> arguments;
                    char** argv;
                    stringstream sR, aC, aB, sW, sH, fR, vB;
                    string sSR, sAC, sAB, sFR, sVB, resolution;

                    sW << instance->width;
                    sH << instance->height;
                    resolution = sW.str() + "x" + sH.str();

                    fR << instance->frameRate;
                    sFR = fR.str();
                    
                    vB << instance->videoBitrate;
                    sVB = vB.str();

                    sR << instance->sampleRate;
                    sSR = sR.str();

                    aC << instance->audioChannels;
                    sAC = aC.str();

                    aB << instance->audioBitrate;
                    sAB = aB.str();

                    arguments.push_back("ffmpeg");
                    arguments.push_back("-fflags");
                    arguments.push_back("+genpts");
                    arguments.push_back("-re");
                    arguments.push_back("-f");
                    arguments.push_back("fbgrab");
                    arguments.push_back("-r");
                    arguments.push_back((char*) sFR.c_str());
                    arguments.push_back("-i");
                    arguments.push_back(":");
                    arguments.push_back("-f");
                    arguments.push_back("mpeg2video");

                    if (instance->videoFormat == "rtp"){
                        arguments.push_back("-an");
                    }

                    arguments.push_back("-vcodec");
                    arguments.push_back((char*) instance->videoCodec.c_str());
                    arguments.push_back("-sameq");

                    if (instance->videoFormat == "rtp"){
                        arguments.push_back("-f");
                        arguments.push_back((char*) instance->videoFormat.c_str());
                        arguments.push_back((char*) instance->destinations[0]->c_str());
                    }

                    arguments.push_back("-f");
#if HAVE_ALSA_ASOUNDLIB_H 
                    arguments.push_back("alsa");
#else
                    arguments.push_back("oss");
#endif
                    arguments.push_back("-ar");
                    arguments.push_back((char*) sSR.c_str());
                    arguments.push_back("-ac");
                    arguments.push_back((char*) sAC.c_str());
                    arguments.push_back("-ab");
                    arguments.push_back((char*) sAB.c_str());
                    arguments.push_back("-i");
#if HAVE_ALSA_ASOUNDLIB_H 
                    arguments.push_back("default");
#else
                    arguments.push_back("/dev/dsp");
#endif
                    arguments.push_back("-acodec");
                    arguments.push_back("pcm_s16le");

                    if (instance->videoFormat == "rtp"){
                        arguments.push_back("-vn");
                    }

                    arguments.push_back("-acodec");
                    arguments.push_back((char*) instance->audioCodec.c_str());
                    arguments.push_back("-sameq");
                    arguments.push_back("-f");
                    arguments.push_back((char*) instance->videoFormat.c_str());

                    if (instance->videoFormat == "rtp"){
                        arguments.push_back((char*) instance->destinations[1]->c_str());
                        arguments.push_back("-newaudio");
                        i = 39;
                    } else{
                        arguments.push_back((char*) instance->destinations[0]->c_str());
                        i = 33;
                    }

                    argv = &arguments[0];

/*
                    ffmpegCapture.start_recording();
                    try {
                        ffmpegCapture.ffmpeg(i, argv);
                    } catch (...) {
                        throw;
                        return false;
                    }
                    ffmpegCapture.stop_recording();
*/
                    ffmpegCapture.set_recording(i, argv);
                    try {
                        ffmpegCapture.run();
                    } catch (...) {
                        throw;
                        return false;
                    }

                    return true;
                }

                /**
                 * Pára a captura de vídeo do framebuffer.
                 * @return true, caso a ação ocorra com sucesso, ou false, caso contrário.
                 */
                bool VideoCapture::stop() {
                    ffmpegCapture.stop_recording();

                    return true;
                }

                /**
                 * Coloca em execução a captura de vídeo.
                 * Deve ser executado por uma thread paralela.
                 * @param param alguma parâmetro que será utilizado pela thread ou NULL.
                 * @return NULL, caso a ação seja executada com sucesso, ou alguma instância de um objeto, caso necessário.
                 */
                void* VideoCapture::run(void* param) {
                    start();

                    return NULL;
                }

                /**
                 * Percorre o vetor de fluxos de saída e envia os bytes do vídeo que está sendo capturado para cada fluxo (OutputStream).
                 * Este método somente é usado caso não seja especificado um destino para o vídeo a ser capturado.
                 * @param b um byte do vídeo que está sendo capturado.
                 */
                void VideoCapture::write(int b) {

                }

                /**
                 * Percorre o vetor de fluxos de saída e envia os bytes do vídeo que está sendo capturado para cada fluxo (OutputStream).
                 * Este método somente é usado caso não seja especificado um destino para o vídeo a ser capturado.
                 * @param b origem dos dados.
                 * @param len quantidade de bytes da origem.
                 * @return quantidade de bytes escritos ou -1 caso ocorra erro.
                 */
                int VideoCapture::write(const char* b, int len) {

                }

            }
        }
    }
}

