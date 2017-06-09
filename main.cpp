#include <iostream>
#include <string>
#include "manager.h"

int main(int, char*[]) {
   try {
     Manager manager;
     manager.predictUnrated();
   }
   catch (const std::string& msg) { std::cout << msg << std::endl; }
   catch(...) {
      std::cout << "oops! someone threw an exception!" << std::endl;
   }
   return 0;
}
