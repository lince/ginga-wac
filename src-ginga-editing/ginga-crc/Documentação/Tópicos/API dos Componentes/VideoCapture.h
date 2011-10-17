#include <string>
#include <vector>

#include "../../Exceptions/include/Exceptions.h"

using namespace std;

namespace br {
    namespace ufscar {
        namespace lince {
                namespace util {

                        class VideoCapture {
                        private:
                            int frameRate;
                            int sampleRate;
                            int videoBitrate;
                            int audioBitrate;
                            int audioChannels;
                            int width;
                            int height;
                            bool isStreaming;
                            string videoFormat;
                            string videoCodec;
                            string audioCodec;
                            vector<string *> destinations;
                            vector<OutputStream *> outputStreams;

                        public:

                            /**
                             * Factory.
                             * Devolve uma instância de VideoCapture.
                             * @return instância de VideoCapture.
                             */
                            static VideoCapture* VideoCaptureFactory();

                            /**
                             * Destrutor.
                             */
                            ~VideoCapture();

                            /**
                             * Configura a taxa de quadros por segundo (fps) do vídeo a ser capturado.
                             * @param frameRate taxa de quadros (fps) do vídeo destino (valor padrão: 25).
                             */
                            void setFrameRate(int FrameRate);

                            /**
                             * Retorna a taxa de quadros por segundo (fps) do vídeo a ser capturado.
                             * @return taxa de quadros (fps) do vídeo destino.
                             */
                            int getFrameRate();

                            /**
                             * Configura a taxa de amostragem do áudio do vídeo a ser capturado.
                             * @param sampleRate taxa de amostragem do áudio do vídeo destino (valor padrão: 44100).
                             */
                            void setSampleRate(int SampleRate);

                            /**
                             * Retorna a taxa de amostragem do áudio do vídeo a ser capturado.
                             * @return taxa de amostragem do áudio do vídeo destino.
                             */
                            int getSampleRate();

                            /**
                             * Configura a taxa de bits (bit/s) do vídeo a ser capturado.
                             * @param videoBitrate taxa de bits do vídeo destino (valor padrão: 200000).
                             */
                            void setVideoBitrate(int VideoBitrate);

                            /**
                             * Retorna a taxa de bits (bit/s) do vídeo a ser capturado.
                             * @return taxa de bits do vídeo destino.
                             */
                            int getVideoBitrate();

                            /**
                             * Configura a taxa de bits (bit/s) do áudio do vídeo a ser capturado.
                             * @param audioBitrate taxa de bits do áudio do vídeo destino (valor padrão: 64000).
                             */
                            void setAudioBitrate(int AudioBitrate);

                            /**
                             * Retorna a taxa de bits (bit/s) do áudio do vídeo a ser capturado.
                             * @return taxa de bits do áudio do vídeo destino.
                             */
                            int getAudioBitrate();

                            /**
                             * Configura a quantidade de canais do áudio do vídeo a ser capturado.
                             * @param audioChannels quantidade de canais do áudio do vídeo destino (valor padrão: 1).
                             */
                            void setAudioChannels(int AudioChannels);

                            /**
                             * Retorna a quantidade de canais do áudio do vídeo a ser capturado.
                             * @return quantidade de canais do áudio do vídeo destino.
                             */
                            int getAudioChannels();

                            /**
                             * Configura a largura (em pixels) do vídeo a ser capturado.
                             * @param width largura do vídeo destino.
                             */
                            void setWidth(int Width);

                            /**
                             * Retorna a largura (em pixels) do vídeo a ser capturado.
                             * @return largura do vídeo destino.
                             */
                            int getWidth();

                            /**
                             * Configura a altura (em pixels) do vídeo a ser capturado.
                             * @param height altura do vídeo destino.
                             */
                            void setHeight(int Height);

                            /**
                             * Retorna a altura (em pixels) do vídeo a ser capturado.
                             * @return altura do vídeo destino.
                             */
                            int getHeight();

                            /**
                             * Configura o formato do vídeo a ser capturado.
                             * @param videoFormat formato do vídeo destino (possíveis valores: mpeg2video, mpegts, mp4, rtp, ...).
                             */
                            void setVideoFormat(const char* VideoFormat);

                            /**
                             * Retorna o formato do vídeo a ser capturado.
                             * @return formato do vídeo destino.
                             */
                            const char* getVideoFormat();

                            /**
                             * Configura o codec (compressão) do vídeo a ser capturado.
                             * @param videoCodec codec do vídeo destino (possíveis valores: mpeg2video, mpeg4, theora, h264, ...).
                             */
                            void setVideoCodec(const char* VideoCodec);

                            /**
                             * Retorna o codec (compressão) do vídeo a ser capturado.
                             * @return codec do vídeo destino.
                             */
                            const char* getVideoCodec();

                            /**
                             * Configura o codec (compressão) do áudio do vídeo a ser capturado.
                             * @param audioCodec codec do áudio do vídeo destino (possíveis valores: mp2, mp3, vorbis, aac, ...).
                             */
                            void setAudioCodec(const char* AudioCodec);

                            /**
                             * Retorna o codec (compressão) do áudio do vídeo a ser capturado.
                             * @return codec do áudio do vídeo destino.
                             */
                            const char* getAudioCodec();

                            /**
                             * Adiciona um destino (arquivo local ou rede) do vídeo a ser capturado.
                             * Caso não possa escrever no arquivo de destino, lança exception (FileWriteException)
                             * Caso não possa escrever no fluxo de saída (OutputStream) lança exception (StreamWriteException)
                             * Caso não possa escrever no formato informado (mpeg2video, mpegts, mp4, etc) lança exception (EncondingException)
                             * Caso não possa enviar streaming pela rede lança exception (NetworkingException)
                             * Caso não consiga reconhecer a URI lança exception (URIException)
                             * @param destination destino do vídeo (possíveis valores: "/home/user/file.mpg", "rtp://239.255.255.250:5123").
                             */
                            void addDestination(const char* Destination) throw();

                            /**
                             * Remove um destino (arquivo local ou rede) do vídeo a ser capturado.
                             * Caso não exista o destino informado lança exception (URIException)
                             * @param destination destino do vídeo (possíveis valores: "/home/user/file.mpg", "rtp://239.255.255.250:5123").
                             */
                            void removeDestination(const char* Destination) throw();

                            /**
                             * Adiciona um conjunto de destinos (arquivo local ou rede) do vídeo a ser capturado.
                             * Caso não possa escrever no arquivo de destino, lança exception (FileWriteException)
                             * Caso não possa escrever no fluxo de saída (OutputStream) lança exception (StreamWriteException)
                             * Caso não possa escrever no formato informado (mpeg2video, mpegts, mp4, etc) lança exception (EncondingException)
                             * Caso não possa enviar streaming pela rede lança exception (NetworkingException)
                             * Caso não consiga reconhecer a URI lança exception (URIException)
                             * @param destination conjunto de destinos do vídeo (possíveis valores: "/home/user/file.mpg", "rtp://239.255.255.250:5123").
                             */
                            void addDestination(vector<const char*> Destination) throw();

                            /**
                             * Remove um conjunto de destinos (arquivo local ou rede) do vídeo a ser capturado.
                             * Caso não exista o destino informado lança exception (URIException)
                             * @param destination conjunto de destinos do vídeo (possíveis valores: "/home/user/file.mpg", "rtp://239.255.255.250:5123").
                             */
                            void removeDestination(vector<const char*> Destination) throw();

                            /**
                             * Retorna os destinos do vídeo a ser capturado.
                             * @return destinos do vídeo.
                             */
                            vector<const char*> getDestinations();

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

                        private:

                            /**
                             * Construtor.
                             */
                            VideoCapture();

                            /**
                             * Inicia a captura de vídeo do framebuffer conforme os parâmetros préviamente configurados.
                             * @return true, caso a ação ocorra com sucesso, ou false, caso contrário.
                             */
                            bool start();

                            /**
                             * Pára a captura de vídeo do framebuffer.
                             * @return true, caso a ação ocorra com sucesso, ou false, caso contrário.
                             */
                            bool stop();

                            /**
                             * Coloca em execução a captura de vídeo.
                             * Deve ser executado por uma thread paralela.
                             * @param param alguma parâmetro que será utilizado pela thread ou NULL.
                             * @return NULL, caso a ação seja executada com sucesso, ou alguma instância de um objeto, caso necessário.
                             */
                            void* run(void* param);

                            /**
                             * Percorre o vetor de fluxos de saída e envia os bytes do vídeo que está sendo capturado para cada fluxo (OutputStream).
                             * Este método somente é usado caso não seja especificado um destino para o vídeo a ser capturado.
                             * @param b um byte do vídeo que está sendo capturado.
                             */
                            void write(int b);

                            /**
                             * Percorre o vetor de fluxos de saída e envia os bytes do vídeo que está sendo capturado para cada fluxo (OutputStream).
                             * Este método somente é usado caso não seja especificado um destino para o vídeo a ser capturado.
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
