<template id="errorTag">
    <div class="errorTag">
        <div class="row errorTag--title" @click="toggleDetail">
            <svg width="12" height="16" class="">
                <rect v-for="i in this.priorityValue"
                      width="2"
                      height="14"
                      style="fill: #f00;"
                      :x=(i-1)*4
                />
            </svg>
            <div class="tagName">{{errorTag.id}}</div>
            <div class="col category align-self-end" :class="this.opened ? 'active' : ''">
                Category: {{errorTag.category}}
                <span class="arrow" ></span>
            </div>
        </div>
        <div class="row">
            <span>Sensor Name({{errorTag.name.length}}): </span> {{errorTag.name}}
        </div>
        <div class="row">
            <span>Time Frame (M+): </span> {{errorTag.timeFrame}}
        </div>

        <div class="details" :class="this.opened ? 'open' : 'collapse'" >
            <div class="row"><span>Summary: </span> {{errorTag.summary}}</div>
            <div class="row"><span>Notes: </span> {{errorTag.notes}}</div>
            <div class="row"><span>Tags: </span> {{errorTag.tags}}</div>
            <div class="row"><span>Created: </span> {{errorTag.timeFrame[0]}}</div>
            <div class="row"><span>Last modified: </span> {{errorTag.timeFrame[0]}}</div>
        </div>

        <div class="row viewButton" @click="viewAnalysis(errorTag)">
            <span >VIEW ANALYSIS</span>
        </div>



    </div>
</template>

<script>
    export default {
        name: "ErrorTag",
        props:['errorTag','openAnalysis', 'index'],
        mounted() {
          this.setPriority();
        },
        methods:{
            viewAnalysis(errorTag){
                this.$emit('pushTag', );
            },
            setPriority(){
                if (this.errorTag.priority === 'Low') {
                    this.priorityValue = 1;
                }
                else if (this.errorTag.priority === 'Mid') {
                    this.priorityValue = 2;
                }
                else {
                    this.priorityValue = 3;
                }
            },
            toggleDetail(){
                this.opened = !this.opened
            }
        },
        data(){
            return {
                priorityValue: 0,
                opened: false
            }
        }
    }
</script>

<style scoped lang="scss">
    @import '@/styles/_functional.scss';
    .errorTag {
        position: relative;
        border-radius: 4px;
        width: 95%;
        padding-left: 16px;

        font-size: 1.1rem;
        line-height: 12px;
        margin-bottom: 8px;

        .row {
            margin-bottom: 8px;
        }
        .errorTag--title {
            margin-left: -16px;
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

        .tagName {
            color: #FF2F46;
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
</style>
