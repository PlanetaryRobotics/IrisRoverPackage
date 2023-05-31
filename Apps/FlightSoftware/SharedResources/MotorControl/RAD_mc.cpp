/*
 * RAD_mc.cpp
 *
 *  Created on: May 28, 2023
 *      Author: iris
 */


//#include "RAD_mc.hpp"
#include "RAD_mc.hpp"


//namespace IrisMC
//{

    void init_rad_mc()
    {
        // Init Motors


        // Init Register Array

    }

    bool init_motor ()
    {
        int i = 0;
        MCError_t err = MC_NO_ERROR;
        for (i; i < NUM_MOTORS; i++) {
            motors[i].id = 0;
        }
        return true;
    }

    bool init_i2c_regs ()
    {

    }

    void test_mc ()
    {

    }

//}
