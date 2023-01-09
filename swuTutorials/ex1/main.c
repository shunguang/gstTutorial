int ex1_v1_use_bus_msg(int argc, char* argv[]);
int ex1_v2_use_main_loop(int argc, char* argv[]);
//int ex1_v3_set_src_element_params(int argc, char* argv[]);
int main(int argc, char* argv[])
{
  int x;
  
  x = ex1_v1_use_bus_msg(argc, argv);
  //x = ex1_v2_use_main_loop(argc, argv);

  return x;
}
