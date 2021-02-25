#include <Fw/Cfg/Config.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Os/FreeRTOS/FRAM_File.hpp>
#include <Fw/Types/Assert.hpp>

//#include <cerrno>
//#include <sys/types.h>
//#include <limits>
#include <string.h>
#include <stdio.h>


//#define DEBUG_PRINT(x,...) printf(x,##__VA_ARGS__); fflush(stdout)
//#define DEBUG_PRINT(x,...)
  

namespace Os {

  /**
   * @brief      Constructs a new instance.
   */
  File::File() {

    table_start_ptr.all = FRAM_TABLE_PTR_START;
    table_end_ptr.all = FRAM_TABLE_PTR_START;
    first_write = true;

  }


  /**
   * @brief      Destroys the object.
   */
  //File::~File() {
  //}

  //***********************TODO: SHOULD DO A CHECK IN COMLOGGER THAT WE HAVE WRITTEN SOMETHING BEFORE WE TRY TO READ
  //***********************TODO: NEED TO FIGURE OUT HOW MUCH WE CAN STORE IN BUFFER WHEN READING AND HOW TO SEND MORE

  /**
   * @brief      Read from flash memory
   *
   * @param      buffer       The buffer
   * @param      start_time   Time start search for
   * @param      end_time     Time end search for
   *
   * @return     The file status
   */
  File::Status File::read(void * buffer, U32 start_time, U32 end_time) {
    Status stat = OP_OK;

    // Check if start_time and end_time are between our know max and min times
    if(start_time < table_start_data.data_name || end_time > table_end_data.data_name)
    {
      return BAD_REQUESTED_BOUNDS;
    }

    FRAM_PTR current_table_tracker = table_start_ptr;
    FRAM_TABLE_ENTRY current_data_tracker = table_start_data;

    // Find the first entry that we need to send back
    while(current_data_tracker.data_name < start_time)
    {
        // Increment table tracker and check for table overrun
        current_table_tracker += FRAM_TABLE_ENTRY_SIZE;
        if(current_table_tracker > FRAM_TABLE_PTR_END)
          current_table_tracker = FRAM_TABLE_PTR_START;

        // Update data tracker with new table entry
        CY15B102QError error_code = framSpiReadData(CY15B102Q::FRAMSpiCommands FSRTD,
                                                      uint8_t *(&current_data_tracker),
                                                      FRAM_TABLE_ENTRY_SIZE,
                                                      current_table_tracker);
        // TODO: Deal with error code 
    }

    // Send all of the next data entries until we reach end_time
    while(current_data_tracker.data_name <= end_time)
    {
      // Check if read will go over max data value
      if((current_data_tracker.location.bit + current_data_tracker.data_size) > FRAM_DATA_PTR_END)
      {
        // Send Data at end of data section
        CY15B102QError error_code1 = framSpiReadData(CY15B102Q::FRAMSpiCommands FSRTD,
                                                      uint8_t *(buffer),
                                                      FRAM_DATA_PTR_END - current_data_tracker.location.bit,
                                                      current_data_tracker.location.bit);
        // Send Data at start of data section
        CY15B102QError error_code2 = framSpiReadData(CY15B102Q::FRAMSpiCommands FSRTD,
                                                      uint8_t *(buffer + (FRAM_DATA_PTR_END - current_data_tracker.location.bit)),
                                                      (current_data_tracker.location.bit + current_data_tracker.data_size) - FRAM_DATA_PTR_END,
                                                      FRAM_DATA_PTR_START);
        // TODO: Deal with error codes
      }  
      else
      {
        // Send Data
        CY15B102QError error_code = framSpiReadData(CY15B102Q::FRAMSpiCommands FSRTD,
                                                      uint8_t *(buffer),
                                                      current_data_tracker.data_size,
                                                      current_data_tracker.location.bit);
        // TODO: Deal with error code
      } 
      // Check if we're at the end of our Table Data, if so we should return
      if(current_table_tracker == table_end_ptr)
        return stat;

      // Increment table tracker and check for table overrun
      current_table_tracker += FRAM_TABLE_ENTRY_SIZE;
      if(current_table_tracker > FRAM_TABLE_PTR_END)
        current_table_tracker = FRAM_TABLE_PTR_START;

      // Update data tracker with new table entry
      CY15B102QError error_code = framSpiReadData(CY15B102Q::FRAMSpiCommands FSRTD,
                                                      uint8_t *(&current_data_tracker),
                                                      FRAM_TABLE_ENTRY_SIZE,
                                                      current_table_tracker);
      // TODO: Deal with error code

    }
    return stat;
  }


/**
 * @brief      Write to flash memory
 *
 * @param[in]  buffer       The buffer
 * @param      size         The size
 *
 * @return     The file status
 */
  File::Status File::write(const void * buffer, uint8_t buff_size, uint32_t buff_name) {
    Status stat = OP_OK;

    // Check for invalid buffer or buffer size
    if(buff == NULL)
      {
        stat = BAD_BUFFER;
        return stat;
      }
    if(buff_size > 128 || buff_size == 0)
      {
        stat = BAD_SIZE;
        return stat;
      }

    if(first_write)
    {
      // Set table_start/end_data to very start of FRAM_DATA_PTR_START
      table_start_data.data_name = buff_name;
      table_start_data.data_size = buff_size;
      table_start_data.data_location = FRAM_DATA_PTR_START;
      table_end_data = table_start_data;

      // Write data to start location
      CY15B102QError error_code_data = framSpiWriteData(CY15B102Q::FRAMSpiCommands WRSR,
      // May need to cast buffer to a uint8_t * as it's a void * right now
                                                    buffer,
                                                    buff_size,
                                                    table_start_data.data_location);
      // Write table to start location
      CY15B102QError error_code_table = framSpiWriteData(CY15B102Q::FRAMSpiCommands WRSR,
                                                    uint8_t *(&table_start_data),
                                                    sizeof(table_start_data),
                                                    table_start_ptr);  
      // TODO: Deal with error codes

      // Change Table Pointer Locations just in case even though we did it when init-ing File.cpp
      table_start_ptr.all = FRAM_TABLE_PTR_START;
      table_end_ptr.all = FRAM_TABLE_PTR_START;

      // Don't need to do this again
      first_write = false;
    }

    else
    {
      // Calculate what the expected ending address of the buff would be if we write it
      // Calcuation: where last data is stored + size of data + size of buffer
      uint32_t projected_data_loc = table_end_data.data_location.bit + table_end_data.data_size + buff_size;
      // Calcualte how many bytes can be save at the end of the data section and left for the begining
      uint32_t start_projected_data_size = projected_data_loc - FRAM_DATA_PTR_END;
      uint32_t end_projected_data_size = buff_size - start_projected_data_size;

      if(projected_data_loc > FRAM_DATA_PTR_END)
      {
        // Check to see if we don't need to remove table start data
        if(table_start_data.data_location.bit < table_end_data.data_location.bit /*check to see if end clear*/ 
            && table_start_data.data_location.bit > start_projected_data_size /*check to see if start of data is clear*/)
        {
          // Write end projected data size of buffer to the last bytes of the data FRAM
          CY15B102QError error_code1 = framSpiWriteData(CY15B102Q::FRAMSpiCommands WRSR,
                                                      buffer,
                                                      end_projected_data_size,
                                                      table_end_data.data_location.bit + table_end_data.data_size);
          // Write the rest of the project data size of buffer to the first bytes of the data FRAM
          CY15B102QError error_code2 = framSpiWriteData(CY15B102Q::FRAMSpiCommands WRSR,
                                                      (uint8_t *)((uint32_t)buffer + end_projected_data_size),
                                                      start_projected_data_size,
                                                      FRAM_DATA_PTR_START);
          // TODO: Deal with error codes
          // Increment table end ptr and change table data
          table_end_ptr += FRAM_TABLE_ENTRY_SIZE;
          Status temp = check_end_table_overrun();
          if(temp != OP_OK)
            stat = temp;
          // TODO: Deal with error code 
          table_end_data.data_location = table_end_data.data_location.bit + table_end_data.data_size;
          table_end_data.data_name = buff_name;
          table_end_data.data_size = buff_size;
          // Write data to table end location
          CY15B102QError error_code_table = framSpiWriteData(CY15B102Q::FRAMSpiCommands WRSR,
                                                        uint8_t *(&table_end_data),
                                                        sizeof(table_end_data),
                                                        table_end_ptr);
        }

        // We need to remove table start data
        else
        {
          // Calculate how many bytes will be needed to be given up
          int32_t data_bytes_needed;

          // Check to see if we don't have space at the end (and also thus the begining) of the data
          if(table_start_data.data_location.bit > table_end_data.data_location.bit)
            data_bytes_needed = buff_size - (table_start_data.data_location.bit - (table_end_data.data_location.bit + table_end_data.data_size));
          // We have space at the end but don't have enough space in the begining of the data section so we must go to the next start_table entry
          else
            data_bytes_needed = start_projected_data_size - table_start_data.data_location.bit;

          // Free next start locations until we have enough data bytes open
          while(data_bytes_needed > 0)
          {
            // Reduce the number of bytes needed by the size of the start 
            data_bytes_needed -= table_start_data.data_size;
            // Increment to new Table start entry
            table_start_ptr += FRAM_TABLE_ENTRY_SIZE;
            Status temp = check_start_table_overrun();
            if(temp != OP_OK)
              stat = temp;
            // Copy next Table entry into table start data
            CY15B102QError error_code = framSpiReadData(CY15B102Q::FRAMSpiCommands FSRTD,
                                                      uint8_t *(&table_start_data),
                                                      FRAM_TABLE_ENTRY_SIZE,
                                                      table_start_ptr);
            // TODO: Deal with error code 
          }
          // Write data to new end location and change table end ptr tracker
          table_end_ptr += FRAM_TABLE_ENTRY_SIZE;
          Status temp = check_end_table_overrun();
          if(temp != OP_OK)
            stat = temp;
          // TODO: Deal with error code 
          // Write end projected data size of buffer to the last bytes of the data FRAM
          CY15B102QError error_code1 = framSpiWriteData(CY15B102Q::FRAMSpiCommands WRSR,
                                                      buffer,
                                                      end_projected_data_size,
                                                      table_end_data.data_location.bit + table_end_data.data_size);
          // Write the rest of the project data size of buffer to the first bytes of the data FRAM
          CY15B102QError error_code2 = framSpiWriteData(CY15B102Q::FRAMSpiCommands WRSR,
                                                      (uint8_t *)((uint32_t)buffer + end_projected_data_size),
                                                      start_projected_data_size,
                                                      FRAM_DATA_PTR_START);
          // TODO: Deal with error code 
          table_end_data.data_location = table_end_data.data_location.bit + table_end_data.data_size;
          table_end_data.data_name = buff_name;
          table_end_data.data_size = buff_size;
          // Write data to table end location
          CY15B102QError error_code = framSpiWriteData(CY15B102Q::FRAMSpiCommands WRSR,
                                                        uint8_t *(&table_end_data),
                                                        sizeof(table_end_data),
                                                        table_end_ptr);
        }
      }

      else
      {
        // Write data to new end location and change table end ptr tracker
        table_end_ptr += FRAM_TABLE_ENTRY_SIZE;
        Status temp = check_end_table_overrun();
        if(temp != OP_OK)
          stat = temp;

        // Check if our end data is before our start data
        if(table_end_data.data_location.bit < table_start_data.location.bit && projected_data_loc > table_start_data.location.bit)
        {
          // Calculate how many bytes will be needed to be given up
          int32_t data_bytes_needed = projected_data_loc - table_start_data.data_location.bit;
          // Free next start locations until we have enough data bytes open
          while(data_bytes_needed > 0)
          {
            // Reduce the number of bytes needed by the size of the start 
            data_bytes_needed -= table_start_data.data_size;
            // Increment to new Table start entry
            table_start_ptr += FRAM_TABLE_ENTRY_SIZE;
            Status temp = check_start_table_overrun();
            if(temp != OP_OK)
              stat = temp;
            // Copy next Table entry into table start data
            CY15B102QError error_code = framSpiReadData(CY15B102Q::FRAMSpiCommands FSRTD,
                                                      uint8_t *(&table_start_data),
                                                      FRAM_TABLE_ENTRY_SIZE,
                                                      table_start_ptr);
            // TODO: Deal with error code 
          }
        }
        // Write data to end location
        CY15B102QError error_code_data = framSpiWriteData(CY15B102Q::FRAMSpiCommands WRSR,
                                                    buffer,
                                                    buff_size,
                                                    table_end_data.data_location.bit + table_end_data.data_size);
        // TODO: Deal with error code 
        table_end_data.data_location = table_end_data.data_location.bit + table_end_data.data_size;
        table_end_data.data_name = buff_name;
        table_end_data.data_size = buff_size;
        // Write data to table end location
        CY15B102QError error_code_table = framSpiWriteData(CY15B102Q::FRAMSpiCommands WRSR,
                                                        uint8_t *(&table_end_data),
                                                        sizeof(table_end_data),
                                                        table_end_ptr);
        // TODO: Deal with error codes 
      }
    }

    return stat;
  }

  // Check if the new table_end_ptr is larger than the table
  File::Status File::check_end_table_overrun()
  {
      // Check if we've hit the end of our table allocation
      if(table_end_ptr > FRAM_TABLE_PTR_END)
      {
        // If we're still using the first table address, we need to increment it
        if(table_start_ptr == FRAM_TABLE_PTR_START)
        {
          table_start_ptr = FRAM_TABLE_PTR_START + FRAM_TABLE_ENTRY_SIZE;
          // Read new table start entry data into table start data from table_start_ptr
          CY15B102QError error_code = framSpiReadData(CY15B102Q::FRAMSpiCommands FSRTD,
                                                      uint8_t *(&table_start_data),
                                                      FRAM_TABLE_ENTRY_SIZE,
                                                      table_start_ptr);
          // TODO: check error_code and assign stat code from that
        }

        table_end_ptr = FRAM_TABLE_PTR_START;
      }
  }

  // Check if the new table_start_ptr is larger than the table
  File::Status File::check_start_table_overrun()
  {
      // Check if we've hit the end of our table allocation
      if(table_start_ptr > FRAM_TABLE_PTR_END)
      {
        // If we're still using the first table address, we need to increment it
        if(table_end_ptr == FRAM_TABLE_PTR_START)
        {
          // TODO: Error that shouldn't happen where table_end
        }

        table_start_ptr = FRAM_TABLE_PTR_START;
      }
  }

}
