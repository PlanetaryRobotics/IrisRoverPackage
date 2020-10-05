<template>
    <ComponentContainer :header="telemetry" id="subpage">
        <div class="subpage row">
            <div class="col-2">
                <ComponentContainer :header="feature" class="feature">
                    <div class="telemetryContent">
                        <div class="row-1">
                            <p>Sort by Sensor Names</p>
                            <br>
                            <Toggler></Toggler>
                        </div>
                    </div>
                    <div class="telemetryContent">
                        <div class="row-2">
                            <p>Safe Range</p>
                            <SafeZone
                            :safezone="temperature.safezone"
                            v-on:editRange="editSafeRange"></SafeZone>
                        </div>
                    </div>
                    <div class="telemetryContent">
                        <div class="row-9">
                            <p>TroubleShoot</p>
                            <button v-bind:style="{'background-color': ''}"
                                    :id="troubleshoot.id"
                                    @click="errorOn()"
                                    class="button">
                                {{ troubleshoot.title }}
                            </button>
                        </div>
                    </div>
                </ComponentContainer>
            </div>
            <div class="col-7">
                <ComponentContainer :header="temperature" class="wrap">
                    <div class="telemetryContent">
                        <div class="progress_wrap" v-bind:style="{'display': progress.display}">
                            <ul class="progressbar">
                                <li :class="progress.step >= 2 ? 'active' : 'default'">Step 1: Select Sensors</li>
                                <li :class="progress.step >= 3 ? 'active' : 'default'">Step 2: Select Timeframe</li>
                                <li :class="progress.step >= 4 ? 'active' : 'default'">Step 3: Summary</li>
                            </ul>
                            <div class="progressbar_operations">
                                <button v-bind:style="{'background-color': progress.color}"
                                        :id="progress.id"
                                        type="submit"
                                        @click="stepForward()"
                                        class="button">
                                    {{ progress.title }}
                                </button>
                                <button v-bind:style="{'background-color': cancel.color}"
                                        :id="cancel.id"
                                        type="button"
                                        @click="OnPressCancel()"
                                        class="button">
                                    {{ cancel.title }}
                                </button>
                            </div>
                            <div style="clear: both;"></div>
                            <div class="progressbar_detail">{{progress.detail}}</div>
                        </div>
                        <div style="clear: both;"></div>

                        <div class="analysis" v-bind:style="{'display': showAnalysis ? 'block' : 'none'}">
                            <form id="create-error" @submit.prevent="processForm">
                            <div class="row">
                                <div class="col-6">Selected Sensors</div>
                                <div class="col-6">Selected TimeFrame</div>
                            </div>
                            <div class="row">
                                <div class="col-6">{{checked}}</div>
                                <div class="col-6">{{selectedRange}}</div>
                            </div>
                            <div class="row">
                                <div class="col-12"><label class="label">Summary</label></div>
                                <input name="summary" type="text" v-model="errorTag.summary" placeholder="Metadata stuff vestibulum id ligula porta felis euismod semper. Curabitur blandit tempus porttitor. (50 characters left)">
                            </div>
                            <div class="row">
                                <div class="col-12"><label class="label">Notes</label></div>
                                <input name="notes" type="text" v-model="errorTag.notes" placeholder="Metadata stuff vestibulum id ligula porta felis euismod semper. Curabitur blandit tempus porttitor. Maecenas faucibus mollis
                                interdum. Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt u... Curabitur
                                blandit tempus porttitor.">
                            </div>
                            <div class="row">
                                <div class="col-6">
                                    <div class="col-12"><label class="label">Tag</label></div>
                                    <input type="search" placeholder="Select Tag" name="tag">
                                </div>
                                <div class="col-6">
                                    <div class="col-12"><label class="label">Priority</label></div>
                                    <PriorityToggler v-on:selectTab="setPriority"/>
                                </div>
                            </div>
                                <button type="submit" class="button is-danger">Submit</button>
                            </form>
                        </div>
                        <div style="clear: both;"></div>
                        <!--                        <span>Checked names: {{ checked }}</span>-->
                        <div class="graphArea">
                            <div v-bind:style="{'clear':'both;', 'margin-top': progress.display ? '50px' : '0'}"></div>
                            <div class="subgraphs" v-for="(graph, id) in temperature.analysisGraphs" :key="id" v-bind:style="{'border': isGraphChecked(id)}">
                                <p>Temp Sensor {{id}}</p>
                                <input type="checkbox" :id="id" :value="id" v-model="checked" v-bind:style="{'display': progress.display, 'z-index': '10'}">
                                <D3Functional
                                        :graphid="id"
                                        :column="graph"
                                        :layout="temperature.layout"
                                        :name="temperature.name"
                                        :safezone="temperature.safezone"></D3Functional>
                            </div>
                            <D3_Timeline :layout="temperature.timeline.layout"
                                         :axes="temperature.timeline.axes"
                                         :selectedRange="this.allRanges"
                                         v-on:timelineSelection="onTimelineSelected"
                                         zoomable="true"
                            >
                            </D3_Timeline>
                        </div>
                    </div>
                </ComponentContainer>
            </div>
            <div class="col-3">
                <div class="row-6">
                <ComponentContainer :header="error" class="error">
                    <div class="telemetryContent" v-for="(errorTag, index) in errorTags" :key="index">
                        <ErrorTag  :error-tag="errorTag">
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
    </ComponentContainer>
</template>

<script>
    import D3Functional from "./TelemetryComponents/Graph/Functional/D3Functional";
    import ComponentContainer from "../Interface/TelemetryContainer";
    import Toggler from "./TelemetryComponents/Toggler/Toggler";
    import D3_Timeline from "./TelemetryComponents/Graph/Functional/Timeline";
    import SafeZone from "./TelemetryComponents/SafeZone/SafeZone";
    import PriorityToggler from "./TelemetryComponents/Toggler/PriorityToggler";
    import ErrorTag from "./TelemetryComponents/ErrorTag/ErrorTag";
    export default {
        name: "TelemetrySub",
        components: {
            ErrorTag,
            PriorityToggler,
            SafeZone,
            D3Functional,
            ComponentContainer,
            Toggler,
            D3_Timeline
        },
        data(){
            return{
                newLink:'',
                telemetry:{
                    name:"TELEMETRY SUBPAGE"
                },
                feature:{name:"Features"},
                error:{name:"Error Log"},
                system:{name:"System Log"},
                checked:[],
                temperature:{
                    name: "Temperature",
                    graphs: [
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                        ['Date', 'Air_Temp'],
                    ],
                    analysisGraphs:[
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
                troubleshoot: {
                    title: "MARK ERROR",
                    color: '',
                    id: "troubleshoot_button",
                    clicked: false
                },
                progress: {
                    title: "Next: Step 2",
                    step: 2,
                    color: '#8a6dff',
                    display:'none',
                    id: "progress_button",
                    detail:"Select sensors that have abnormal readings."
                },
                cancel:{
                    title: "CANCEL",
                    color: '',
                    id: "cancel"
                },
                confirmed: false,
                selectedRange:[],
                showAnalysis: false,
                allRanges:[
                    [Date.parse('Wed Jan 01 2014 09:00:00 GMT-0500 (EST)'), Date.parse('Thu Jan 02 2014 02:00:00 GMT-0500 (EST)')],
                    [Date.parse('Wed Jan 01 2014 010:00:00 GMT-0500 (EST)'), Date.parse('Thu Jan 02 2014 01:00:00 GMT-0500 (EST)')]
                ],
                errorTag: {
                    name:'',
                    timeFrame:[],
                    summary:[],
                    notes:[],
                    tag:[],
                    priority: null
                },
                errorTags:[
                    {
                        id:'A01',
                        name:['B1','B2','B3'],
                        category:'Temperature',
                        timeFrame:['Wed Jan 01 2014 09:00:00 GMT-0500 (EST)', 'Thu Jan 02 2014 02:00:00 GMT-0500 (EST)'],
                        summary: 'Template card 2',
                        notes: 'A bunch of notes',
                        tags: ['TempError'],
                        priority: 'Mid',
                        priorityValue: 2
                    }
                ]
            }
        },
        methods:{
            errorOn(){
                console.log('yo');
                this.troubleshoot.clicked = !this.troubleshoot.clicked;
                if(this.troubleshoot.clicked){
                    this.troubleshoot.title = 'CANCEL';
                    this.progress.display = 'block';
                } else {
                    this.restoreNormal();
                }
            },
            OnPressCancel(){
                this.restoreNormal();
            },
            editSafeRange(range){
                this.temperature.safezone = [parseInt(range.min), parseInt(range.max)];
            },
            stepForward(){
                this.progress.step++;
                if (this.progress.step < 4) {
                    this.progress.title = "Next: Step " + this.progress.step;
                    this.confirmSelection();
                } else if (this.progress.step === 4) {
                    this.progress.title = "Confirm";
                    this.showAnalysis = true;
                } else {
                   this.restoreNormal();
                }
            },
            restoreNormal(){
                this.troubleshoot.title = 'MARK ERROR';
                this.progress.display = 'none'; // turn off progress
                this.checked = [];
                this.progress.step = 2;
                this.progress.title = "Next: Step " + this.progress.step;
                this.confirmed = false;
                this.showAnalysis = false;
                this.temperature.analysisGraphs = this.temperature.graphs;
            },
            isGraphChecked(id){
                let i = 0;
                for(i = 0; i < this.checked.length; i++){
                    if (this.checked[i] === id) {
                        return 'solid 2px #fff';
                    }
                }
            },
            onTimelineSelected(range){
                this.selectedRange = [range[0], range[1]];
            },
            confirmSelection(){
                const checkedGraph = this.checked;
                this.temperature.analysisGraphs = this.temperature.graphs.filter(function(graph, id){
                    return checkedGraph.includes(id);
                });
                this.confirmed = true;
            },
            setPriority(priority){
                this.errorTag.priority = priority;
            },
            processForm() {
                this.errorTag.name = this.checked;
                this.errorTag.timeFrame = this.selectedRange;
                this.allRanges.push(this.selectedRange);
                // console.log({ name: this.errorTag.summary, email: this.errorTag.notes, priority: this.errorTag.priority });
                this.errorTags.push(this.errorTag);
                this.restoreNormal();
            }
        }
    }
</script>

<style scoped lang="scss">
@import '@/styles/_functional.scss';
    .wrap{
        height: 100%;
        .containerContent{
            overflow-y: scroll;
        }
    }
    .subpage{
        height: 100vh;
        background-color: #000;
    }
    .subpage button {
        margin: 15px 15px 15px 15px;
        padding: 3px 17px;
        font-size: 9.5px;
        line-height: 11px;
    }
    .toggle__container{
        margin-left: -10px;
        margin-bottom: 20px;
    }

    #subpage{
        background-color: #000;
        overflow: hidden;
        >.telemetryContainer__header{
            background-color: #000;
            border: none;
        }
        .telemetryContent{
            background: #222222;
        }
    }

    .row-1{
        height: 8.3%;
        float: left;
    }
    .row-2{
        height: 16.6%;
        float: left;
    }
    .row-9{
        height: 75%;
        float: left;
    }
    .progress_wrap {
        width: 100%;
        position: relative;
        z-index: 1;

        .progressbar {
            width: 75%;

            li {
                float: left;
                width: 32%;
                position: relative;
                text-align: left;
                color: #9F9F9F;
                list-style-type: none;
                text-indent: 12px;

                &.active {
                    font-weight: 600;
                    color: #fff;
                }

                &:after {
                    content: '>';
                    position: absolute;
                    left: 90%;
                }

                &:last-child:after {
                    content: '';
                }
            }
        }

        .progressbar_operations {
            float: right;
            button {
                margin: 0 5px;
            }
        }

        .progressbar_detail {
            text-indent: 12px;
            font-weight: 500;
            font-size: 8px;
            line-height: 10px;
            color: #E9E9E9;
            margin-bottom: 8px;
        }
    }
    .hide{
        display: none;
    }
    input[type=checkbox]
    {
        position: absolute;
        left: 15px;
        top: 25px;
        cursor: pointer;
        background: #DADADA;
        height:40px;
        width: 40px;
        border: #9F9F9F;
        display:inline-block;
        padding: 0 0 0 0px;
    }
    input[type=checkbox]:checked
    {
        background: #0080FF;
        display:inline-block;
        padding: 0 0 0 0px;
    }
    .subgraphs {
        position:relative;
    }
    .analysis{
        padding-left: 30px;
        input {
            margin: 5px auto;
            background-color: #585858;
            border-radius: 6px;
        }
        input[type=text]{
            width: 90%;
        }
    }
    .graphArea{
        position: relative;
    }
    .d3__timeline{
        position: absolute;
        top: -40px;
    }

</style>
