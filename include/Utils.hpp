#ifndef UTLS_HPP
#define UTILS_HPP


#include "Design.hpp"

// Function to print some stats to verify the parser worked correctly.
void printDesignStats(const Design& design);

/**
 * @brief 将net group信息输出到文件
 * @param design 包含设计数据的Design对象
 * @param output_file 输出文件路径
 */
void outputNetGroupsToFile(const Design& design, const std::string& output_file);

#endif // UTLS_HPP