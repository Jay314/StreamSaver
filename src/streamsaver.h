/*
 * streamsaver.h
 *
 *  Created on: 17.02.2016
 *      Author: jay
 */

#ifndef STREAMSAVER_H_
#define STREAMSAVER_H_

#include <iostream>

#include <ctime>
#include <string>

// general cv functions
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

// for circular frame buffer:
#include <boost/circular_buffer.hpp>


struct ProgramConfig
{
  // [StreamingSource]
	std::string RTP_IP;
	std::string RTP_PORT;

	// [StreamSavings]
	double duration_sec;
	bool show_time;
	double time_font_size;

	// [FileOutput]
	std::string output_folder;
	int fps;
	std::string file_container;

	// [UDPTrigger]
	std::string listen_IP;
	uint listen_PORT;
  std::string answer_IP;
	uint answer_PORT;

	// [RunningOptions]
	bool show_image;
};

// error handling
void error(const char *msg);

// config handling
bool readConfigFile(const std::string &filename, ProgramConfig* config);
// verbose level 0: just basic info; level = 1: all data
void outputConfig(const ProgramConfig &config, const int &verbose_level = 0);

void drawTimeStampInImage(cv::Mat *image);

//void receiveVideoStreamThread(const std::string &address);
void *receiveVideoStreamThread(void *address);

void *showCurrentFrame(void *dummy);

std::string saveVideoInFile(const std::string &filename);

void *ServerUDPCommandListening(void *dummy);

void sendUDPCommand(const std::string &message);

class timer
{
    struct timespec ts_start, ts_end;
    clockid_t id;
    public:
    void start() {
        clock_gettime(id, &ts_start);
    }
    void stop() {
        clock_gettime(id, &ts_end);
    }
    timer(clockid_t clock = CLOCK_MONOTONIC) {
        id = clock;
        start();
    }
    unsigned long usec() {
        unsigned long ns = (ts_end.tv_sec - ts_start.tv_sec) * 1000000000 + ts_end.tv_nsec - ts_start.tv_nsec;
        return ns / 1000;
    }
    unsigned long msec() {
        unsigned long ns = (ts_end.tv_sec - ts_start.tv_sec) * 1000000000 + ts_end.tv_nsec - ts_start.tv_nsec;
        return ns / 1000000;
    }
    friend std::ostream & operator<<(std::ostream& os, const timer &t);
};

std::ostream & operator<<(std::ostream& os, const timer &t)
{
    unsigned long ns = (t.ts_end.tv_sec - t.ts_start.tv_sec) * 1000000000 + t.ts_end.tv_nsec - t.ts_start.tv_nsec;
    std::string ext = "ns";
    if (ns >= 10000) {
        ns /= 1000;
        ext = "us";
    }
    if (ns >= 10000) {
        ns /= 1000;
        ext = "ms";
    }
    if (ns >= 10000) {
        ns /= 1000;
        ext = "s";
    }

    os << ns << " " << ext;

    return os;
}

//////////////////////
// global variables
//////////////////////

ProgramConfig g_config;
// circular buffer for received frames
boost::circular_buffer<cv::Mat> g_circ_frame_buffer;

// program status: 0 waiting for stream, 1 normal run, 2 saving,
uint g_program_status;


#endif /* STREAMSAVER_H_ */
