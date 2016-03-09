//============================================================================
// Name        : streamsaver.cpp
// Author      : Juergen Pfadt
// Version     :
// Copyright   : StreamSaver can not be copied and/or distributed without the
//	             express permission of Juergen Pfadt (streamsaver@pfadt.info)
// Description : Saving a video network stream on disk for a couple of seconds
//============================================================================

#include "streamsaver.h"

#include <iostream>

#include <time.h>
#include <string.h>


// for UDP server function
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>

// for string and string conversion operations
#include <sstream>
#include <string>
#include <iostream>

using namespace std;
using namespace cv;

// for reading config file
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

// for threads (receiving and saving stream)
#include <pthread.h>

void error(const char *msg)
{
  std::cout << msg << std::endl;
  exit(0);
}

void errorSystem(const char *msg)
{
    perror(msg);
    exit(0);
}


void outputConfig(const ProgramConfig &config, const int &verbose_level)
{
  std::cout << "using RTP stream from IP " << config.RTP_IP << ":" << config.RTP_PORT << std::endl;
  if (verbose_level > 0)
  {
    std::cout << "StreamSavings: duration = " << config.duration_sec << " sec, show_time = " << config.show_time << ", time_font_size = " << config.time_font_size << std::endl;
    std::cout << "FileOutput: output_folder = " << config.output_folder << ", fps = " << config.fps << ", file_container = " << config.file_container << std::endl;
  }
  std::cout << "listening on IP " << config.listen_IP << ":" << config.listen_PORT << " for incoming UDP commands and answering on IP " << config.answer_IP << ":" << config.answer_PORT << "..." << std::endl;
}

bool readConfigFile(const std::string &filename, ProgramConfig* config)
{
  boost::property_tree::ptree pt;
  boost::property_tree::ini_parser::read_ini(filename, pt);
  try
  {
    // [StreamingSource]
    config->RTP_IP = pt.get<std::string>("StreamingSource.RTP_IP");
    config->RTP_PORT = pt.get<std::string>("StreamingSource.RTP_PORT");

    // [StreamSavings]
    config->duration_sec = pt.get<double>("StreamSavings.duration_sec");
    config->show_time = pt.get<bool>("StreamSavings.show_time");
    config->time_font_size = pt.get<double>("StreamSavings.time_font_size");

    // [FileOutput]
    config->output_folder = pt.get<std::string>("FileOutput.output_folder");
    config->fps = pt.get<int>("FileOutput.fps");
    config->file_container = pt.get<std::string>("FileOutput.file_container");

    // [UDPTrigger]
    config->listen_IP = pt.get<std::string>("UDPTrigger.listen_IP");
    config->listen_PORT = pt.get<uint>("UDPTrigger.listen_PORT");
    config->answer_IP = pt.get<std::string>("UDPTrigger.answer_IP");
    config->answer_PORT = pt.get<uint>("UDPTrigger.answer_PORT");

    // [RunningOptions]
    config->show_image = pt.get<bool>("RunningOptions.show_image");
  }
  catch (...)
  {
    outputConfig(*config, 1);
    error("ERROR: cannot read from config file!\n");
  }
  return true;
}

void drawTimeStampInImage(cv::Mat *image)
{
  // date and time info
  time_t t;
  char time_text[24];

  // put current date/time in image:
  time(&t);

  strncpy(time_text, ctime(&t), 24);
  cv::putText(*image, time_text, cv::Point2f(10,30), cv::FONT_HERSHEY_DUPLEX, g_config.time_font_size,  cv::Scalar(0,0,255,255), 1.5);
}

void *receiveVideoStreamThread(void *address)
{
  // create VideoCapture object
  cv::VideoCapture capture_stream;
  while(!capture_stream.isOpened())
  {
    std::cout << "trying to open stream from " << *(std::string*)address << "..." << std::endl;
    try
    {
      capture_stream.open(*(std::string*)address);
    }
    catch (...)
    {
      // some errors or just waiting...
    }
  }
  cv::Mat frame;
  while (true)
  {
    try
    {
      capture_stream >> frame;
      // save image only if not empty and program not in saving mode
      if (!frame.empty() && g_program_status != 2)
      {
        if (g_config.show_time)
        {
          drawTimeStampInImage(&frame);
        }
        g_circ_frame_buffer.push_back(frame.clone());
        g_program_status = 1;
      }
    }
    catch (...)
    {
      // some errors
    }
  }
  return NULL;
}

void *showCurrentFrame(void *dummy)
{
  cv::namedWindow("replay");
  cv::Mat frame;
  while (true)
  {
    if (g_circ_frame_buffer.size() > 0)
    {
      frame = g_circ_frame_buffer[g_circ_frame_buffer.size()-1];
    }
    if (!frame.empty())
    {
      try
      {
        cv::imshow("replay", frame);
      }
      catch (...)
      {
        // some errors with display functions
      }
    }
    else
    {
      // std::cout << "no frame received..." << std::endl;
    }
    cv::waitKey(40);
  }
  return NULL;
}

std::string saveVideoInFile(const std::string &filename)
{
  try
  {
    std::cout << "saving video file...";

    // Evaluate the sum of all elements
    uint sum = g_circ_frame_buffer.size();

    // save output video in one file
    cv::VideoWriter outputFile;
    outputFile.open(g_config.output_folder + std::string("/") + filename + std::string(".") + g_config.file_container,
      CV_FOURCC('H','2','6','4'), g_config.fps, g_circ_frame_buffer[0].size(), true);
  //    outputFile.open("test.avi", CV_FOURCC(0, 0, 0, 0), 15, g_circ_frame_buffer[0].size(), true);
    outputFile.set(cv::VIDEOWRITER_PROP_QUALITY, 100);

    for (uint i = 0; i < sum; i++)
    {
      outputFile.write(g_circ_frame_buffer[i]);
    }
    outputFile.release();
    std::cout << " finished!" << std::endl;

    return filename + std::string(".") + g_config.file_container;
  }
  catch (...)
  {
    return std::string();
  }
}

void *ServerUDPCommandListening(void *dummy)
{
  int sock, length, n;
  socklen_t fromlen;
  struct sockaddr_in server;
  struct sockaddr_in from;
  char buf[1024];

  sock=socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0)
  {
    errorSystem("Opening socket");
  }
  length = sizeof(server);
  bzero(&server,length);
  server.sin_family=AF_INET;

  // create hostname
  struct hostent *hp;
  hp = gethostbyname(g_config.listen_IP.c_str());
  if (hp==0)
  {
    errorSystem("Unknown host");
  }
  bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);

  server.sin_port = htons(g_config.listen_PORT);
  if (bind(sock, (struct sockaddr *)&server, length)<0)
  {
    errorSystem("binding");
  }
  fromlen = sizeof(struct sockaddr_in);
  while (true)
  {
    n = recvfrom(sock, buf, 1024, 0, (struct sockaddr *)&from, &fromlen);
    std::string answer;

    if (n < 0) errorSystem("recvfrom");
    if (std::string(buf).compare(0,6,"status") == 0)
    {
      // status request
      std::cout << "status request via udp..." << std::endl;
      outputConfig(g_config, 1);
      std::ostringstream ss;
      ss << g_program_status;
      answer = ss.str();
    }
    if (std::string(buf).compare(0,4,"save") == 0)
    {
      // save request
      char filename[60];
      std::size_t length = std::string(buf).copy(filename,std::string(buf).length()-4,4);
      if (length > 0)
      {
        filename[length]='\0';
        std::cout << "save request for filename " << filename << "..." << std::endl;
        // now, save the video in file and change program status to saving:
        g_program_status = 2;
        answer = saveVideoInFile(filename);
        g_program_status = 1;
      }
      else
      {
        std::cout << "WARNING: no filename given! use format like 'save20160208'." << std::endl;
      }
    }
    // convert answer and send packet if answer exist
    if (!answer.empty())
    {
      sendUDPCommand(answer);
    }
  }
  return NULL;
}

void sendUDPCommand(const std::string &message)
{
  int sock, n;
  unsigned int length;

  struct sockaddr_in server;
  char buffer[256];

  sock= socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0)
  {
    errorSystem("socket");
  }

  server.sin_family = AF_INET;

  // create hostname
  struct hostent *hp;
  hp = gethostbyname(g_config.answer_IP.c_str());
  if (hp==0)
  {
    errorSystem("Unknown host");
  }
  bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);

  server.sin_port = htons(g_config.answer_PORT);
  length=sizeof(struct sockaddr_in);
  bzero(buffer,256);

  // sending packet
  const char *cstr_answer = message.c_str();
  n=sendto(sock, cstr_answer, strlen(cstr_answer),0,(const struct sockaddr *)&server,length);
  if (n < 0)
  {
    errorSystem("Sendto");
  }
  std::cout << "sending answer[" << strlen(cstr_answer) << "]: " << cstr_answer << std::endl;
  close(sock);
}

int main(int argc, char** argv)
{
  g_program_status = 0;

  // loading configuration from config file
  if (argc < 2)
  {
    error("ERROR: no config file provided.\nuse: streamsaver settings.ini");
  }
  readConfigFile(argv[1], &g_config);
  outputConfig(g_config);

  // Initialize variables
  // buffer size = duration * fps
  g_circ_frame_buffer.set_capacity(int(g_config.duration_sec * g_config.fps));
  std::cout << "buffer_size: " << int(g_config.duration_sec * g_config.fps) << std::endl;

  // create address of streaming source
  std::string address;
  address = std::string("rtp://") + g_config.RTP_IP + std::string(":") + std::string(g_config.RTP_PORT);


  // creating threads
  pthread_t thread_1, thread_2, thread_3;

  // thread for VideoCapture starting
  pthread_create (&thread_1, NULL, receiveVideoStreamThread, &address);

  // thread for ImageShowing depending on config starting
  if (g_config.show_image)
  {
    pthread_create (&thread_2, NULL, showCurrentFrame, NULL);
  }

  // thread for UDP command server starting
  pthread_create (&thread_3, NULL, ServerUDPCommandListening, NULL);

  pthread_join (thread_1, NULL);
  pthread_join (thread_2, NULL);
  pthread_join (thread_3, NULL);

  return 0;
}

/*

# These are the codecs I've tried so far
codec = cv2.cv.CV_FOURCC('I','4','2','0')
#codec = cv2.cv.CV_FOURCC('A','V','C','1')
#codec = cv2.cv.CV_FOURCC('Y','U','V','1')
#codec = cv2.cv.CV_FOURCC('P','I','M','1')
#codec = cv2.cv.CV_FOURCC('M','J','P','G')
#codec = cv2.cv.CV_FOURCC('M','P','4','2')
#codec = cv2.cv.CV_FOURCC('D','I','V','3')
#codec = cv2.cv.CV_FOURCC('D','I','V','X')
#codec = cv2.cv.CV_FOURCC('U','2','6','3')
#codec = cv2.cv.CV_FOURCC('I','2','6','3')
#codec = cv2.cv.CV_FOURCC('F','L','V','1')
#codec = cv2.cv.CV_FOURCC('H','2','6','4')
#codec = cv2.cv.CV_FOURCC('A','Y','U','V')
#codec = cv2.cv.CV_FOURCC('I','U','Y','V')
#codec = -1
#codec = 0

*/
