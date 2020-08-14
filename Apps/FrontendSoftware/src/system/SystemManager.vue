<!--
A singleton renderless Vue component containing basic functionality to setup and manage the complete (frontend+DB+backend) 
software system.

**Note:** There should only be one SystemManager instatiated on the root Component (App.vue). This class mainly exists as a 
way of streamlining the code present in the root component.

Author: Connor W. Colombo, CMU
Created: 08/07/2020
Last Update: 08/12/2020, Colombo
-->

<template>
    <div />
</template>

<script>
    import { mapState, mapGetters } from 'vuex'

    import DB from '@/DBInterface/DBInterface.js'
    import SystemData from '@/data_classes/SystemData.js'

    let _instance_count = 0; // Total number of system managers components in existence (there should only be one)

    export default {
        name: 'SystemManager',
        data(){
            return {
                initialized: 0, // Whether the system has been initialized
            };
        },
        computed: {
            ...mapGetters({
                log: 'systemStateLog', // Getter FUNCTION for the SystemData entries on the Database from DBLazyList
                currentState: 'currentSystemState', // Most recent SystemData entry on the Database from DBLazyList
            }),
            ...mapState({
                logLazyList: state => state.SYS.stateLogList // DBLazyList of recent SystemData states
            })
        },
        mounted(){
            _instance_count++;
            if(_instance_count > 1){
                console.error(`[IRIS-SYS] There should only be ONE SystemManager. Currently ${_instance_count} have been instantiated.`);
            }

            // Once DB is Connected:
            DB.eventBus.on('statusChange', this.onDBConnectionChange);
            DB.checkConnection(); // Force initial connection check.
        },
        beforeDestroy(){
            DB.eventBus.off('statusChange', this.onDBConnectionChange);
        },
        methods: {
            onDBConnectionChange({connected}){
                if(connected && !this.initialized){
                    /** 
                     * On first connection to DB, check to see if this terminal is the first to access the DB for this mission (partition).
                     * If so, initialize the DB by providing an initial SystemData entry.
                     */
                    if(this.log.length === 0){
                        this.logLazyList.forcePush({obj: new SystemData()});
                    }
                    this.initialized = true;
                    DB.eventBus.off('statusChange', this.onDBConnectionChange); // Deregister. Event handler no longer needed.
                }
            }
        }
    }
</script>