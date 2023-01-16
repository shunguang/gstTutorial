#ifndef  CH1_CPP_EXAMPLE_H 
#define  CH1_CPP_EXAMPLE_H 

#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <string>
#include <gst/gst.h>
struct Ch1CppExampleCfg{
  std::string text{ "Front Door" };
  int dx{50};
  int dy{-50};
  std::string fontdesc{ "Sans, 48" };
};

class Ch1CppExample {
public:
  Ch1CppExample( const Ch1CppExampleCfg &cfg );
  Ch1CppExample( const Ch1CppExample& rhs )=delete;
  Ch1CppExample& operator=( const Ch1CppExample& rhs )=delete;
  ~Ch1CppExample();

  void start();
  void stop();

private:
  static gboolean cbMsgHandle(GstBus* bus, GstMessage* message, gpointer user_data);
  int	 mainLoop();
  gboolean mainLoopHelperCreateElements();
  gboolean mainLoopHelperLinkElements();

protected:
  Ch1CppExampleCfg _cfg;

public:
  GMainLoop* _loop{ NULL };
  GstElement* _pipeline{ NULL };
  GstElement* _videoSrc{ NULL };
  GstElement* _textOverlay{ NULL };
  GstElement* _videoSink{NULL};
  GstBus* _bus = NULL;

  bool    _isStarted{ false };
  std::unique_ptr<std::thread>  _appThread;

};
#endif

