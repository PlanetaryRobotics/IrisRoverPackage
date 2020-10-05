<template>
    <div class="error__analysis">
        <div class="master--timeline">
            <D3_Timeline :layout="temperature.timeline.layout"
                             :axes="temperature.timeline.axes"
                             :selectedRange="this.allRanges"
                             zoomable="false"
                >
            </D3_Timeline>
        </div>
        <div class="row">
            <Tabs class="col-9 error" @handleremove="setActive">
                <!--               A Tab Container            -->
                <Tab v-for="(tab,id) in tabs" :key="id" :title="tab.id" :active="tab.isActive" color="#fff">
                    <!--                    left info sidebar             -->
                    <div class="col-3">
                        <div class="row-12 error--log" :content="renderSensor()">
                            <div class="row error--log--title text__main">
                                <svg width="12" height="16" class="">
                                    <rect v-for="(i, id) in tab.priorityValue"
                                          :key="id"
                                          width="2"
                                          height="14"
                                          style="fill: #f00;"
                                          :x=(i-1)*4
                                    />
                                </svg>
                                <div class="error--log--name">{{tab.id}}</div>
                                <div class="col category align-self-end">
                                    icon
                                </div>
                            </div>
                            <div class="row">
                                <span> Category</span>{{tab.category}}
                            </div>
                            <div class="row">
                                <span>Sensor Name({{tab.name.length}})</span> {{tab.name}}
                            </div>
                            <div class="row">
                                <span>Time Frame (M+)</span> {{tab.timeFrame}}
                            </div>

                            <div class="break"></div>

                            <div class="row">
                                <span>Summary</span>
                                <p>{{tab.summary}}</p>
                            </div>
                            <div class="row"><span>Notes</span> {{tab.notes}}</div>
                            <div class="row"><span>Priority</span> {{tab.priority}}</div>
                            <div class="row"><span>Tags</span> {{tab.tags}}</div>

                            <div class="break"></div>

                            <div class="row"><span>Created</span> {{tab.timeFrame[0]}}</div>
                            <div class="row"><span>Last modified</span> {{tab.timeFrame[0]}}</div>

                            <div class="break"></div>
                            <div class="row"><span>Images</span></div>
                        </div>
                    </div>
                    <div class="col-9">
                        <div class="row-12">
                            <div class="error--graph">
                                <div class="error--graph--title">
                                    <span class="title text__main--bold">Error Data </span>
                                    <span>Expend All</span>
                                </div>

                                <div class="subgraphs error--graph__selected">
                                    <div class="title text__main--bold">
                                        <span> Selected Sensors</span>
                                    </div>

                                    <!--                  selected sensors                -->
                                    <div class="" v-for="(graph, id) in temperature.analysisGraphs" :key="id">
                                        <p class="text__small">Temp Sensor {{id}}</p>
                                        <D3Functional
                                                :graphid="id"
                                                :column="graph"
                                                :layout="temperature.layout"
                                                :name="temperature.name"
                                                :safezone="temperature.safezone"
                                        ></D3Functional>
                                    </div>
                                    <span>placeholder for other graphs</span>
                                </div>
                            </div>

                        </div>
                    </div>
                </Tab>
            </Tabs>
            <div class="col-3 logs">
                <div class="row-6">
                    <ComponentContainer :header="error" class="">
                        <div class="telemetryContent" v-for="(errorTag, index) in errorTags" :key="index">
                            <ErrorTag
                                    :error-tag="errorTag"
                                    :index="index"
                                    v-on:pushTag="pushToTabs(errorTag)"
                            >
                            </ErrorTag>
                        </div>
                    </ComponentContainer>
                </div>
                <div class="row-6">
                    <ComponentContainer :header="system" class="system">
                        <div class="telemetryContent"></div>
                    </ComponentContainer>
                </div>
            </div>
        </div>
    </div>
</template>

<script>
    import D3Functional from "./TelemetryComponents/Graph/Functional/D3Functional";
    import ComponentContainer from "../Interface/TelemetryContainer";
    // import Toggler from "./TelemetryComponents/Toggler/Toggler";
    import D3_Timeline from "./TelemetryComponents/Graph/Functional/Timeline";
    // import SafeZone from "./TelemetryComponents/SafeZone/SafeZone";
    // import TelemetryButton from "./TelemetryComponents/TelemetryButtons/TelemetryButton";

    import ErrorTag from "./TelemetryComponents/ErrorTag/ErrorTag";
    // import D3Graph from "./TelemetryComponents/Graph/D3Graph";
    import Tabs from "./TelemetryComponents/Tabs/Tabs";
    import Tab from "./TelemetryComponents/Tabs/Tab";

    export default {
        name: "ErrorAnalysis",
        props:['activeCard'],
        components:{
            Tab,
            Tabs,
            // D3Graph,
            ErrorTag,
            // TelemetryButton,
            // SafeZone,
            D3Functional,
            ComponentContainer,
            // Toggler,
            D3_Timeline
        },
        mounted() {
            this.analysisInit();
        },
        methods:{
            /*
                dataInit
                Initialize dates from error cards & render on master timeline
                Add props.activeCard / default first card to page so that contents are dynamically displayed
            */
            analysisInit(){
                this.errorTags.forEach(d => {
                    this.allRanges.push([Date.parse(d.timeFrame[0]), Date.parse(d.timeFrame[1])])
                });
                if (this.activeCard !== undefined) {
                    // if there is an active card from previous detailed page
                    this.tabs.push(this.activeCard);
                } else {
                    if (this.errorTags.length >= 1) {
                        this.tabs.push(this.errorTags[0]);
                        this.tabs[0].isActive = true;
                    } else {
                      console.log('no errors');
                    }
                }
            },
            renderSensor(){
            },
            pushToTabs(tag) {
                let tempTabs = [...this.tabs];
                if (!tempTabs.includes(tag)){
                    tempTabs.push(tag);

                    tempTabs.forEach(tab => {
                        tab.isActive = (tab.id === tag.id);
                    });
                    console.log('pushed')

                } else {
                    tempTabs.forEach(tab => {
                        tab.isActive = (tab.id === tag.id);
                    });
                }
                this.tabs = tempTabs;
            },
            setActive(currentTab){
                console.log(currentTab)

                let tempTabs = this.tabs.filter(tab => {
                    return tab.id !== currentTab
                });
                tempTabs.forEach(tab => {
                    tab.isActive = false;
                });
                if (tempTabs.length > 0)
                    tempTabs[0].isActive = true;
                this.tabs = tempTabs;
            }
        },
        data(){
            return {
                temperature:{
                    name: "Temperature",
                    analysisGraphs:[
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                    ],
                    layout:{
                        width: 700,
                        height: 100,
                        marginTop: 20,
                        marginRight: 20,
                        marginBottom: 20,
                        marginLeft: 20,
                    },
                    safezone:[4,15],
                    timeline:{
                        layout:{
                            width: 700,
                            height: 50,
                            marginTop: 0,
                            marginRight: 20,
                            marginBottom: 20,
                            marginLeft: 20,
                        },
                        axes:['bottom']
                    },
                },
                allRanges:[],
                error:{name:"Error Log"},
                system:{name:"System Log"},
                errorTags:[
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
                    }
                ],
                tabs:[]
            }
        },
        watch:{
        }
    }
</script>

<style scoped lang="scss">
    @import '@/styles/_functional.scss';

    .error__analysis{
        color: $color-near-white;
        height: 100%;
        background-color: $color-near-black;
    }
    .master--timeline{
        position: absolute;
        left: 18.75%;
        width: 56.5%;
    }
    .error .col-9 {
        margin-top: 100px;
    }

    .error--graph {
        width: 100%;
        background-color: $color-background;


        .error--graph--title {
            line-height: 30px;
            border-bottom: 1px solid $color-grey-dark;
            /*padding-left: 16px;*/
            :nth-child(2) {
                float: right;
                padding-right: 16px;
                color: $color-grey-light;
            }
        }
        .error--graph__selected {
            .title {
                line-height: 30px;
                /*padding-left: 16px;*/
            }
            p {
                position: absolute;
                left: 100px;
            }
        }
    }

    .error--log {
        position: relative;
        background-color: $color-background;

        border-radius: 4px;
        padding-left: 16px;
        padding-top: 8px;

        font-size: 1.1rem;
        line-height: 12px;
        margin-bottom: 8px;

        &--title{
            margin-top: 8px;
            margin-left: -16px;
        }
        &--name {
            color: #FF2F46;
            font-weight: 600;
        }
        span {
            width: 100%;
            margin-bottom: 4px;
        }

        .row {
            margin-bottom: 16px;
        }

        .category {
            text-align: right;
            padding-right: 25px;
            font-weight: 500;
            font-size: 0.9rem;
            line-height: 10px;
            color: #FFFFFF;
            margin-bottom: 8px;

            transition: 0.15s ease;
            -webkit-transition: 0.15s ease;

            &.active .arrow {
                -webkit-transform: rotate(45deg) translate(-4px, -4px);
                transform: rotate(45deg) translate(-4px, -4px);
            }

            &.active .arrow:before {
                -webkit-transform: translate(8px, 0);
                transform: translate(8px, 0);
            }

            &.active .arrow:after {
                -webkit-transform: rotate(90deg) translate(8px, 0);
                transform: rotate(90deg) translate(8px, 0);
            }

            .arrow {
                width: 8px;
                height: 8px;
                display: inline-block;
                position: relative;
                bottom: 0;
                left: 5px;
                -webkit-transition: 0.4s ease;
                transition: 0.4s ease;
                margin-top: 2px;
                text-align: left;
                -webkit-transform: rotate(45deg);
                transform: rotate(45deg);
                float: right;
            }

            .arrow:before, .arrow:after {
                position: absolute;
                content: "";
                display: inline-block;
                width: 8px;
                height: 1px;
                background-color: #fff;
                -webkit-transition: 0.4s ease;
                transition: 0.4s ease;
            }

            .arrow:after {
                position: absolute;
                -webkit-transform: rotate(90deg);
                transform: rotate(90deg);
                top: -4px;
                left: 4px;
            }

        }



        .viewButton {
            /* Primary */
            color: #8A6DFF;
            cursor: pointer;
            padding-top: 12px;
            /*padding-bottom: 12px;*/
            clear: both;
        }

        span {
            font-weight: 600;
        }

        .open {
            display: block;
        }

        .collapse {
            display: none;
        }
    }
    .break {
        height: 1px;
        margin: 16px 8px 16px 0;
        background-color: $color-grey-dark;
    }
    .logs{
        .containerContent{
            margin-left: 16px;
        }
    }
</style>
