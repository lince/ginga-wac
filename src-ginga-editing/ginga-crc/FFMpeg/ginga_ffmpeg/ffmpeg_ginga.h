#ifndef __FFMPEG_GINGA_H__
#define __FFMPEG_GINGA_H__

extern "C" {
/* needed for usleep() */
#define _XOPEN_SOURCE 600

//Necessário definir para uso de inteiros de 64 bits.
#define _XOPEN_SOURCE 600
#define __STDC_CONSTANT_MACROS
#include <stdint.h>

#undef INT64_MAX
#define INT64_MAX INT64_C(9223372036854775807)

#include "config.h"
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>
#include <unistd.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include "libavcodec/opt.h"
#include <libavcodec/avcodec.h>
#include <libavcodec/audioconvert.h>
#include <libavcodec/colorspace.h>
#include <libavutil/fifo.h>
#include <libavutil/avstring.h>
#include <libavformat/os_support.h>

#if HAVE_SYS_RESOURCE_H
#include <sys/types.h>
#include <sys/resource.h>
#elif HAVE_GETPROCESSTIMES
#include <windows.h>
#endif

#if HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#if HAVE_TERMIOS_H
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <termios.h>
#elif HAVE_CONIO_H
#include <conio.h>
#endif
#undef time //needed because HAVE_AV_CONFIG_H is defined on top
#include <time.h>

#include "cmdutils.h"

#undef NDEBUG
#include <assert.h>

#undef exit

#include <pthread.h>
}

#include "Exceptions.h"

namespace br {
    namespace ufscar {
        namespace lince {
            namespace util {

#define MAX_FILES 20

                /* select an input stream for an output stream */
                typedef struct AVStreamMap {
                    int file_index;
                    int stream_index;
                    int sync_file_index;
                    int sync_stream_index;
                } AVStreamMap;

                /** select an input file for an output file */
                typedef struct AVMetaDataMap {
                    int out_file;
                    int in_file;
                } AVMetaDataMap;

#define DEFAULT_PASS_LOGFILENAME_PREFIX "ffmpeg2pass"

                struct AVInputStream;

                typedef struct AVOutputStream {
                    int file_index; /* file index */
                    int index; /* stream index in the output file */
                    int source_index; /* AVInputStream index */
                    AVStream *st; /* stream in the output file */
                    int encoding_needed; /* true if encoding needed for this stream */
                    int frame_number;
                    /* input pts and corresponding output pts
                       for A/V sync */
                    //double sync_ipts;        /* dts from the AVPacket of the demuxer in second units */
                    struct AVInputStream *sync_ist; /* input stream to sync against */
                    int64_t sync_opts; /* output frame counter, could be changed to some true timestamp */ //FIXME look at frame_number
                    /* video only */
                    int video_resample;
                    AVFrame pict_tmp; /* temporary image for resampling */
                    struct SwsContext *img_resample_ctx; /* for image resampling */
                    int resample_height;
                    int resample_width;
                    int resample_pix_fmt;

                    /* full frame size of first frame */
                    int original_height;
                    int original_width;

                    /* cropping area sizes */
                    int video_crop;
                    int topBand;
                    int bottomBand;
                    int leftBand;
                    int rightBand;

                    /* cropping area of first frame */
                    int original_topBand;
                    int original_bottomBand;
                    int original_leftBand;
                    int original_rightBand;

                    /* padding area sizes */
                    int video_pad;
                    int padtop;
                    int padbottom;
                    int padleft;
                    int padright;

                    /* audio only */
                    int audio_resample;
                    ReSampleContext *resample; /* for audio resampling */
                    int reformat_pair;
                    AVAudioConvert *reformat_ctx;
                    AVFifoBuffer *fifo; /* for compression: one audio fifo per codec */
                    FILE *logfile;
                } AVOutputStream;

                typedef struct AVInputStream {
                    int file_index;
                    int index;
                    AVStream *st;
                    int discard; /* true if stream data should be discarded */
                    int decoding_needed; /* true if the packets must be decoded in 'raw_fifo' */
                    int64_t sample_index; /* current sample */

                    int64_t start; /* time when read started */
                    int64_t next_pts; /* synthetic pts for cases where pkt.pts
                                is not defined */
                    int64_t pts; /* current pts */
                    int is_start; /* is 1 at the start and after a discontinuity */
                } AVInputStream;

                typedef struct AVInputFile {
                    int eof_reached; /* true if eof reached */
                    int ist_index; /* index of first stream in ist_table */
                    int buffer_size; /* current total buffer size */
                    int nb_streams; /* nb streams we are aware of */
                } AVInputFile;

                class
                GingaFFMpeg {
                private:
                    static volatile bool recording;

                    static pthread_mutex_t record_mutex;
                    static pthread_t record_thread;

                    static int number_arguments;
                    static char** arguments;

                    //FFMpeg data members:
                    static char *last_asked_format;
                    static AVFormatContext *input_files[MAX_FILES];
                    static int64_t input_files_ts_offset[MAX_FILES];
                    static double input_files_ts_scale[MAX_FILES][MAX_STREAMS];
                    static AVCodec *input_codecs[MAX_FILES*MAX_STREAMS];
                    static int nb_input_files;
                    static int nb_icodecs;

                    static AVFormatContext *output_files[MAX_FILES];
                    static AVCodec *output_codecs[MAX_FILES*MAX_STREAMS];
                    static int nb_output_files;
                    static int nb_ocodecs;

                    static AVStreamMap stream_maps[MAX_FILES*MAX_STREAMS];
                    static int nb_stream_maps;

                    static AVMetaDataMap meta_data_maps[MAX_FILES];
                    static int nb_meta_data_maps;

                    static int frame_width;
                    static int frame_height;
                    static float frame_aspect_ratio;
                    static enum PixelFormat frame_pix_fmt;
                    static enum SampleFormat audio_sample_fmt;
                    static int frame_padtop;
                    static int frame_padbottom;
                    static int frame_padleft;
                    static int frame_padright;
                    static int padcolor[3]; /* default to black */
                    static int frame_topBand;
                    static int frame_bottomBand;
                    static int frame_leftBand;
                    static int frame_rightBand;
                    static int max_frames[4];
                    static AVRational frame_rate;
                    static float video_qscale;
                    static uint16_t *intra_matrix;
                    static uint16_t *inter_matrix;
                    static const char *video_rc_override_string;
                    static int video_disable;
                    static int video_discard;
                    static char *video_codec_name;
                    static int video_codec_tag;
                    static int same_quality;
                    static int do_deinterlace;
                    static int top_field_first;
                    static int me_threshold;
                    static int intra_dc_precision;
                    static int loop_input;
                    static int loop_output;
                    static int qp_hist;

                    static int intra_only;
                    static int audio_sample_rate;
                    static int64_t channel_layout;
#define QSCALE_NONE -99999
                    static float audio_qscale;
                    static int audio_disable;
                    static int audio_channels;
                    static char *audio_codec_name;
                    static int audio_codec_tag;
                    static char *audio_language;

                    static int subtitle_disable;
                    static char *subtitle_codec_name;
                    static char *subtitle_language;
                    static int subtitle_codec_tag;

                    static float mux_preload;
                    static float mux_max_delay;

                    static int64_t recording_time;
                    static int64_t start_time;
                    static int64_t rec_timestamp;
                    static int64_t input_ts_offset;
                    static int file_overwrite;
                    static int metadata_count;
                    static AVMetadataTag *metadata;
                    static int do_benchmark;
                    static int do_hex_dump;
                    static int do_pkt_dump;
                    static int do_psnr;
                    static int do_pass;
                    static char *pass_logfilename_prefix;
                    static int audio_stream_copy;
                    static int video_stream_copy;
                    static int subtitle_stream_copy;
                    static int video_sync_method;
                    static int audio_sync_method;
                    static float audio_drift_threshold;
                    static int copy_ts;
                    static int opt_shortest;
                    static int video_global_header;
                    static char *vstats_filename;
                    static FILE *vstats_file;
                    static int opt_programid;
                    static int copy_initial_nonkeyframes;

                    static int rate_emu;

                    static int video_channel;
                    static char *video_standard;

                    static int audio_volume;

                    static int exit_on_error;
                    static int using_stdin;
                    static int verbose;
                    static int thread_count;
                    static int q_pressed;
                    static int64_t video_size;
                    static int64_t audio_size;
                    static int64_t extra_size;
                    static int nb_frames_dup;
                    static int nb_frames_drop;
                    static int input_sync;
                    static uint64_t limit_filesize;
                    static int force_fps;

                    static int pgmyuv_compatibility_hack;
                    static float dts_delta_threshold;

                    static unsigned int sws_flags;

                    static int64_t timer_start;

                    static uint8_t *audio_buf;
                    static uint8_t *audio_out;
                    static uint8_t *audio_out2;

                    static short *samples;

                    static AVBitStreamFilterContext *video_bitstream_filters;
                    static AVBitStreamFilterContext *audio_bitstream_filters;
                    static AVBitStreamFilterContext *subtitle_bitstream_filters;
                    static AVBitStreamFilterContext *bitstream_filters[MAX_FILES][MAX_STREAMS];

                    static OptionDef options[];

#if HAVE_TERMIOS_H

                    /* init terminal so that we can grab keys */
                    static struct termios oldtty;
#endif

                    static void term_exit(void);

                    static volatile int received_sigterm;

                    static void
                    sigterm_handler(int sig);

                    static void term_init(void);

                    /* read a key without blocking */
                    static int read_key(void);

                    static int decode_interrupt_cb(void);

                    static int av_exit(int ret);

                    static int read_ffserver_streams(AVFormatContext *s, const char *filename);

                    static double
                    get_sync_ipts(const AVOutputStream *ost);

                    static void write_frame(AVFormatContext *s, AVPacket *pkt, AVCodecContext *avctx, AVBitStreamFilterContext *bsfc);

#define MAX_AUDIO_PACKET_SIZE (128 * 1024)

                    static void do_audio_out(AVFormatContext *s,
                            AVOutputStream *ost,
                            AVInputStream *ist,
                            unsigned char *buf, int size);

                    static void pre_process_video_frame(AVInputStream *ist, AVPicture *picture, void **bufp);

                    /* we begin to correct av delay at this threshold */
#define AV_DELAY_MAX 0.100

                    static void do_subtitle_out(AVFormatContext *s,
                            AVOutputStream *ost,
                            AVInputStream *ist,
                            AVSubtitle *sub,
                            int64_t pts);

                    static int bit_buffer_size;
                    static uint8_t *bit_buffer;

                    static void do_video_out(AVFormatContext *s,
                            AVOutputStream *ost,
                            AVInputStream *ist,
                            AVFrame *in_picture,
                            int *frame_size);

                    static double psnr(double d);

                    static void do_video_stats(AVFormatContext *os, AVOutputStream *ost,
                            int frame_size);

                    static void print_report(AVFormatContext **output_files,
                            AVOutputStream **ost_table, int nb_ostreams,
                            int is_last_report);

                    /* pkt = NULL means EOF (needed to flush decoder buffers) */
                    static int output_packet(AVInputStream *ist, int ist_index,
                            AVOutputStream **ost_table, int nb_ostreams,
                            const AVPacket *pkt);

                    static void print_sdp(AVFormatContext **avc, int n);

                    static int stream_index_from_inputs(AVFormatContext **input_files,
                            int nb_input_files,
                            AVInputFile *file_table,
                            AVInputStream **ist_table,
                            enum CodecType type,
                            int programid);

                    /*
                     * The following code is the main loop of the file converter
                     */
                    static int av_encode(AVFormatContext **output_files,
                            int nb_output_files,
                            AVFormatContext **input_files,
                            int nb_input_files,
                            AVStreamMap *stream_maps, int nb_stream_maps);

                    static void opt_format(const char *arg);

                    static void opt_video_rc_override_string(const char *arg);

                    static int opt_me_threshold(const char *opt, const char *arg);

                    static int opt_verbose(const char *opt, const char *arg);

                    static int opt_frame_rate(const char *opt, const char *arg);

                    static int opt_bitrate(const char *opt, const char *arg);

                    static void opt_frame_crop_top(const char *arg);

                    static void opt_frame_crop_bottom(const char *arg);

                    static void opt_frame_crop_left(const char *arg);

                    static void opt_frame_crop_right(const char *arg);

                    static void opt_frame_size(const char *arg);

                    static void opt_pad_color(const char *arg);

                    static void opt_frame_pad_top(const char *arg);

                    static void opt_frame_pad_bottom(const char *arg);


                    static void opt_frame_pad_left(const char *arg);


                    static void opt_frame_pad_right(const char *arg);

                    static void list_fmts(void (*get_fmt_string)(char *buf, int buf_size, int fmt), int nb_fmts);

                    static void opt_frame_pix_fmt(const char *arg);

                    static void opt_frame_aspect_ratio(const char *arg);

                    static int opt_metadata(const char *opt, const char *arg);

                    static void opt_qscale(const char *arg);

                    static void opt_top_field_first(const char *arg);

                    static int opt_thread_count(const char *opt, const char *arg);

                    static void opt_audio_sample_fmt(const char *arg);

                    static int opt_audio_rate(const char *opt, const char *arg);

                    static int opt_audio_channels(const char *opt, const char *arg);

                    static void opt_video_channel(const char *arg);

                    static void opt_video_standard(const char *arg);

                    static void opt_codec(int *pstream_copy, char **pcodec_name,
                            int codec_type, const char *arg);

                    static void opt_audio_codec(const char *arg);

                    static void opt_audio_tag(const char *arg);

                    static void opt_video_tag(const char *arg);
                    static void opt_video_codec(const char *arg);

                    static void opt_subtitle_codec(const char *arg);

                    static void opt_subtitle_tag(const char *arg);

                    static void opt_map(const char *arg);

                    static void opt_map_meta_data(const char *arg);

                    static void opt_input_ts_scale(const char *arg);

                    static int opt_recording_time(const char *opt, const char *arg);

                    static int opt_start_time(const char *opt, const char *arg);

                    static int opt_rec_timestamp(const char *opt, const char *arg);

                    static int opt_input_ts_offset(const char *opt, const char *arg);

                    static enum CodecID find_codec_or_die(const char *name, int type, int encoder);

                    static void opt_input_file(const char *filename);

                    static void check_audio_video_sub_inputs(int *has_video_ptr, int *has_audio_ptr,
                            int *has_subtitle_ptr);

                    static void new_video_stream(AVFormatContext *oc);

                    static void new_audio_stream(AVFormatContext *oc);

                    static void new_subtitle_stream(AVFormatContext *oc);

                    static void opt_new_audio_stream(void);

                    static void opt_new_video_stream(void);

                    static void opt_new_subtitle_stream(void);

                    static void opt_output_file(const char *filename);

                    /* same option as mencoder */
                    static void opt_pass(const char *pass_str);

                    static int64_t getutime(void);

                    static void parse_matrix_coeffs(uint16_t *dest, const char *str);

                    static void opt_inter_matrix(const char *arg);

                    static void opt_intra_matrix(const char *arg);

                    /**
                     * Trivial log callback.
                     * Only suitable for show_help and similar since it lacks prefix handling.
                     */
                    static void log_callback_help(void* ptr, int level, const char* fmt, va_list vl);

                    static void show_help(void);

                    static void opt_target(const char *arg);

                    static void opt_vstats_file(const char *arg);

                    static void opt_vstats(void);

                    static int opt_bsf(const char *opt, const char *arg);

                    static int opt_preset(const char *opt, const char *arg);

                public:
                    GingaFFMpeg();
                    ~GingaFFMpeg();
                    static int ffmpeg(int argc, char **argv);

                    static void set_recording(int argc, char **argv);

                    static void reset_recording();

                    static void run();

                    static void start_recording();

                    static void stop_recording();

                    static void init();

                    static void clean();

                    static void* run_record_thread(void* data);

                };
            }
        }
    }
}


#endif //__FFMPEG_GINGA_H__

