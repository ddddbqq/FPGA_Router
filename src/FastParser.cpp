#include "FastParser.hpp"
#include <cstdio>
#include <stdexcept>

FastParser::FastParser(const std::string& filename) : buffer_(nullptr), current_pos_(nullptr), file_size_(0) {
    // Open the file in binary read mode.
    FILE* file = fopen(filename.c_str(), "rb");
    if (!file) {
        throw std::runtime_error("FastParser Error: Cannot open file: " + filename);
    }

    // Seek to the end of the file to determine its size.
    fseek(file, 0, SEEK_END);
    file_size_ = ftell(file);
    fseek(file, 0, SEEK_SET); // Rewind to the beginning.

    // Allocate buffer with an extra byte for a null terminator.
    buffer_ = new char[file_size_ + 1];
    
    // Read the entire file into the buffer.
    size_t bytes_read = fread(buffer_, 1, file_size_, file);
    if (bytes_read != file_size_) {
        delete[] buffer_;
        fclose(file);
        throw std::runtime_error("FastParser Error: Failed to read the entire file: " + filename);
    }

    // Null-terminate the buffer to treat it like a C-string.
    buffer_[file_size_] = '\0';
    current_pos_ = buffer_;

    fclose(file);
}

FastParser::~FastParser() {
    delete[] buffer_;
}

bool FastParser::isEOF() const {
    // The end is reached if the current position is at or beyond the end of the file content.
    return current_pos_ >= buffer_ + file_size_;
}

void FastParser::skipWhitespace() {
    while (*current_pos_ && (*current_pos_ == ' ' || *current_pos_ == '\n' || *current_pos_ == '\r' || *current_pos_ == '\t')) {
        current_pos_++;
    }
}

char FastParser::peekNextNonWhitespaceChar() {
    // Save current position
    char* original_pos = current_pos_;
    skipWhitespace();
    char next_char = isEOF() ? '\0' : *current_pos_;
    // Restore position, since this is a peek.
    current_pos_ = original_pos;
    return next_char;
}

int FastParser::parseInt() {
    skipWhitespace();
    int val = 0;

    while (*current_pos_ >= '0' && *current_pos_ <= '9') {
        val = val * 10 + (*current_pos_ - '0');
        current_pos_++;
    }
    return val;
}

int FastParser::parseId(char prefix) {
    skipWhitespace();
    // Skip the prefix character, e.g., 'F' or 'g'.
    if (*current_pos_ == prefix) {
        current_pos_++;
    }
    return parseInt();
}

void FastParser::skipChar(char c) {
    skipWhitespace();
    if (*current_pos_ == c) {
        current_pos_++;
    }
    skipWhitespace();
}

