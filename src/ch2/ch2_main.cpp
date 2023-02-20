#include <iostream>

int ch2_testGstUdpSender(int argc, char* argv[]);
int ch2_testGstUdpReceiver(int argc, char* argv[]);
void printUsage();

int main(int argc, char* argv[])
{
  int x=-1;
  if (argc < 2) {
    printUsage();
    return x;
  }
  
  int flag = atoi(argv[1]);
  if( flag==1)
    x = ch2_testGstUdpSender(argc, argv);
  else if (flag == 2)
    x = ch2_testGstUdpReceiver(argc, argv);
  else if (flag == 3)
    x = ch2_testGstUdpSender(argc, argv);
  else if (flag == 4)
    x = ch2_testGstUdpSender(argc, argv);
  else if (flag == 5)
    x = ch2_testGstUdpSender(argc, argv);
  else if (flag == 6)
    x = ch2_testGstUdpSender(argc, argv);
  else  if (flag == 7)
    x = ch2_testGstUdpSender(argc, argv);
  else
    printf("wrong argv.\n");

  return x;
}

void printUsage()
{
#ifdef _win32
const char *cmd = "a.exe";
#else
const char *cmd = "a.out";
#endif
  printf("usage: a.exe <integer>\n");
  printf("  examples:\n");
  printf("  %s 1 -- ch2_testGstUdpSender()\n", cmd);
  printf("  %s 2 -- ch1_ex2_use_main_loop_v1()\n", cmd);
  printf("  %s 3 -- ch1_ex3_use_main_loop_v2()\n", cmd);
  printf("  %s 4 -- ch1_ex4_get_n_set_src_element_properties()\n", cmd);
  printf("  %s 5 -- ch1_ex5_add_an_element()\n", cmd);
  printf("  %s 6 -- ch1_ex6_use_gst_parse_launch()\n", cmd);
  printf("  %s 7 -- ch1_ex7_cpp_ui()\n", cmd);
}
