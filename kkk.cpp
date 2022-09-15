#include <iostream>
#include <iomanip>

int main(){
    std::cout << "Files processed: " << 1 <<'\n';
    std::cout << std::setfill ('-') << std::setw(100) << '\n';
    std::cout << "Filename" << std::setfill (' ') << std::setw(18) << "Language" << std::setw(18) << "Comments" << std::setw(18) << "Blank" << std::setw(15) << "Code" << std::setw(16) << "All" << '\n';
    std::cout << std::setfill ('-') << std::setw(100) << '\n';
    std::cout << "main.cpp" << std::setfill (' ') << std::setw(18) << "cpp" << std::setw(18) << 1 << std::setw(18) << 1 << std::setw(15) << 1 << std::setw(16) << 1 << '\n';
  





  return 0;
}