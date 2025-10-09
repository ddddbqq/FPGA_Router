#ifndef DATATYPES_HPP
#define DATATYPES_HPP

#include "Global.hpp"

// Forward declaration to resolve circular dependency between Node and FPGA.
class FPGA;

/**
 * @class Node
 * @brief Represents a logical node (gate) in the design.
 * * Each logical node has a unique ID and is mapped to a specific physical FPGA.
 */
class Node {
public:
    int id;          // Unique identifier for the logical node (e.g., 1 for "g1").
    FPGA* fpga;      // Pointer to the FPGA this node is placed on.

    // Default constructor for map creation.
    Node() : id(-1), fpga(nullptr) {} 
    
    // Constructor for initialization.
    Node(int nodeId) : id(nodeId), fpga(nullptr) {}
};

/**
 * @class FPGA
 * @brief Represents a physical FPGA device in the system.
 * * It contains information about its resource constraints (max I/O) and the
 * logical nodes that are mapped to it.
 */
class FPGA {
public:
    int id;                                // Unique identifier for the FPGA (e.g., 1 for "F1").
    int max_io;                            // Maximum number of external I/O channels allowed.
    std::vector<Node*> nodes;       // List of logical nodes placed on this FPGA.

    // Default constructor.
    FPGA() : id(-1), max_io(0) {}

    // Constructor for initialization.
    FPGA(int fpgaId, int maxIo) : id(fpgaId), max_io(maxIo) {}
};

/**
 * @class Net
 * @brief Represents a hyperedge (net) connecting multiple logical nodes.
 * * Each net has one source node and one or more sink nodes.
 * The weight is uniformly 1 as per the problem description.
 */
class Net {
public:
    int id;                               // Unique ID, typically its line number in the .net file.
    Node* source;                  // Pointer to the source logical node.
    std::vector<Node*> sinks;      // Pointers to the sink logical nodes.
    int weight;                           // Weight of the net (always 1 in this problem).
};

#endif // DATATYPES_HPP