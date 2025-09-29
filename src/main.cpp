#include "Design.hpp"
#include <iostream>
#include <string>
#include <filesystem>
#include <chrono>

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



int main() {
    auto start_time = std::chrono::high_resolution_clock::now();

    // UPDATED: File paths are now relative to the project root.
    // This works because the executable will be run from the 'build' directory.
    const std::string data_prefix = "benchmarks/case04/";
    const std::string info_file = data_prefix + "design.info";
    const std::string net_file = data_prefix + "design.net";
    const std::string topo_file = data_prefix + "design.topo";
    const std::string fpga_map_file = data_prefix + "design.fpga.out";

    try {
        Design design;

        // Load files in the correct logical order.
        auto load_start = std::chrono::high_resolution_clock::now();

        std::cout << "Loading " << info_file << "..." << std::endl;
        design.loadInfo(info_file);

        std::cout << "Loading " << fpga_map_file << "..." << std::endl;
        design.loadFpgaMapping(fpga_map_file);

        std::cout << "Loading " << net_file << "..." << std::endl;
        design.loadNets(net_file);

        std::cout << "Loading " << topo_file << "..." << std::endl;
        design.loadTopo(topo_file);
        auto load_end = std::chrono::high_resolution_clock::now();

        std::cout << "\nAll files parsed successfully!\n" << std::endl;

        // Print loading time
        auto load_duration = std::chrono::duration_cast<std::chrono::milliseconds>(load_end - load_start);
        std::cout << "File loading time: " << load_duration.count() << " milliseconds" << std::endl;
        // Print a summary of the parsed data for verification.
        // printDesignStats(design);

    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
