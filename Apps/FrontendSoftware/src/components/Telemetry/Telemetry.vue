<template>
    <div class="telemetry row">
        <div class="col-9">
            <div class="row telemetry--dashboard--header">
                <div class="col">Time Scale: {{graphWidth}}</div>
                <div class="col alignright">[Placeholder for Mark Error]</div>
            </div>
<div class="row">
<!-- Col 1: Temperature -->
        <div class="col-4">
            <ComponentContainer :header="temperature" class="temp wrap">
                <div class="telemetryContent">
                    <div style="clear: both; height: 15px;"></div>
                    <div style="clear: both; height: 15px;"></div>
                    <div class="dashitem" v-for="(graph, id) in temperature.graphs" :key="id" style="position:relative;">
                        <p>Temp Sensor {{id}}</p>
                        <D3Graph :column="graph"
                                 :layout="temperature.layout"
                                 :name="temperature.name"
                                 :width="graphWidth"
                                 :safezone="temperature.safezone"></D3Graph>
                    </div>
                </div>
            </ComponentContainer>
        </div>
<!--  Col 2: Power + Communication -->
        <div class="col-4">
            <div class="row-6">
                <ComponentContainer :header="power">
                    <div class="telemetryContent">
                        <div class="dashitem" v-for="(graph, id) in power.graphs" :key="id" style="position:relative;">
                            <D3Graph :column="graph"
                                     :layout="power.layout"
                                     :name="power.name"
                                     :width="graphWidth"
                                     :safezone="power.safezone"></D3Graph>
                        </div>
                    </div>
                </ComponentContainer>
            </div>
            <div class="row-6">
                <ComponentContainer :header="communication">
                    <div class="telemetryContent">
                        <div class="dashitem" v-for="(graph, id) in communication.graphs" :key="id" style="position:relative;">
                            <D3Graph :column="graph"
                                     :layout="communication.layout"
                                     :name="communication.name"
                                     :width="graphWidth"
                                     :safezone="communication.safezone"
                            ></D3Graph>
                        </div>
                    </div>
                </ComponentContainer>
            </div>
        </div>
<!--  Col 3: Orientation + Acceleration -->
        <div class="col-4">
            <div class="rover row-8">
                <ComponentContainer :header="orientation">
                    <div class="telemetryContent">
                        <div class="tdwrap">
                            <TDRover :RoverID="orientation.twoD.xz"></TDRover>
                            <TDRover :RoverID="orientation.twoD.yz"></TDRover>
                        </div>
                        <div style="clear:both;"></div>
                        <div class="rover-content">
                            <div class="rover-pages">
                                <Rover></Rover>
                            </div>
                        </div>
                    </div>
                </ComponentContainer>
            </div>
            <div class="row-4">
                <ComponentContainer :header="acceleration">
                    <div class="telemetryContent">
                        <div class="dashitem" v-for="(graph, id) in acceleration.graphs" :key="id" style="position:relative;">
                            <D3Graph :column="graph"
                                     :layout="acceleration.layout"
                                     :name="acceleration.name"
                                     :width="graphWidth"
                                     :safezone="acceleration.safezone"></D3Graph>
                        </div>
                    </div>
                </ComponentContainer>
            </div>
        </div>
</div>
        </div>
<!--  Col 4: Error log -->
        <div class="col-3">
            <ComponentContainer :header="error">
                <div class="telemetryContent" v-for="(errorTag, index) in errorTags" :key="index">
                        <ErrorTag  :error-tag="errorTag"></ErrorTag>
                </div>
            </ComponentContainer>
        </div>
    </div>
</template>

<script>
    import { mapState, mapMutations, mapActions } from 'vuex'
    import Rover from '@/components/Telemetry/TelemetryComponents/Orientation/rover.vue'
    import D3Graph from '@/components/Telemetry/TelemetryComponents/Graph/D3Graph.vue'
    import ComponentContainer from "../Interface/TelemetryContainer";
    import TDRover from '@/components/Telemetry/TelemetryComponents/Orientation/TDRover.vue'
    import ErrorTag from "./TelemetryComponents/ErrorTag/ErrorTag";

    export default {
        name: 'Telemetry',
        data(){
            return{
                newLink:'',
                errorTags:[
                    {
                        id:'A02',
                        name:['B1','B2','B3'],
                        category:'Temperature',
                        timeFrame:['Wed Jan 01 2014 09:00:00 GMT-0500 (EST)', 'Thu Jan 02 2014 02:00:00 GMT-0500 (EST)'],
                        summary: 'Template card 2',
                        notes: 'A bunch of notes',
                        tags: ['TempError'],
                        priority: 'Mid',
                        priorityValue: 2
                    },
                    {
                        id:'A01',
                        name:['A1','A2','A3'],
                        category:'Motor',
                        timeFrame:['Wed Jan 01 2014 03:00:00 GMT-0500 (EST)', 'Thu Jan 02 2014 02:00:00 GMT-0500 (EST)'],
                        summary: 'Template card 1',
                        notes: 'No notes',
                        tags: ['Mechanical', 'OverHeat'],
                        priority: 'Low',
                        priorityValue: 1
                    },
                ],
                temperature:{
                    name: "Temperature",
                    graphs: [
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                    ],
                    layout:{
                        height: 100,
                        marginTop: 20,
                        marginRight: 20,
                        marginBottom: 20,
                        marginLeft: 20,
                    },
                    safezone:[4,15],
                },
                communication:{
                    name: "Communication",
                    graphs: [
                        ['Date', 'Air_Temp'],
                    ],
                    layout:{
                        height: 100,
                        marginTop: 20,
                        marginRight: 20,
                        marginBottom: 20,
                        marginLeft: 20,
                    },
                    safezone:[4,15]
                },
                power:{
                    name: "Power",
                    graphs: [
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                    ],
                    layout:{
                        height: 100,
                        marginTop: 20,
                        marginRight: 20,
                        marginBottom: 20,
                        marginLeft: 20,
                    },
                    safezone:[5,17]
                },
                orientation:{
                    name: "Orientation",
                    twoD:{
                        xz:['xz','backxz'],
                        yz:['yz','backyz']
                    }
                },
                acceleration:{
                    name: "Acceleration",
                    graphs: [
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                    ],
                    layout:{
                        height: 100,
                        marginTop: 20,
                        marginRight: 20,
                        marginBottom: 20,
                        marginLeft: 20,
                    },
                    safezone:[4,12]
                },
                error:{name:"Error Log"},
                windowWidth: 0,
                graphWidth: 1200
            }
        },
        components:{
            // Toggler,
            ComponentContainer,
            // Outer,
            // Stats,
            // Bar,
            Rover,
            TDRover,
            D3Graph,
            ErrorTag,
        },
        computed: {
            ...mapState({
                title: state => state.TELEMETRY.title,
                links: state => state.TELEMETRY.links,
            })
        },
        mounted() {
            this.$nextTick(function() {
                window.addEventListener('resize', this.debounce(this.getWindowWidth));
                this.getWindowWidth();
            })
        },
        methods:{
            ...mapMutations([
                'TELEMETRY_ADD_DATA'
            ]),
            ...mapActions([
                'removeLink'
            ]),
            addData: function(){
                this.TELEMETRY_ADD_DATA(this.newLink);
                this.newLink=''; // set to empty when someone hit enter
            },
            removeLinks: function(link){
                this.removeLink(link);
            },
            debounce(func) {
              let timeout = null;
              return function() {
                clearTimeout(timeout);
                timeout = setTimeout(() => {
                  func.apply(this.arguments);
                }, 300);
              }
            },
            getWindowWidth() {
                this.windowWidth = document.documentElement.clientWidth;
                this.graphWidth = this.windowWidth / 4;
            },
        },
        beforeDestroy() {
            window.removeEventListener('resize', this.debounce(this.getWindowWidth));
        }
    }
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style lang="scss">
    @import '@/styles/_functional.scss';

    .telemetry {
        height: 100vh;
        background-color: $color-near-black;
    }

    .rover-pages canvas {
        width: 80% !important;
        height: 80% !important;
        margin-left: 10%;
    }

    .rover-header {
        position: absolute;
        top: 0;
        width: 100%;
        background: #f3f6fb;
        text-shadow: 0 1px 0 rgba(255,255,255,0.5);
        border-bottom: solid 1px #dfe2e7;
        padding: 15px 15px 15px 30px;
        line-height: 20px;
    }

    .rover-content {
        width: 100%;
        /*height: 100%;*/
        padding-top: 52px;
        /*overflow: hidden;*/
    }

    .orientation{
        float: left;
        width: 33%;
        text-align: center;
    }


    .dashitem{
        margin-left:0;
    }

    body::-webkit-scrollbar {
        width: 0.1em;
        border: rgba(0,0,0,0);
    }

    body::-webkit-scrollbar-track{
        color: #2A2A2A;
        box-shadow: inset 0 0 6px rgba(0, 0, 0, 0.3);
    }

    ::-webkit-slider-container{
        color: #2A2A2A;
        box-shadow: inset 0 0 6px rgba(0, 0, 0, 0.3);
    }

    .telemetryContainer{
        .telemetryContainer__header{
            background-color: $color-near-black;
            margin-left: 6px;
            padding-left: 0;
            padding-top: 12px;
            /*margin-bottom: 4px;*/
            border: none !important;
        }
        .telemetryContent{
            margin-left: 6px;
            background-color: #2a2a2a;
            height: 100%;
            margin-bottom: 4px;
            padding-top: 15px;
        }
        p{
            padding-left: 15px;
        }
    }
    .row-6 .telemetryContainer{
        height: 100%;
    }
    .telemetry--dashboard--header {
        line-height: 32px;
      color: $color-near-white;
    }
    .alignleft {
        float: left;
        font-style: normal;
        font-weight: 800;
    }

    .alignright {
        float: right;
        text-align: right;
    }

    .inactive{
        color: $color-grey-light;
    }

    .tdrover{
        position: relative;
        float: left;
    }


</style>
