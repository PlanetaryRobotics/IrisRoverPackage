<template>
    <div>
        <svg :view-box.camel="viewBox"
             preserveAspectRatio="xMidYMid meet"
             :width="width"
             :height="layout.height"
             class="static graph"
             :class="name">
            <Inner :layout="layout"
                   :chart-data="chartData"
                   :axes="axes"
                   :name="name"
                   :width="width"
                   :safezone="safezone">
            </Inner>
        </svg>
        <div class="tooltip" :class="name"></div>
    </div>

</template>

<script>

    import Inner from "./GraphComponents/Inner";
    import * as d3 from 'd3';


    export default {
        name: "Outer",
        components:{
            Inner,
        },
        props:['cData','layout','width','name','safezone'],
        computed:{
            // SVG viewbox
            viewBox: function() {
                let outerWidth = this.width + (this.layout.marginLeft || 20) + (this.layout.marginRight || 20),
                    outerHeight = this.layout.height + (this.layout.marginTop || 20) + (this.layout.marginBottom|| 20);
                return '0 0 ' + outerWidth + ' ' + outerHeight;
            },

        },
        data() {
            return {
                chartData: this.cData,
                axes: ['left', 'bottom'],
                axes2: ['bottom'],
                svg: null,
                focus:null,
                context:null,
                zoom:null,
                brush:null,
            }
        },
        mounted(){
            this.drawChart();
        },
        methods:{
            drawChart(){
                const groupName = '.static.'+this.name;
                this.svg = d3.selectAll(groupName); // the svg

                this.focus = d3.selectAll(groupName+" .focus")
                    .attr("transform", "translate(" + this.layout.marginLeft + "," + this.layout.marginTop + ")");
            },
        },
        watch: {
            width: {
                deep: true,
                handler: function() {
                }
            },
        },
    }

</script>

<style>
.tooltip {
    opacity: 1e-6;
}
.safezone {
    fill: #21DF84;
}

</style>
