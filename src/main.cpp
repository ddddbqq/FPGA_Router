#include "Design.hpp"
#include "Utils.hpp"



int main() {
    auto start_time = std::chrono::high_resolution_clock::now();

    // UPDATED: File paths are now relative to the project root.
    const std::string data_prefix = "benchmarks/case03/";
    const std::string info_file = data_prefix + "design.info";
    const std::string net_file = data_prefix + "design.net";
    const std::string topo_file = data_prefix + "design.topo";
    const std::string fpga_map_file = data_prefix + "design.fpga.out";

    const std::string viz_output_file = "scripts/visualization_data.json";

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

        design.generateVisualizationData(viz_output_file);
        
        // 输出net group信息到文件
        const std::string net_groups_file = "scripts/net_groups.txt";
        outputNetGroupsToFile(design, net_groups_file);

    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
