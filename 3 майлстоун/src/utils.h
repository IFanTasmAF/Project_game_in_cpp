#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <cctype>
#include <iostream>
#include <limits>
#include <random> 
#include <string>

namespace utils {

// Преобразование строки в нижний регистр
inline std::string toLower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return s;
}

// Ожидание нажатия Enter
inline void waitForEnter() {
  std::cout << "Нажмите Enter для продолжения...";
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Генератор случайных чисел (инициализируется один раз)
inline int getRandomInt(int min, int max) {
  static std::mt19937 rng(std::random_device{}());
  std::uniform_int_distribution<int> dist(min, max);
  return dist(rng);
}

}  // namespace utils

#endif
