#ifndef _FRAM_File_hpp_
#define _FRAM_File_hpp_

#include <Fw/Cfg/Config.hpp>
#include <Fw/Types/BasicTypes.hpp>

    #define FRAM_TABLE_PTR_START 0x00000
    #define FRAM_TABLE_PTR_END   0x13880
    #define FRAM_DATA_PTR_START  0x13881
    #define FRAM_DATA_PTR_END    0x3FFFF
    #define FRAM_TABLE_ENTRY_SIZE   0x8 

    struct FRAM_PTR_BITS{
        uint32_t address:18;
    };

    typedef union FRAM_PTR{
        uint32_t all;
        FRAM_PTR_BITS bit;
    }FRAM_PTR;

    typedef struct FRAM_TABLE_ENTRY{
        uint32_t data_name;
        uint8_t data_size;
        FRAM_PTR data_location;
    }FRAM_TABLE_ENTRY;

    typedef uint32_t AddressPointer;
    typedef AddressPointer StartAddress;
    typedef AddressPointer FilePointer;
    typedef uint32_t FileSize;

namespace Os {

    // This class encapsulates a very simple file interface that has the most often-used features

    class FRAM_File {
        public:

            typedef enum {
                OP_OK, //!<  Operation was successful
                DOESNT_EXIST, //!<  File doesn't exist (for read)
                NO_SPACE, //!<  No space left
                NO_PERMISSION, //!<  No permission to read/write file
                BAD_SIZE, //!<  Invalid size parameter
                BAD_BUFFER, //!< Buffer doesn't exist
                BAD_REQUESTED_BOUNDS, //!< Requested Log bounds don't exist
                OTHER_ERROR, //!<  A catch-all for other errors. Have to look in implementation-specific code
            } Status;

            FRAM_File(); //!<  Constructor
            Status read(void * buffer, U32 start_time, U32 end_time);
            Status write(const void * buffer, uint8_t buff_size, uint32_t buff_name);
            Status check_end_table_overrun();
            Status check_start_table_overrun();

        private:
        
            // required to run 
            //CY15B102Q m_framMem;
            FRAM_PTR table_start_ptr;
            FRAM_PTR table_end_ptr;
            FRAM_TABLE_ENTRY table_start_data;
            FRAM_TABLE_ENTRY table_end_data;
            bool first_write;
    };

}

#endif
