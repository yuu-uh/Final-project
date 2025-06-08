#include <iostream>
#include <enet/enet.h>
int main(){
  if(enet_initialize()==0){
    std::cout<<"ENet initialize success\n";
    enet_deinitialize();
    return 0;
  }
  std::cerr<<"ENet intitialize failed\n";
  return 1;
}