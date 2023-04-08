// Names and messages to be sent to the Moon.
// Post-processed to remove/substitute non-ASCII characters, have consistent formatting, and ensure no name/messages is longer than 99char (FW_LOG_STRING_MAX_SIZE).

#ifndef IRIS_NAMES_AND_MESSAGES_HPP
#define IRIS_NAMES_AND_MESSAGES_HPP

// NOTE: All names/messages are separated with null terminators here so this is basically just a bunch of consecutive cstrings in memory.

namespace IrisNames{
    extern unsigned int LEN_CREDITS;
    extern const char CREDITS[];

    extern const unsigned int LEN_MESSAGES;
    // Length of the longest name of a messager:
    #define LEN_MAX_MESSAGER_NAME 38
    extern const char MESSAGES[];
} // namespace IrisNames

#endif // IRIS_NAMES_AND_MESSAGES_HPP
