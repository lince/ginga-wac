#include "../include/AudioCapture.h"

namespace br {
    namespace ufscar {
        namespace lince {
            namespace util {
                AudioCapture* AudioCapture::instance = NULL;
                GingaFFMpeg AudioCapture::ffmpegCapture;

                /**
                 * Factory.
                 * Devolve uma instância de AudioCapture.
                 * @return instância de AudioCapture.
                 */
                AudioCapture* AudioCapture::AudioCaptureFactory() {
                    if (instance == NULL) {
                        instance = new AudioCapture();
                    }
                    return instance;
                }

                /**
                 * Destrutor.
                 */
                AudioCapture::~AudioCapture() {
                    if (instance != NULL) {
                        delete instance;
                    }
                    instance = NULL;
                }

                /**
                 * Configura a taxa de amostragem do áudio a ser capturado.
                 * @param sampleRate taxa de amostragem do áudio destino (valor padrão: 44100).
                 */
                void AudioCapture::setSampleRate(int SampleRate) {
                    if (SampleRate <= 0) {
                        instance->sampleRate = 44100;
                    } else {
                        instance->sampleRate = SampleRate;
                    }
                }

                /**
                 * Retorna a taxa de amostragem do áudio a ser capturado.
                 * @return taxa de amostragem do áudio destino.
                 */
                int AudioCapture::getSampleRate() {
                    return instance->sampleRate;
                }

                /**
                 * Configura a taxa de bits (bit/s) do áudio a ser capturado.
                 * @param audioBitrate taxa de bits do áudio destino (valor padrão: 64000).
                 */
                void AudioCapture::setAudioBitrate(int AudioBitrate) {
                    if (AudioBitrate <= 0) {
                        instance->audioBitrate = 64000;
                    } else {
                        instance->audioBitrate = AudioBitrate;
                    }
                }

                /**
                 * Retorna a taxa de bits (bit/s) do áudio a ser capturado.
                 * @return taxa de bits do áudio destino.
                 */
                int AudioCapture::getAudioBitrate() {
                    return instance->audioBitrate;
                }

                /**
                 * Configura a quantidade de canais do áudio a ser capturado.
                 * @param audioChannels quantidade de canais do áudio destino (valor padrão: 1).
                 */
                void AudioCapture::setAudioChannels(int AudioChannels) {
                    if (AudioChannels <= 0) {
                        instance->audioChannels = 1;
                    } else {
                        instance->audioChannels = AudioChannels;
                    }
                }

                /**
                 * Retorna a quantidade de canais do áudio a ser capturado.
                 * @return quantidade de canais do áudio destino.
                 */
                int AudioCapture::getAudioChannels() {
                    return instance->audioChannels;
                }

                /**
                 * Configura o formato do áudio a ser capturado.
                 * @param audioFormat formato do áudio destino (possíveis valores: mp2, mp3, ogg, rtp, ...).
                 */
                void AudioCapture::setAudioFormat(const char* AudioFormat) {
                    if (AudioFormat != NULL) {
                        instance->audioFormat = AudioFormat;
                    } else {
                        instance->audioFormat = "mp2";
                    }
                }

                /**
                 * Retorna o formato do áudio a ser capturado.
                 * @return formato do áudio destino.
                 */
                const char* AudioCapture::getAudioFormat() {
                    return instance->audioFormat.c_str();
                }

                /**
                 * Configura o codec (compressão) do áudio a ser capturado.
                 * @param audioCodec codec do áudio destino (possíveis valores: mp2, mp3, vorbis, aac, ...).
                 */
                void AudioCapture::setAudioCodec(const char* AudioCodec) {
                    if (AudioCodec != NULL) {
                        instance->audioCodec = AudioCodec;
                    } else {
                        instance->audioCodec = "mp2";
                    }
                }

                /**
                 * Retorna o codec (compressão) do áudio a ser capturado.
                 * @return codec do áudio destino.
                 */
                const char* AudioCapture::getAudioCodec() {
                    return instance->audioCodec.c_str();
                }

                /**
                 * Adiciona um destino (arquivo local ou rede) do áudio a ser capturado.
                 * Caso não possa escrever no arquivo de destino, lança exception (FileWriteException)
                 * Caso não possa escrever no fluxo de saída (OutputStream) lança exception (StreamWriteException)
                 * Caso não possa escrever no formato informado (mp2, mp3, ogg, etc) lança exception (EncondingException)
                 * Caso não possa enviar streaming pela rede lança exception (NetworkingException)
                 * Caso não consiga reconhecer a URI lança exception (URIException)
                 * @param destination destino do áudio (possíveis valores: "/home/user/file.mp3", "rtp://239.255.255.250:5123").
                 */
                void AudioCapture::addDestination(const char* destination) throw () {//TODO: adicionar exceções; quebrar destination em protocolo, etc.
                    string* s;
                    s = new string;
                    *s = destination;
                    instance->destinations.push_back(s);
                    s = NULL;
                }

                /**
                 * Remove um destino (arquivo local ou rede) do áudio a ser capturado.
                 * Caso não exista o destino informado lança exception (URIException)
                 * @param destination destino do áudio (possíveis valores: "/home/user/file.mp3", "rtp://239.255.255.250:5123").
                 */
                void AudioCapture::removeDestination(const char* destination) throw () {
                    for (int i = 0; i < instance->destinations.size(); i++){
                        if (!strcmp(instance->destinations[i]->c_str(), destination)){
                            string* s;
                            s = instance->destinations[i];
                            instance->destinations.erase(instance->destinations.begin() + i--);
                            delete s;
                            s = NULL;
                        }
                    }
                }

                /**
                 * Adiciona um conjunto de destinos (arquivo local ou rede) do áudio a ser capturado.
                 * Caso não possa escrever no arquivo de destino, lança exception (FileWriteException)
                 * Caso não possa escrever no fluxo de saída (OutputStream) lança exception (StreamWriteException)
                 * Caso não possa escrever no formato informado (mp2, mp3, ogg, etc) lança exception (EncondingException)
                 * Caso não possa enviar streaming pela rede lança exception (NetworkingException)
                 * Caso não consiga reconhecer a URI lança exception (URIException)
                 * @param destination conjunto de destinos do áudio (possíveis valores: "/home/user/file.mp3", "rtp://239.255.255.250:5123").
                 */
                void AudioCapture::addDestination(vector<const char*> destination) throw () {
                    for (int i = 0; i < destination.size(); i++){
                        try{
                            addDestination(destination[i]);
                        } catch(...){
                            throw;
                        }
                    }
                }

                /**
                 * Remove um conjunto de destinos (arquivo local ou rede) do áudio a ser capturado.
                 * Caso não consiga reconhecer a URI lança exception (URIException)
                 * @param destination conjunto de destinos do áudio (possíveis valores: "/home/user/file.mp3", "rtp://239.255.255.250:5123").
                 */
                void AudioCapture::removeDestination(vector<const char*> destination) throw () {
                    for (int i = 0; i < destination.size(); i++){
                        try{
                            removeDestination(destination[i]);
                        } catch(...){
                            throw;
                        }
                    }
                }

                /**
                 * Retorna os destinos do áudio a ser capturado.
                 * @return destinos do áudio.
                 */
                vector<const char*> AudioCapture::getDestinations() {
                    vector<const char*> dest;

                    for (int i = 0; i < instance->destinations.size(); i++){
                        dest.push_back(instance->destinations[i]->c_str());
                    }

                    return dest;
/*//FIXME / XXX: pode ser removido.
                    vector<const char*> dest;
                    string* str;
                    char* ch;

                    for (int i = 0; i < instance->destinations.size(); i++){
                        str = new string;
                        *str = instance->destinations[i]->c_str();
                        ch = new char[str->size() + 1];
                        strcpy(ch, str->c_str());
                        std::cout << endl << ch;
                        dest.push_back(ch);
                    }

                    return dest;
*/
                }

#if OUTPUT_STREAM
                /**
                 * Adiciona um fluxo de saída (OutputStream) à captura.
                 * Quando adicionado algum fluxo de saída (OutputStream), o método write() deve ser usado.
                 * @param stream fluxo de saída.
                 * @return true, se a ação for executada com sucesso, ou false, caso contrário.
                 */
                bool AudioCapture::addOutputStream(OutputStream* stream) {

                }

                /**
                 * Remove um fluxo de saída (OutputStream) da captura.
                 * @param stream fluxo de saída.
                 * @return true, se a ação for executada com sucesso, ou false, caso contrário.
                 */
                bool AudioCapture::removeOutputStream(OutputStream* stream) {

                }
#endif

                /**
                 * Construtor.
                 */
                AudioCapture::AudioCapture() {
                }

                /**
                 * Inicia a captura de áudio conforme os parâmetros préviamente configurados.
                 * Caso não possa escrever no arquivo de destino, lança exception (FileWriteException)
                 * Caso não possa escrever no fluxo de saída (OutputStream) lança exception (StreamWriteException)
                 * Caso não possa escrever no formato informado (jpg, png, etc) lança exception (EncondingException)
                 * Caso não possa enviar streaming pela rede lança exception (NetworkingException)
                 * @return true, caso a ação ocorra com sucesso, ou false, caso contrário.
                 */
                bool AudioCapture::start() {
                    ffmpegCapture.start_recording();
                    int i;
                    vector<char*> arguments;
                    char** argv;
                    stringstream sR, aC, aB;
                    string sSR, sAC, sAB;

                    sR << instance->sampleRate;
                    sSR = sR.str();

                    aC << instance->audioChannels;
                    sAC = aC.str();

                    aB << instance->audioBitrate;
                    sAB = aB.str();

                    arguments.push_back("ffmpeg");
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
                    arguments.push_back("-vn");
                    arguments.push_back("-acodec");
                    arguments.push_back((char*) instance->audioCodec.c_str());
                    arguments.push_back("-sameq");
                    arguments.push_back("-f");
                    arguments.push_back((char*) instance->audioFormat.c_str());
                    arguments.push_back((char*) instance->destinations[0]->c_str());

                    argv = &arguments[0];
                    i = 20;

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
                }

                /**
                 * Pára a captura de áudio.
                 * @return true, caso a ação ocorra com sucesso, ou false, caso contrário.
                 */
                bool AudioCapture::stop() {
                    ffmpegCapture.stop_recording();

                    return true;
                }

                /**
                 * Coloca em execução a captura de áudio.
                 * Deve ser executado por uma thread paralela.
                 * @param param alguma parâmetro que será utilizado pela thread ou NULL.
                 * @return NULL, caso a ação seja executada com sucesso, ou alguma instância de um objeto, caso necessário.
                 */
                void* AudioCapture::run(void* param) {
                    start();

                    return NULL;
                }

                /**
                 * Percorre o vetor de fluxos de saída e envia os bytes do áudio que está sendo capturado para cada fluxo (OutputStream).
                 * Este método somente é usado caso não seja especificado um destino para o áudio a ser capturado.
                 * @param b um byte do áudio que está sendo capturado.
                 */
                void AudioCapture::write(int b) {

                }

                /**
                 * Percorre o vetor de fluxos de saída e envia os bytes do áudio que está sendo capturado para cada fluxo (OutputStream).
                 * Este método somente é usado caso não seja especificado um destino para o áudio a ser capturado.
                 * @param b origem dos dados.
                 * @param len quantidade de bytes da origem.
                 * @return quantidade de bytes escritos ou -1 caso ocorra erro.
                 */
                int AudioCapture::write(const char* b, int len) {

                }


            }
        }
    }
}


