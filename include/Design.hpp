#ifndef DESIGN_HPP
#define DESIGN_HPP

#include "DataTypes.hpp"
#include <vector>
#include <string>
#include <unordered_map>

/**
 * @class Design
 * @brief Main container for all design data.
 *
 * This class holds all the information parsed from the input files,
 * including FPGAs, logical nodes, nets, and the physical topology.
 * It provides a structured way to access the entire design representation.
 */
class Design {
public:
    Design() = default;

    /**
     * @brief Loads FPGA resource constraints from a .info file.
     * @param filename Path to the design.info file.
     */
    void loadInfo(const std::string& filename);

    /**
     * @brief Loads the netlist and communication requirements from a .net file.
     * @param filename Path to the design.net file.
     */
    void loadNets(const std::string& filename);

    /**
     * @brief Loads the initial physical topology from a .topo file.
     * @param filename Path to the design.topo file.
     */
    void loadTopo(const std::string& filename);

    /**
     * @brief Loads the mapping of logical nodes to FPGAs from a .fpga.out file.
     * @param filename Path to the design.fpga.out file.
     */
    void loadFpgaMapping(const std::string& filename);


    /**
     * @brief Analyzes the loaded data and generates a JSON file for visualization.
     * @param filename The path for the output JSON file.
     */
    void generateVisualizationData(const std::string& filename) const; // Added const

    // Public getters to access the parsed data.
    const std::vector<FPGA>& getFpgas() const { return fpgas_; }
    const std::unordered_map<int, Node>& getNodes() const { return nodes_; }
    const std::vector<Net>& getNets() const { return nets_; }
    const std::vector<std::vector<int>>& getTopology() const { return topology_; }

private:
    std::vector<FPGA> fpgas_;                            // Stores all FPGA objects, indexed by ID-1.
    std::unordered_map<int, Node> nodes_;         // Maps node ID to Node object for fast lookup.
    std::vector<Net> nets_;                              // Stores all nets.
    std::vector<std::vector<int>> topology_;             // Adjacency matrix for the FPGA topology.
};

#endif // DESIGN_HPP