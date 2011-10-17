#ifndef __AUDIO_CAPTURE_H__
#define __AUDIO_CAPTURE_H__

#include <string>
#include <vector>
#include <sstream>

#include <ffmpeg_ginga.h>

#define OUTPUT_STREAM 0
#if OUTPUT_STREAM
  #include <ginga/capture/OutputStream.h>
#endif

using namespace std;

namespace br {
    namespace ufscar {
        namespace lince {
                namespace util {

                        class AudioCapture {
                        private:
                            int sampleRate;
                            int audioBitrate;
                            int audioChannels;
                            bool isStreaming;
                            string audioFormat;
                            string audioCodec;
                            vector<string *> destinations;
#if OUTPUT_STREAM
                            vector<OutputStream *> outputStreams;
#endif
                            static AudioCapture* instance;
                            static GingaFFMpeg ffmpegCapture;

                        public:

                            /**
                             * Factory.
                             * Devolve uma instância de AudioCapture.
                             * @return instância de AudioCapture.
                             */
                            static AudioCapture* AudioCaptureFactory();

                            /**
                             * Destrutor.
                             */
                            ~AudioCapture();

                            /**
                             * Configura a taxa de amostragem do áudio a ser capturado.
                             * @param sampleRate taxa de amostragem do áudio destino (valor padrão: 44100).
                             */
                            void setSampleRate(int SampleRate);

                            /**
                             * Retorna a taxa de amostragem do áudio a ser capturado.
                             * @return taxa de amostragem do áudio destino.
                             */
                            int getSampleRate();

                            /**
                             * Configura a taxa de bits (bit/s) do áudio a ser capturado.
                             * @param audioBitrate taxa de bits do áudio destino (valor padrão: 64000).
                             */
                            void setAudioBitrate(int AudioBitrate);

                            /**
                             * Retorna a taxa de bits (bit/s) do áudio a ser capturado.
                             * @return taxa de bits do áudio destino.
                             */
                            int getAudioBitrate();

                            /**
                             * Configura a quantidade de canais do áudio a ser capturado.
                             * @param audioChannels quantidade de canais do áudio destino (valor padrão: 1).
                             */
                            void setAudioChannels(int AudioChannels);

                            /**
                             * Retorna a quantidade de canais do áudio a ser capturado.
                             * @return quantidade de canais do áudio destino.
                             */
                            int getAudioChannels();

                            /**
                             * Configura o formato do áudio a ser capturado.
                             * @param audioFormat formato do áudio destino (possíveis valores: mp2, mp3, ogg, rtp, ...).
                             */
                            void setAudioFormat(const char* AudioFormat);

                            /**
                             * Retorna o formato do áudio a ser capturado.
                             * @return formato do áudio destino.
                             */
                            const char* getAudioFormat();

                            /**
                             * Configura o codec (compressão) do áudio a ser capturado.
                             * @param audioCodec codec do áudio destino (possíveis valores: mp2, mp3, vorbis, aac, ...).
                             */
                            void setAudioCodec(const char* AudioCodec);

                            /**
                             * Retorna o codec (compressão) do áudio a ser capturado.
                             * @return codec do áudio destino.
                             */
                            const char* getAudioCodec();

                            /**
                             * Adiciona um destino (arquivo local ou rede) do áudio a ser capturado.
                             * Caso não possa escrever no arquivo de destino, lança exception (FileWriteException)
                             * Caso não possa escrever no fluxo de saída (OutputStream) lança exception (StreamWriteException)
                             * Caso não possa escrever no formato informado (mp2, mp3, ogg, etc) lança exception (EncondingException)
                             * Caso não possa enviar streaming pela rede lança exception (NetworkingException)
                             * Caso não consiga reconhecer a URI lança exception (URIException)
                             * @param destination destino do áudio (possíveis valores: "/home/user/file.mp3", "rtp://239.255.255.250:5123").
                             */
                            void addDestination(const char* Destination) throw();

                            /**
                             * Remove um destino (arquivo local ou rede) do áudio a ser capturado.
                             * Caso não exista o destino informado lança exception (URIException)
                             * @param destination destino do áudio (possíveis valores: "/home/user/file.mp3", "rtp://239.255.255.250:5123").
                             */
                            void removeDestination(const char* Destination) throw();

                            /**
                             * Adiciona um conjunto de destinos (arquivo local ou rede) do áudio a ser capturado.
                             * Caso não possa escrever no arquivo de destino, lança exception (FileWriteException)
                             * Caso não possa escrever no fluxo de saída (OutputStream) lança exception (StreamWriteException)
                             * Caso não possa escrever no formato informado (mp2, mp3, ogg, etc) lança exception (EncondingException)
                             * Caso não possa enviar streaming pela rede lança exception (NetworkingException)
                             * Caso não consiga reconhecer a URI lança exception (URIException)
                             * @param destination conjunto de destinos do áudio (possíveis valores: "/home/user/file.mp3", "rtp://239.255.255.250:5123").
                             */
                            void addDestination(vector<const char*> Destination) throw();

                            /**
                             * Remove um conjunto de destinos (arquivo local ou rede) do áudio a ser capturado.
                             * Caso não consiga reconhecer a URI lança exception (URIException)
                             * @param destination conjunto de destinos do áudio (possíveis valores: "/home/user/file.mp3", "rtp://239.255.255.250:5123").
                             */
                            void removeDestination(vector<const char*> Destination) throw();

                            /**
                             * Retorna os destinos do áudio a ser capturado.
                             * @return destinos do áudio.
                             */
                            vector<const char*> getDestinations();

#if OUTPUT_STREAM
                            /**
                             * Adiciona um fluxo de saída (OutputStream) à captura.
                             * Quando adicionado algum fluxo de saída (OutputStream), o método write() deve ser usado.
                             * @param stream fluxo de saída.
                             * @return true, se a ação for executada com sucesso, ou false, caso contrário.
                             */
                            bool addOutputStream(OutputStream* Stream);

                            /**
                             * Remove um fluxo de saída (OutputStream) da captura.
                             * @param stream fluxo de saída.
                             * @return true, se a ação for executada com sucesso, ou false, caso contrário.
                             */
                            bool removeOutputStream(OutputStream* Stream);
#endif

                        private:

                            /**
                             * Construtor.
                             */
                            AudioCapture();

public://XXX FIXME: remover.
                            /**
                             * Inicia a captura de áudio conforme os parâmetros préviamente configurados.
                             * Caso não possa escrever no arquivo de destino, lança exception (FileWriteException)
                             * Caso não possa escrever no fluxo de saída (OutputStream) lança exception (StreamWriteException)
                             * Caso não possa escrever no formato informado (jpg, png, etc) lança exception (EncondingException)
                             * Caso não possa enviar streaming pela rede lança exception (NetworkingException)
                             * @return true, caso a ação ocorra com sucesso, ou false, caso contrário.
                             */
                            bool start();

                            /**
                             * Pára a captura de áudio.
                             * @return true, caso a ação ocorra com sucesso, ou false, caso contrário.
                             */
                            bool stop();

                            /**
                             * Coloca em execução a captura de áudio.
                             * Deve ser executado por uma thread paralela.
                             * @param param alguma parâmetro que será utilizado pela thread ou NULL.
                             * @return NULL, caso a ação seja executada com sucesso, ou alguma instância de um objeto, caso necessário.
                             */
                            void* run(void* param);

                            /**
                             * Percorre o vetor de fluxos de saída e envia os bytes do áudio que está sendo capturado para cada fluxo (OutputStream).
                             * Este método somente é usado caso não seja especificado um destino para o áudio a ser capturado.
                             * @param b um byte do áudio que está sendo capturado.
                             */
                            void write(int b);

                            /**
                             * Percorre o vetor de fluxos de saída e envia os bytes do áudio que está sendo capturado para cada fluxo (OutputStream).
                             * Este método somente é usado caso não seja especificado um destino para o áudio a ser capturado.
                             * @param b origem dos dados.
                             * @param len quantidade de bytes da origem.
                             * @return quantidade de bytes escritos ou -1 caso ocorra erro.
                             */
                            int write(const char* b, int len);
                            
                        };

            }
        }
    }
}

#endif //__AUDIO_CAPTURE_H__

