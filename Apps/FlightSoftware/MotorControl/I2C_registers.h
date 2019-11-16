/*
 * CubeRover Motor Control Registers for Communicating over I2C.
 */
#ifndef _CR_MOT_I2C_REG_H
#define _CR_MOT_I2C_REG_H

#include <stdint.h>

    // Struct of register contents (struct members used to make code w/in motor controller more readable).
    struct i2c_register_data_type {
        // Registers:
        uint8_t address; //  - Motor Controller I2C Address

        int32_t target_pos; //      - [ticks] Relative Target Position
        uint8_t direction; //       - Direction of Rotation (0: Forward, 1: Reverse)
        uint8_t target_speed; //    - [%] Target Speed (0-100%)
        int32_t current_pos; //     - [ticks] Motor Current Position
        int32_t rotor_pos; //       - [ticks] Rotor Position

        int16_t mot_current; //     - [mA] Motor Current

        int16_t kP_current; //      - [pwm/mA] Proportional Constant for Current-PWM Control Loop
        int16_t kI_current; //      - [pwm/mA/s] Integral Constant for Current-PWM Control Loop

        int16_t kP_velocity; //     - [mA*s/tick] Proportional Constant for Velocity-Current Control Loop
        int16_t kI_velocity; //     - [mA/tick] Integral Constant for Velocity-Current Control Loop
        int16_t kD_velocity; //     - [mA*s^2/tick] Derivative Constant for Velocity-Current Control Loop

        int16_t kP_position; //     - [mA/tick] Proportional Constant for Position-Velocity Control Loop
        int16_t kI_position; //     - [mA/tick/s] Integral Constant for Position-Velocity Control Loop
        int16_t kD_position; //     - [mA*s/tick] Derivative Constant for Position-Velocity Control Loop

        uint16_t acceleration; //   - [ticks/s/s]
        uint16_t deceleration; //   - [ticks/s/s]

        uint8_t execute; //         - write -> Execute new target position

        uint16_t velocity; //       - [ticks/s] Present Velocity

        uint8_t enable; //          - write -> Enable the Driver
        uint8_t disable; //         - write -> Disable the Driver (TODO: can we consolidate these to just one param called "enabled").
        uint8_t reset; //           - Reset Controller

        struct { //                 - Fault Registers
            uint8_t overcurrent:1;
            uint8_t motor_stall:1;
            uint8_t bad_posSensor:1;
            uint8_t bad_i2cParameter:1;
            uint8_t unexpected:1;
            uint8_t watchdog:1;
            uint8_t:0; // close out the byte
        } fault;

        uint8_t clearFault; //      - write -> Clear fault state

        struct {
            uint8_t current:2; //   - Current state (00b: disabled, 01b: enabled, 10b: fault).
            uint8_t complete:1; //  - Whether the target position has been reached (1=reached).
            uint8_t:0; // close out the byte
        } status;

        struct { //                 - Present state of each hall effect sensor (probably can't transmit this as fast as it's changing).
            uint8_t sensor1:1;
            uint8_t sensor2:1;
            uint8_t sensor3:1;
            uint8_t:0; // close out the byte
        } hall_state;
      };

      // Link structure to union to allow dumping / passing complete register bank as a single stream.
      union {
          uint8_t raw[sizeof(struct i2c_register_data_type)]; //  - Raw data as bytes
          struct i2c_register_data_type data; //                  - Core representation of data
      } i2c_register;

      // Map the register IDs which will be given over I2C to identify what's being read/written.
      void* i2c_regmap[] = {
        &i2c_register.data.address, //       0x01

        &i2c_register.data.target_pos, //    0x02
        &i2c_register.data.direction, //     0x03
        &i2c_register.data.target_speed, //  0x04
        &i2c_register.data.current_pos, //   0x05
        &i2c_register.data.rotor_pos, //     0x06

        &i2c_register.data.mot_current, //   0x07

        &i2c_register.data.kP_current, //    0x08
        &i2c_register.data.kI_current, //    0x09

        &i2c_register.data.kP_velocity, //   0x0A
        &i2c_register.data.kI_velocity, //   0x0B
        &i2c_register.data.kD_velocity, //   0x0C

        &i2c_register.data.kP_position, //   0x0D
        &i2c_register.data.kI_position, //   0x0E
        &i2c_register.data.kD_position, //   0x0F

        &i2c_register.data.acceleration, //  0x10
        &i2c_register.data.deceleration, //  0x11

        &i2c_register.data.execute, //       0x12

        &i2c_register.data.velocity, //      0x13

        &i2c_register.data.enable, //        0x14
        &i2c_register.data.disable, //       0x15
        &i2c_register.data.reset, //         0x16

        &i2c_register.data.fault, //         0x17
        &i2c_register.data.clearFault, //    0x18

        &i2c_register.data.status, //        0x19
        &i2c_register.data.hall_state //     0x1A
      };


      /*
       * Set default / hard-coded values on start.
       */
      void Register_Init() {
          i2c_register.data.address = 0x43;
      }

#endif // _CR_MOT_I2C_REG_H
