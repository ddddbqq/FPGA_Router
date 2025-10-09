#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
FPGA Net Group Consolidation Script

This script processes a file containing FPGA net group information and 
consolidates groups that have identical connection patterns (ignoring count values).

Input Format:
Group [group_number]: Source_FPGA -> Sink_FPGA1(count),Sink_FPGA2(count) -> [net_id1, net_id2, ...]

Output:
Consolidated groups with identical connection patterns, combining all nets from
groups that share the same source and sink FPGAs (regardless of count values).
"""

import re
from collections import defaultdict
import sys

def parse_net_groups(file_path):
    """
    Parse the net groups from the input file
    
    Args:
        file_path (str): Path to the input file
        
    Returns:
        list: List of dictionaries representing each net group
    """
    groups = []
    group_pattern = re.compile(r'Group\s*\[(\d+)\]:\s*(\w+)\s*->\s*(.*?)\s*->\s*\[(.*?)\]')
    
    with open(file_path, 'r') as f:
        for line in f:
            line = line.strip()
            if line.startswith('#') or not line:
                continue
                
            match = group_pattern.match(line)
            if match:
                group_num = int(match.group(1))
                source_fpga = match.group(2)
                sinks_str = match.group(3)
                nets_str = match.group(4)
                
                # Parse sink FPGAs (ignore counts)
                sinks = []
                if sinks_str != 'unknown':
                    for sink in sinks_str.split(','):
                        # Remove count information (e.g., "F1 (1)" becomes "F1")
                        sink_fpga = re.sub(r'\s*\(\d+\)', '', sink).strip()
                        sinks.append(sink_fpga)
                
                # Parse net IDs
                nets = [net.strip() for net in nets_str.split(',')]
                
                groups.append({
                    'group_num': group_num,
                    'source': source_fpga,
                    'sinks': sinks,
                    'nets': nets
                })
    
    return groups

def consolidate_groups(groups):
    """
    Consolidate groups with identical connection patterns
    
    Args:
        groups (list): List of parsed net groups
        
    Returns:
        dict: Consolidated groups with connection pattern as key
    """
    consolidated = defaultdict(list)
    
    for group in groups:
        # Create a key based on source and sorted sinks (ignoring counts)
        sinks_key = tuple(sorted(group['sinks'])) if group['sinks'] else 'unknown'
        key = (group['source'], sinks_key)
        
        consolidated[key].append(group)
    
    return consolidated

def generate_output(consolidated_groups):
    """
    Generate output in the required format
    
    Args:
        consolidated_groups (dict): Consolidated groups
        
    Returns:
        str: Formatted output string
    """
    output_lines = []
    output_lines.append("# Consolidated Net Groups by FPGA Connection Pattern")
    output_lines.append("# Format: Consolidated_Group: Source_FPGA -> Sink_FPGA1,Sink_FPGA2 -> [net_id1, net_id2, ...]")
    output_lines.append("")
    
    group_counter = 1
    for key, groups in consolidated_groups.items():
        source, sinks = key
        sinks_str = "unknown" if sinks == "unknown" else ",".join(sinks)
        
        # Combine all nets from all groups with this connection pattern
        all_nets = []
        for group in groups:
            all_nets.extend(group['nets'])
        
        output_lines.append(f"Consolidated_Group [{group_counter}]: {source} -> {sinks_str} -> [{', '.join(all_nets)}]")
        group_counter += 1
    
    # Add statistics
    total_original_groups = sum(len(groups) for groups in consolidated_groups.values())
    total_consolidated_groups = len(consolidated_groups)
    total_nets = sum(len(groups[0]['nets']) for groups in consolidated_groups.values())
    
    output_lines.append("")
    output_lines.append("# Statistics:")
    output_lines.append(f"# Original groups: {total_original_groups}")
    output_lines.append(f"# Consolidated groups: {total_consolidated_groups}")
    output_lines.append(f"# Total nets: {total_nets}")
    
    return "\n".join(output_lines)

def main(input_file, output_file=None):
    """
    Main function to process the FPGA net groups file
    
    Args:
        input_file (str): Path to input file
        output_file (str, optional): Path to output file. If None, print to console.
    """
    print("Reading and parsing net groups...")
    groups = parse_net_groups(input_file)
    print(f"Parsed {len(groups)} net groups")
    
    print("Consolidating groups with identical connection patterns...")
    consolidated = consolidate_groups(groups)
    
    print("Generating output...")
    output = generate_output(consolidated)
    
    if output_file:
        with open(output_file, 'w') as f:
            f.write(output)
        print(f"Output written to {output_file}")
    else:
        print("\n" + output)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python consolidate_net_groups.py <input_file> [output_file]")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else None
    
    main(input_file, output_file)