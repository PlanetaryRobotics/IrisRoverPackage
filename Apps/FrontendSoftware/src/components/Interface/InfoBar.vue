<!--
Information Bar that Goes in the Header of the Cosmos Combined Screens.

Author: Connor W. Colombo, CMU
Created: 08/07/2020
Last Update: 08/14/2020, Colombo
-->

<template>
    <div class="info-bar">
        <span
            class="tooltip"
            title="Rate of data arriving from the rover"
        >
            Data Frequency: 10 pt/sec
        </span>
        <span 
            class="updatable tooltip"
            :style="`width: ${timeString.length+1}ch`"
            :title="`Current ${this.clock.name} time (H:M:S)`"
        >
            {{ timeString }}
        </span>
        <span 
            class="updatable tooltip"
            :style="`width: ${lastRoverDataTimeString.length+1}ch`"
            :title="`Time of last update from the Rover (H:M:S)`"
        >
            {{ lastRoverDataTimeString }}
        </span>
        <span 
            class="clickable tooltip"
            title="Status of connection to rover. Click to manually recheck."
            :style="`width: ${longestRoverStateLength+11}ch`"
            @click="forceConnectionCheck(); roverStateHovered = false;"
            @mouseenter="roverStateHovered = true"
            @mouseleave="roverStateHovered = false"
        >
            Rover State: {{ roverStateHovered && !pollingConnection ? roverStates.get('clickToCheckConnection') : roverState }}
        </span>
        <!-- Battery Indicator: -->
        <span 
            class="battery tooltip"
            title="Rover battery level"
        >
            <svg class="battery" width="100%" height="100%" viewBox="0 0 20 20" fill="none" xmlns="http://www.w3.org/2000/svg">
                <path fill-rule="evenodd" clip-rule="evenodd" d="M2 17C2 16 2 16 3 16H17C17.5 16 17.5 16 17.5 16.5V17.175H18C18.5 17.175 18.5 17.175 18.5 17.5V18.5C18.5 18.825 18.5195 18.825 18 18.825H17.5V19.05C17.5 20 17.5 20 17 20H3C2 20 2 20 2 19V17ZM3 16.5C2.5 16.5 2.5 16.5 2.5 17V19C2.5 19.5 2.5 19.5 3 19.5H16.5C17 19.5 17 19.5 17 19V18.5H17.5C18 18.5 18 18.5 18 18C18 17.5 18 17.5 17.5 17.5H17V17C17 16.5 17 16.5 16.5 16.5L3 16.5Z" fill="#FCFCFC"/>
                <path :d="`M3 17.5V17H3.50001H${batteryLevelSVGLength}V19H3.5H3V18.5V17.5Z`" fill="#FCFCFC"/>
                <path fill-rule="evenodd" clip-rule="evenodd" d="M2 13C2 12 2 12 3 12H17C17.5 12 17.5 12 17.5 12.5V13.175H18C18.5 13.175 18.5 13.175 18.5 13.5V14.5C18.5 14.825 18.5195 14.825 18 14.825H17.5V15.05C17.5 16 17.5 16 17 16H3C2 16 2 16 2 15V13ZM3 12.5C2.5 12.5 2.5 12.5 2.5 13V15C2.5 15.5 2.5 15.5 3 15.5H16.5C17 15.5 17 15.5 17 15V14.5H17.5C18 14.5 18 14.5 18 14C18 13.5 18 13.5 17.5 13.5H17V13C17 12.5 17 12.5 16.5 12.5L3 12.5Z" fill="#FCFCFC"/>
                <path :d="`M3 13.5V13H3.50001H${batteryLevelSVGLength}V15H3.5H3V14.5V13.5Z`" fill="#FCFCFC"/>
                <path fill-rule="evenodd" clip-rule="evenodd" d="M2 9C2 8 2 8 3 8H17C17.5 8 17.5 8.00004 17.5 8.49996V9.175H18C18.5 9.175 18.5 9.175 18.5 9.5V10.5C18.5 10.825 18.5195 10.825 18 10.825H17.5V11.05C17.5 12 17.5 12 17 12H3C2 12 2 12 2 11V9ZM3 8.5C2.5 8.5 2.5 8.5 2.5 9V11C2.5 11.5 2.5 11.5 3 11.5H16.5C17 11.5 17 11.5 17 11V10.5H17.5C18 10.5 18 10.5 18 10C18 9.5 18 9.5 17.5 9.5H17V9C17 8.5 17 8.49996 16.5 8.5L3 8.5Z" fill="#FCFCFC"/>
                <path :d="`M3 9.5V9H3.50001H${batteryLevelSVGLength}V11H3.5H3V10.5V9.5Z`" fill="#FCFCFC"/>
            </svg>
        </span>
        <span class="updatable tooltip" title="Rover battery level" style="width: 7ch">&nbsp; {{batteryLevel}} %</span>
    </div>
</template>

<script>
    import Vue from 'vue'
    import { mapGetters, mapState } from 'vuex'

    import AsyncComputed from 'vue-async-computed'
    Vue.use(AsyncComputed);
    
    import DB from '@/DBInterface/DBInterface.js'
    import Clock from '@/system/Clock.js'

    import TooltipEquip from '@/styles/TooltipEquip.js';
    
    export default {
        name: 'InfoBar',
        data(){
            return {
                batteryMinLevelSVGLength: 3, // Length of horizontal section of SVG when battery is at 0%
                batteryMaxLevelSVGLength: 16.5, // Length of horizontal section of SVG when battery is at 100%

                time: "+00:00:00", // Time to be displayed in the InfoBar
                lastRoverDataTime: "+00:00:00", // Time of last rover data update to be displayed in the InfoBar

                // TODO: REMOVEME: Connect to actual battery telemetry signal
                battery_fade_time: 4.8, // [hr] Time until simulated battery runs out.
                fake_battery_level: 99,

                // Enumeration of all possible rover states (constant) - used for ensuring text consistency and finding the longest one.
                roverStates: new Map([
                    ['clickToCheckConnection', 'Check'], 
                    ['polling', 'Polling...'],
                    ['connected', 'Connected'],
                    ['noDB', 'DB Lost'],
                    ['noRover', 'Offline']
                ]),
                pollingConnection: true, // Whether the rover state connection is currently being checked abnormally (either as a first check or an intention recheck)
                connectedToDB: false, // Whether this display shows that this terminal is connected to the DB
                connectedToRover: false, // Whether this display shows that the backend is connected to the rover
                roverStateHovered: false // Whether the Rover State indicator is currently hovered
            }
        },
        props: {
            // To be used for syncing the clocks between multiple InfoBar instances (eg. if instances are inside a container that frequently gets v-if'd in and out of existence frequently and you don't want the clock flashing each time).
            syncedClock: {
                required: false,
            }
        },
        mounted(){
            TooltipEquip(this.$el);

            // Update the Clock on Ticks:
            this.clock.eventBus.on('tick', this.onClockTick);
            this.onClockTick(); // Force clock update right away

            this.updateRoverState();
            // Once DB is Connected:
            DB.eventBus.on('statusChange', this.onDBConnectionChange);

            // Force connection check to eliminate polling signal and get most accurate current status:
            this.forceConnectionCheck();

            this.stateLogList.onUpdate( () => {
                this.updateRoverState(true);
            })
        },
        beforeDestroy(){
            // Unsubscribe to prevent memory leak:
            DB.eventBus.off('statusChange', this.onDBConnectionChange);
        },
        methods: {
            onClockTick(){
                this.time = this.clock.full({includeStamp: true});
                this.lastRoverDataTime = this.clock.full({t: this.currentSystemState.avionics.lastMessageTime});
                
                // TODO: FIXME: Using fake time-moderated battery drain rn. Replace with real soon.
                this.fake_battery_level = Math.max(0, Math.min(100, 100 - Math.ceil(100 * this.clock.hours() / this.battery_fade_time)));
            },
            
            forceConnectionCheck(){
                this.pollingConnection = true;
                // Recheck DB Connection Status (changes are reported through DB eventBus):
                DB.checkConnection().then( (connected) => {
                    this.onDBConnectionChange({connected});
                });
                // TODO: Also send message to backend to check rover connection status (if necessary)
            },

            /**
             * Update the InfoBar when the DB connection status changes.
             */
            onDBConnectionChange({connected}){
                this.pollingConnection = false;
                if(connected !== this.connectedToDB){
                    this.updateRoverState(connected);
                }
            },
            /**
             * Returns a string representing the rover's current state.
             * To be called whenever anything that affects the rover connection status changes.
             */
            updateRoverState(dbStatus){
                this.connectedToDB = dbStatus;
                this.connectedToRover = this.currentSystemState.backend.connectedToRover; // TODO: Make sure this changes when the rover connection status flag changes on the DB
            }
        },
        watch: {
            currentSystemState(sys){
                // Update the synchronized clock (if the same clock is to be used across multiple InfoBar instances):
                if(sys.time.moon && !sys.time.moon.empty){
                    this.$emit('update:syncedClock', sys.time.moon);
                }
            }
        },
        computed: {
            ...mapState({
                stateLogList: state => state.SYS.stateLogList
            }),
            ...mapGetters([
                'currentSystemState' // Most recent SystemData entry on the Database from DBLazyList
            ]),

            // Pointer to the clock which should be used to update the time:
            clock(){
                return this.syncedClock || this.currentSystemState.time.moon || new Clock();
            },

            /** 
             * Returns the String indiciating the current clock time.
             */
            timeString(){
                return this.time;
            },
            /** 
             * Returns the String indiciating the last time data was received from the rover.
             */
            lastRoverDataTimeString(){
                return `Last Updated: ${this.lastRoverDataTime}`;
            },

            // Returns the Rover's Battery level as an integer from 0 to 100 (%).
            // TODO: FIXME: Connect to real battery telemetry signal.
            batteryLevel(){
                return this.fake_battery_level;
            },
            // Computes the appropriate length of horizontal section of SVG for the current battery level.
            batteryLevelSVGLength(){
                return this.batteryMinLevelSVGLength + (this.batteryMaxLevelSVGLength - this.batteryMinLevelSVGLength) * this.batteryLevel/100;
            },

            // String of current rover state:
            roverState(){
                if(this.pollingConnection){
                    return this.roverStates.get('polling');
                } else if(!this.connectedToDB){
                    return this.roverStates.get('noDB');
                } else if(!this.connectedToRover){
                    return this.roverStates.get('noRover');
                } else {
                    return this.roverStates.get('connected');
                }
            },

            // Length of longest valid rover state string. Computed in `mounted`. Used for ensuring even spacing.
            longestRoverStateLength(){
                return [...this.roverStates.values()].reduce( (max, str) => str.length > max ? str.length : max, 0 );
            },
        }
    }
</script>

<style scoped lang="scss">
@import '@/styles/_colors.scss';
@import '@/styles/_functional.scss';
@import '@/styles/_dimensions.scss';

    .info-bar {
        height: 100%;
        
        display: flex;
        justify-content: flex-end;
        align-items: baseline;
        padding: {
            left: $std-big-padding;
        }
        user-select: none;

        & > span{
            padding-right: 2.5 * $std-padding;
            line-height: 0;
            color: $color-near-white;
            
            &.clickable{
                &:hover {
                    color: $color-grey-light
                }
            }
        }

        & > .updatable {
            display: flex;
            justify-content: center;
        }
    }

    .battery {
        position: relative;
        top: 0.05rem;
        width: 20px;
        height: 20px;
        line-height: 0;
    }
</style>