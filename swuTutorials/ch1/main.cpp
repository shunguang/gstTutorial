#include <iostream>

int ch1_ex1_use_bus_msg(int argc, char* argv[]);
int ch1_ex2_use_main_loop_v1(int argc, char* argv[]);
int ch1_ex2_use_main_loop_v2(int argc, char* argv[]);
int ch1_ex3_get_n_set_src_element_properties(int argc, char* argv[]);
int ch1_ex4_add_an_element(int argc, char* argv[]);
int ch1_ex5_use_gst_parse_launch(int argc, char* argv[]);
int ch1_ex6_cpp_ui(int argc, char* argv[]);
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
    x = ch1_ex1_use_bus_msg(argc, argv);
  else if (flag == 2)
    x = ch1_ex2_use_main_loop_v1(argc, argv);
  else if (flag == 3)
    x = ch1_ex2_use_main_loop_v2(argc, argv);
  else if (flag == 4)
    x = ch1_ex3_get_n_set_src_element_properties(argc, argv);
  else if (flag == 5)
    x = ch1_ex4_add_an_element(argc, argv);
  else if (flag == 6)
    x = ch1_ex5_use_gst_parse_launch(argc, argv);
  else  if (flag == 7)
    x = ch1_ex6_cpp_ui(argc, argv);
  else
    printf("wrong argv.\n");

  return x;
}

void printUsage()
{
  printf("usage: a.exe <integer>\n");
  printf("  examples:\n");
  printf("  a.exe 1 -- ch1_ex1_use_bus_msg()\n");
  printf("  a.exe 2 -- ch1_ex2_use_main_loop_v1()\n");
  printf("  a.exe 3 -- ch1_ex2_use_main_loop_v2()\n");
  printf("  a.exe 4 -- ch1_ex3_get_n_set_src_element_properties()\n");
  printf("  a.exe 5 -- ch1_ex4_add_an_element()\n");
  printf("  a.exe 6 -- ch1_ex5_use_gst_parse_launch()\n");
  printf("  a.exe 7 -- ch1_ex6_cpp_ui()\n");
}
