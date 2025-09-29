#ifndef FAST_PARSER_HPP
#define FAST_PARSER_HPP

#include <string>

/**
 * @class FastParser
 * @brief A high-performance file parser using fread.
 *
 * This class reads an entire file into a memory buffer upon construction
 * and provides methods to parse data types (integers, IDs) directly
 * from this buffer. This approach minimizes I/O overhead.
 */
class FastParser {
public:
    /**
     * @brief Constructs a FastParser and reads the specified file into memory.
     * @param filename The path to the file to be parsed.
     */
    explicit FastParser(const std::string& filename);

    /**
     * @brief Destructor, frees the allocated memory buffer.
     */
    ~FastParser();

    // Delete copy constructor and assignment operator to prevent shallow copies.
    FastParser(const FastParser&) = delete;
    FastParser& operator=(const FastParser&) = delete;

    /**
     * @brief Checks if the parser has reached the end of the file buffer.
     * @return True if all characters have been processed, false otherwise.
     */
    bool isEOF() const;
    
    /**
     * @brief Peeks at the next non-whitespace character without consuming it.
     * @return The next non-whitespace character, or '\0' if at the end of the file.
     */
    char peekNextNonWhitespaceChar();

    /**
     * @brief Parses a non-negative integer from the current buffer position.
     *
     * It automatically skips any leading whitespace.
     * @return The parsed integer value.
     */
    int parseInt();

    /**
     * @brief Parses an identifier with a specific prefix (e.g., 'F' for FPGAs, 'g' for nodes).
     * * It skips the prefix character and then parses the following integer.
     * e.g., for "g123", it will return 123.
     * @param prefix The character prefix to expect and skip.
     * @return The integer part of the identifier.
     */
    int parseId(char prefix);

    /**
     * @brief Skips a specific character at the current position.
     *
     * This is useful for parsing delimiters like ':' and ','.
     * It also skips any surrounding whitespace.
     */
    void skipChar(char c);

    /**
     * @brief Advances the internal pointer until a non-whitespace character is found.
     */
    void skipWhitespace();

private:
    char* buffer_;          // Pointer to the start of the file content buffer.
    char* current_pos_;     // Pointer to the current parsing position in the buffer.
    size_t file_size_;      // Total size of the file in bytes.
};

#endif // FAST_PARSER_HPP

