#include "Utils.hpp"

// Function to print some stats to verify the parser worked correctly.
void printDesignStats(const Design& design) {
    std::cout << "--- Design Statistics ---" << std::endl;

    // Print FPGA info
    const auto& fpgas = design.getFpgas();
    std::cout << "Total FPGAs: " << fpgas.size() << std::endl;
    for (const auto& fpga : fpgas) {
        if (fpga.id != -1) {
            std::cout << "  FPGA F" << fpga.id << ": Max IO = " << fpga.max_io 
                      << ", Mapped Nodes = " << fpga.nodes.size() << std::endl;
        }
    }

    // Print Node info
    const auto& nodes = design.getNodes();
    std::cout << "\nTotal Logical Nodes: " << nodes.size() << std::endl;
    // Example: print info for a few nodes
    int count = 0;
    for (const auto& pair : nodes) {
        if (count < 5 && pair.second.fpga) {
             std::cout << "  Node g" << pair.second.id << " is on FPGA F" 
                       << pair.second.fpga->id << std::endl;
             count++;
        } else if (count >= 5) {
            break;
        }
    }


    // Print Net info
    const auto& nets = design.getNets();
    std::cout << "\nTotal Nets: " << nets.size() << std::endl;
    if (!nets.empty()) {
        const auto& first_net = nets[0];
        if (first_net.source) {
            std::cout << "  Example Net " << first_net.id << ": Source g" << first_net.source->id
                      << " -> " << first_net.sinks.size() << " sinks." << std::endl;
        }
    }

    // Print Topology info
    const auto& topo = design.getTopology();
    std::cout << "\nTopology Matrix (" << topo.size() << "x" << topo.size() << "):" << std::endl;
    for (size_t i = 0; i < topo.size(); ++i) {
        std::cout << "  F" << i + 1 << ": ";
        for (size_t j = 0; j < topo[i].size(); ++j) {
            std::cout << topo[i][j] << (j == topo[i].size() - 1 ? "" : ", ");
        }
        std::cout << std::endl;
    }

    std::cout << "-------------------------" << std::endl;
}

/**
 * @brief 将net group信息输出到文件
 * @param design 包含设计数据的Design对象
 * @param output_file 输出文件路径
 */
void outputNetGroupsToFile(const Design& design, const std::string& output_file) {
    try {
        // 获取net group信息
        auto net_groups = design.groupNetsByFpgaConnection();
        
        // 打开输出文件
        std::ofstream out_file(output_file);
        if (!out_file.is_open()) {
            std::cerr << "Error: Could not open output file: " << output_file << std::endl;
            return;
        }
        
        // 写入文件头
        out_file << "# Net Groups by FPGA Connection Pattern\n";
        out_file << "# Format: Group [group_number]: Source_FPGA -> Sink_FPGA1(count),Sink_FPGA2(count) -> [net_id1, net_id2, ...]\n\n";
        
        // 写入每个net group的信息
        for (size_t i = 0; i < net_groups.size(); ++i) {
            const auto& group = net_groups[i];
            
            // 获取第一个net的连接模式作为代表
            if (!group.empty()) {
                int first_net_id = group[0];
                const auto& nets = design.getNets();
                
                // 查找第一个net以获取连接模式
                for (const auto& net : nets) {
                    if (net.id == first_net_id) {
                        // 获取源FPGA
                        std::string src_fpga = "unknown";
                        if (net.source && net.source->fpga) {
                            src_fpga = "F" + std::to_string(net.source->fpga->id);
                        }
                        
                        // 获取目标FPGA及其节点数量
                        std::string sink_fpgas = "unknown";
                        if (!net.sinks.empty()) {
                            // 统计每个FPGA上的节点数量
                            std::map<int, int> fpga_counts;
                            for (const auto& sink : net.sinks) {
                                if (sink && sink->fpga) {
                                    // 跳过与源FPGA相同的节点
                                    if (sink->fpga->id == net.source->fpga->id) {
                                        continue;
                                    }
                                    fpga_counts[sink->fpga->id]++;
                                }
                            }
                            
                            // 构建输出字符串
                            if (!fpga_counts.empty()) {
                                sink_fpgas = "";
                                for (const auto& pair : fpga_counts) {
                                    if (!sink_fpgas.empty()) {
                                        sink_fpgas += ",";
                                    }
                                    sink_fpgas += "F" + std::to_string(pair.first) + " (" + std::to_string(pair.second) + ")";
                                }
                            }
                        }
                        
                        // 写入连接模式
                        out_file << "Group [" << i + 1 << "]: " << src_fpga << " -> " << sink_fpgas << " -> [";
                        
                        // 写入net ID列表
                        for (size_t j = 0; j < group.size(); ++j) {
                            out_file << "net" << group[j];
                            if (j < group.size() - 1) {
                                out_file << ", ";
                            }
                        }
                        out_file << "]\n";
                        break;
                    }
                }
            }
        }
        
        // 写入统计信息
        out_file << "\n# Statistics:\n";
        out_file << "# Total net groups: " << net_groups.size() << "\n";
        out_file << "# Total nets: ";
        
        int total_nets = 0;
        for (const auto& group : net_groups) {
            total_nets += group.size();
        }
        out_file << total_nets << "\n";
        
        // 关闭文件
        out_file.close();
        
        std::cout << "Net groups information has been written to: " << output_file << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error writing net groups to file: " << e.what() << std::endl;
    }
}