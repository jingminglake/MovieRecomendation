#ifndef READFILEUTIL__H
#define READFILEUTIL__H
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
class ReadFileUtil{
 public:
  static void readFileByLine(const std::string& sFileName,  std::vector<std::string>& infoList) {
    //   std::cout <<  "ReadFileByLine" << sFileName << std::endl;
    std::ifstream fin(sFileName.c_str());
    if(!fin) {
      throw std::string("Can not open file" + sFileName);
    }
    std::string s;
    while (getline(fin, s))
    {
      // std::cout << "Read from file: " << s << std::endl;
      infoList.push_back(s);
    }
    //infoList
  }
};

#endif
