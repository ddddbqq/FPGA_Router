#include "Design.hpp"
#include "FastParser.hpp"
#include <stdexcept>
#include <vector>
#include <utility>
#include <fstream>
#include <algorithm>
#include <map>

// Logical parsing order:
// 1. Info: To know how many FPGAs exist and their properties.
// 2. FPGA Mapping: To create Node objects and associate them with the FPGAs.
// 3. Nets: To connect the already-created Node objects.
// 4. Topo: To build the connectivity matrix for FPGAs.

void Design::loadInfo(const std::string& filename) {
    FastParser parser(filename);
    
    int max_fpga_id = 0;
    std::vector<std::pair<int, int>> fpgas_data;

    while (!parser.isEOF()) {
        parser.skipWhitespace();
        if (parser.isEOF()) break;
        
        int fpga_id = parser.parseId('F');
        int max_io = parser.parseInt();
        
        if (fpga_id > 0) {
            fpgas_data.push_back({fpga_id, max_io});
            if (fpga_id > max_fpga_id) {
                max_fpga_id = fpga_id;
            }
        }
    }

    fpgas_.resize(max_fpga_id);
    for(const auto& data : fpgas_data){
        // Ensure vector is large enough, then place FPGA at index id-1.
        if (data.first > 0) {
             fpgas_[data.first - 1] = FPGA(data.first, data.second);
        }
    }
}

void Design::loadFpgaMapping(const std::string& filename) {
    if (fpgas_.empty()) {
        throw std::logic_error("Design Error: Please load .info file before .fpga.out file.");
    }

    FastParser parser(filename);
    while (!parser.isEOF()) {
        parser.skipWhitespace();
        if (parser.isEOF()) break;
        
        int fpga_id = parser.parseId('F');
        parser.skipChar(':');

        if(fpga_id <= 0 || fpga_id > fpgas_.size()){
            continue; // Skip line if FPGA ID is invalid.
        }
        FPGA* current_fpga = &fpgas_[fpga_id - 1];

        // This loop parses all logical nodes belonging to the current FPGA.
        // It breaks when it peeks a character that indicates a new line ('F') or EOF.
        while (!parser.isEOF()) {
            char next_char = parser.peekNextNonWhitespaceChar();
            if (next_char == 'F' || next_char == '\0') {
                break; // Found the start of the next FPGA line or end of file.
            }

            int node_id = parser.parseId('g');
            
            if (nodes_.find(node_id) == nodes_.end()) {
                nodes_[node_id] = Node(node_id);
            }
            nodes_[node_id].fpga = current_fpga;
            current_fpga->nodes.push_back(&nodes_[node_id]);
        }
    }
}

void Design::loadNets(const std::string& filename) {
    if (nodes_.empty()) {
        throw std::logic_error("Design Error: Please load .fpga.out file before .net file.");
    }

    FastParser parser(filename);
    int net_id_counter = 1;

    while (!parser.isEOF()) {
        parser.skipWhitespace();
        if (parser.isEOF()) break;
        
        Net current_net;
        current_net.id = net_id_counter++;

        int source_node_id = parser.parseId('g');
        current_net.source = &nodes_.at(source_node_id);

        current_net.weight = parser.parseInt();

        // This loop parses all sink nodes for the current net.
        // It assumes all remaining items on the line are sinks until a new line or EOF.
        while (!parser.isEOF()) {
            char next_char = parser.peekNextNonWhitespaceChar();
            // In the .net file, the next line will also start with 'g'.
            // A simple way to distinguish is to assume sinks are continuous on one line.
            // When skipWhitespace() hits a newline, we can infer the sinks have ended.
            // This is a heuristic. A more robust parser might handle newlines explicitly.
            // For now, if the next char is not 'g', we assume the sinks list has ended.
            if (next_char != 'g') {
                break;
            }
            
            int sink_node_id = parser.parseId('g');
            current_net.sinks.push_back(&nodes_.at(sink_node_id));
        }
        nets_.push_back(current_net);
    }
}

void Design::loadTopo(const std::string& filename) {
    if (fpgas_.empty()) {
        throw std::logic_error("Design Error: Please load .info file before .topo file.");
    }

    FastParser parser(filename);
    size_t num_fpgas = fpgas_.size();
    topology_.resize(num_fpgas, std::vector<int>(num_fpgas));

    while (!parser.isEOF()) {
        parser.skipWhitespace();
        if (parser.isEOF()) break;

        int fpga_id = parser.parseId('F');
        parser.skipChar(':');

        if(fpga_id > 0 && (size_t)fpga_id <= num_fpgas){
            for (size_t i = 0; i < num_fpgas; ++i) {
                topology_[fpga_id - 1][i] = parser.parseInt();
                if (i < num_fpgas - 1) {
                    parser.skipChar(',');
                }
            }
        }
    }
}

/**
 * @brief Generate visualization data.
 */
void Design::generateVisualizationData(const std::string& filename) const {
    if (fpgas_.empty() || nets_.empty() || topology_.empty()) {
        throw std::logic_error("Visualization Error: Not all data has been loaded.");
    }

    size_t num_fpgas = fpgas_.size();
    std::vector<std::vector<int>> logical_demand(num_fpgas, std::vector<int>(num_fpgas, 0));

    // Calculate logical demand between each pair of FPGAs.
    for (const auto& net : nets_) {
        if (!net.source || !net.source->fpga) continue;
        int src_fpga_idx = net.source->fpga->id - 1;

        for (const auto& sink : net.sinks) {
            if (!sink || !sink->fpga) continue;
            int sink_fpga_idx = sink->fpga->id - 1;

            if (src_fpga_idx != sink_fpga_idx) {
                logical_demand[src_fpga_idx][sink_fpga_idx]++;
                // Since it's an undirected representation of demand, increment both ways.
                logical_demand[sink_fpga_idx][src_fpga_idx]++;
            }
        }
    }
    
    std::ofstream json_file(filename);
    if (!json_file.is_open()) {
        throw std::runtime_error("Visualization Error: Cannot open file for writing: " + filename);
    }

    // --- Write JSON data ---
    json_file << "{\n";

    // Write nodes (FPGAs)
    json_file << "  \"nodes\": [\n";
    for (size_t i = 0; i < num_fpgas; ++i) {
        json_file << "    {\"id\": " << (i + 1) << "}";
        if (i < num_fpgas - 1) {
            json_file << ",";
        }
        json_file << "\n";
    }
    json_file << "  ],\n";

    // Write physical links
    json_file << "  \"physical_links\": [\n";
    bool first_link = true;
    for (size_t i = 0; i < num_fpgas; ++i) {
        for (size_t j = i + 1; j < num_fpgas; ++j) {
            if (topology_[i][j] > 0) {
                if (!first_link) {
                    json_file << ",\n";
                }
                json_file << "    {\"source\": " << (i + 1) << ", \"target\": " << (j + 1) << ", \"channels\": " << topology_[i][j] << "}";
                first_link = false;
            }
        }
    }
    json_file << "\n  ],\n";

    // Write logical links
    json_file << "  \"logical_links\": [\n";
    first_link = true;
    for (size_t i = 0; i < num_fpgas; ++i) {
        for (size_t j = i + 1; j < num_fpgas; ++j) {
            if (logical_demand[i][j] > 0) {
                 if (!first_link) {
                    json_file << ",\n";
                }
                // We divided by 2 because we double-counted during calculation.
                json_file << "    {\"source\": " << (i + 1) << ", \"target\": " << (j + 1) << ", \"demand\": " << logical_demand[i][j] / 2 << "}";
                first_link = false;
            }
        }
    }
    json_file << "\n  ]\n";

    json_file << "}\n";
    json_file.close();
}

/**
 * @brief Groups nets based on their FPGA connection patterns.
 * @return A vector of net groups, where each group is a vector of net IDs.
 *         Nets with the same source and sink FPGA connections are grouped together.
 *         If multiple nodes are on the same FPGA, they are counted as one connection.
 *         The connection pattern shows the source FPGA and each sink FPGA with the count of nodes.
 */
std::vector<std::vector<int>> Design::groupNetsByFpgaConnection() const {
    // 检查必要的数据是否已加载
    if (nets_.empty() || fpgas_.empty()) {
        throw std::logic_error("Grouping Error: Nets and FPGAs must be loaded before grouping.");
    }
    
    // 使用map来存储具有相同FPGA连接模式的net组
    // 键是一个表示FPGA连接模式的字符串，值是对应的net ID列表
    std::map<std::string, std::vector<int>> connectionGroups;
    
    // 遍历所有net
    for (const auto& net : nets_) {
        // 跳过无效的net（没有source或fpga信息）
        if (!net.source || !net.source->fpga) {
            continue;
        }
        
        // 获取源FPGA ID
        int srcFpgaId = net.source->fpga->id;
        
        // 收集所有sink所在的FPGA ID，并统计每个FPGA上的节点数量
        std::map<int, int> sinkFpgaCounts; // FPGA ID -> 节点数量
        
        for (const auto& sink : net.sinks) {
            if (sink && sink->fpga) {
                // 如果sink与source在同一个FPGA上，则忽略
                if (sink->fpga->id == srcFpgaId) {
                    continue;
                }
                
                // 统计每个FPGA上的节点数量
                sinkFpgaCounts[sink->fpga->id]++;
            }
        }
        
        // 创建连接模式字符串，格式为: "srcFPGA_id:sinkFPGA1_id(count),sinkFPGA2_id(count),..."
        std::string connectionPattern = std::to_string(srcFpgaId) + ":";
        
        // 对sink FPGA ID进行排序，确保连接模式的一致性
        for (const auto& pair : sinkFpgaCounts) {
            if (connectionPattern.length() > std::to_string(srcFpgaId).length() + 1) {
                connectionPattern += ",";
            }
            connectionPattern += std::to_string(pair.first) + "(" + std::to_string(pair.second) + ")";
        }
        
        // 将当前net ID添加到对应的连接模式组中
        connectionGroups[connectionPattern].push_back(net.id);
    }
    
    // 将map转换为vector<vector<int>>格式返回
    std::vector<std::vector<int>> result;
    for (const auto& group : connectionGroups) {
        result.push_back(group.second);
    }
    
    return result;
}
