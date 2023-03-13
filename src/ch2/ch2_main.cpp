#include <iostream>

int ch2_testGstRtspSender(int argc, char* argv[]);
int ch2_testGstRtspReceiver(int argc, char* argv[]);
static void ch2_printUsage();

int main(int argc, char* argv[])
{
  int x=-1;
  if (argc < 2) {
    ch2_printUsage();
    return x;
  }
  
  int flag = atoi(argv[1]);
  if( flag==1)
    x = ch2_testGstRtspSender(argc, argv);
  else if (flag == 2)
    x = ch2_testGstRtspReceiver(argc, argv);
  else if (flag == 3)
    x = ch2_testGstRtspSender(argc, argv);
  else if (flag == 4)
    x = ch2_testGstRtspSender(argc, argv);
  else if (flag == 5)
    x = ch2_testGstRtspSender(argc, argv);
  else if (flag == 6)
    x = ch2_testGstRtspSender(argc, argv);
  else  if (flag == 7)
    x = ch2_testGstRtspSender(argc, argv);
  else
    printf("wrong argv.\n");

  return x;
}

void ch2_printUsage()
{
#ifdef _WIN32
const char *cmd = "ch2.exe";
#else
const char *cmd = "ch2.out";
#endif
  printf("usage: a.exe <integer>\n");
  printf("  examples:\n");
  printf("  %s 1 -- ch2_testGstRstpSender()\n", cmd);
  printf("  %s 2 -- ch2_testGstRtspReceiver()\n", cmd);
  printf("  %s 3 -- aa()\n", cmd);
  printf("  %s 4 -- aa()\n", cmd);
  printf("  %s 5 -- aa()\n", cmd);
  printf("  %s 6 -- aa()\n", cmd);
  printf("  %s 7 -- aa()\n", cmd);
}
