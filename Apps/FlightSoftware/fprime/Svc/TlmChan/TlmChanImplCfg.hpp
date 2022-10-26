/**
 * \file
 * \author T. Canham
 * \brief Configuration file for Telemetry Channel component
 *
 * \copyright
 * Copyright 2009-2015, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 * <br /><br />
 */

#ifndef TLMCHANIMPLCFG_HPP_
#define TLMCHANIMPLCFG_HPP_

// Anonymous namespace for configuration parameters

// The parameters below provide for tuning of the hash function used to
// write and read entries in the database. The has function is very simple;
// It first takes the telemetry ID and does a modulo computation with
// TLMCHAN_HASH_MOD_VALUE. It then does a second modulo with the number
// of slots to make sure the value lands in the provided slots.
// The values can be experimented with to try and balance the number
// of slots versus the number of buckets.
// To test the set of telemetry ID in the system to see if the hash is
// balanced, do the following:
//  1) From the deployment directory (e.g Ref), do a full build then type:
//      "make comp_report_gen"
//     This will generate a list in "<deployment dir>/ComponentReport.txt"
//     with all the telemetry IDs in the deployment.
//  2) Cut and paste the ID list to the array declared in the TlmChan unit test
//     file TelemChanImplTester.cpp in the runMultiChannel() method.
//  3) Run the unit test ("make ut run_ut")
//  4) After writing all the telemetry IDs to the component, the
//     unit test will dump the hash table. The output looks like the following:
//        Slot: <n> - slot number
//        Entry - a bucket assigned to the slot
//        ... (Other buckets in the slot)
//     The number of buckets assigned to each slot can be checked for balance.

namespace
{

    enum
    {
        TLMCHAN_NUM_TLM_HASH_SLOTS = 22, // !< Number of slots in the hash table.
                                         // Works best when set to about twice the number of components producing telemetry
        // At the time of writing, there are only 12 components producing telem, but only 10 are connected. So 2*10=20 + some room (it used to be 30 as default)=22
        TLMCHAN_HASH_MOD_VALUE = 250, // !< The modulo value of the hashing function.
                                      // Should be set to a little below the ID gaps to spread the entries around

        TLMCHAN_HASH_BUCKETS = 72 // !< Buckets assignable to a hash slot.
                                  // Buckets must be >= number of telemetry channels in system
        // At the time of writing, there are 71 total telemetry channels in the system (used GSW to count across all FPrime XML):
        /*
        Telemetry Channels in BlockDriver: 1
        Telemetry Channels in RateGroupDriver: 0
        *Telemetry Channels in ActiveRateGroup-RateGroupLowFreq: 2
        *Telemetry Channels in ActiveRateGroup-RateGroupMedFreq: 2
        *Telemetry Channels in ActiveRateGroup-RateGroupHiFreq: 2
        Telemetry Channels in CubeRoverTime: 0
        Telemetry Channels in TlmChan: 0
        Telemetry Channels in CommandDispatcher: 2
        *Telemetry Channels in GroundInterface: 13
        *Telemetry Channels in NetworkManager: 6
        Telemetry Channels in ActiveLogger: 0
        *Telemetry Channels in Navigation: 5
        *Telemetry Channels in MotorControl: 12
        *Telemetry Channels in Imu: 6
        *Telemetry Channels in WatchDogInterface: 15
        *Telemetry Channels in Camera: 5

        But only those with *s are connected to `TlmChan` in Top, so this
        number could go as low as: 68 + some room = 70
        */
        // ... So, 72 plays it a little safe (in case any are added last minute but we forget to update this).
    };

}

#endif /* TLMCHANIMPLCFG_HPP_ */
