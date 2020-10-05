<template>
    <div class="tdrover">
        <canvas :id="this.RoverID[1]"></canvas>
        <canvas :id="this.RoverID[0]"></canvas>
    </div>
</template>

<script>
    export default {
        name: "TDRover",
        props:['RoverID'],
        data(){
            return{
                tdRover: null,
                tdBack: null,
                rectAngle: 0,
                x:0
            }
        },
        mounted(){
            this.initRover();
            this.drawBack();
            this.drawRover(0);
        },
        methods:{
            initRover(){
                const id = this.RoverID[0];
                const backid = this.RoverID[1];

                // foreground
                const c = document.getElementById(id);
                const ct = c.getContext("2d");
                this.tdRover = ct;
                this.tdRover.strokeStyle = "#fff";

                // background
                let b = document.getElementById(backid);
                const bt = b.getContext("2d");
                this.tdBack = bt;

                this.tdBack.lineWidth = 1;
                this.tdBack.font ="bold 11px sans-serif";
            },
            drawBack(){

                // clear canvas
                this.tdBack.clearRect(0, 0, 140, 140);

                // draw coordinates
                this.tdBack.fillStyle = "#ffffff";
                this.tdBack.strokeStyle = "#ffffff";
                this.drawCoordinates();

                // fill rect
                this.tdBack.fillStyle = "#9F9F9F";
                this.tdBack.fillRect(30, 50, 60, 30);

            },
            drawRover(angle){
                // clear canvas
                this.tdRover.clearRect(0, 0, 140, 140);

                // draw rect
                this.tdRover.translate(30,80);
                this.tdRover.rotate(angle  * Math.PI / 180);
                this.tdRover.translate(-30,-80);

                this.tdRover.beginPath();
                this.stroke(this.tdRover, 30,50,30,80); // left kk
                this.stroke(this.tdRover, 90,50,30,50); // top kk
                this.stroke(this.tdRover, 90,80,90,50); // right kk
                this.stroke(this.tdRover, 30,80,90,80); // bottom

                this.tdRover.closePath();
                this.tdRover.stroke();
            },
            drawCoordinates(){
                this.tdBack.beginPath();
                // x
                this.stroke(this.tdBack, 30,80,120,80);

                // x-axis arrow
                this.stroke(this.tdBack,115,77,120,80);
                this.stroke(this.tdBack,115,83,120,80);

                // y
                this.stroke(this.tdBack,30,80,30,0);

                // y-axis arrow
                this.stroke(this.tdBack,27,5,30,0);
                this.stroke(this.tdBack,33,5,30,0);

                this.tdBack.textAlign = "right";
                this.tdBack.textBaseline = "top";
                this.tdBack.fillText("Tilt Angel", 120, 0);

                // x sign
                this.tdBack.fillText("x", 118, 60);

                // y/z sign
                if(this.RoverID[0] === 'xz'){
                    this.tdBack.fillText("z", 20, 5);
                    this.tdBack.fillText("front", 55,85);
                } else{
                    this.tdBack.fillText("y", 20, 5);
                    this.tdBack.fillText("side", 55,85);
                }

                const angle = this.rectAngle;
                this.tdBack.fillText(angle, 120, 12);

                this.tdBack.closePath();
                this.tdBack.stroke();
            },
            stroke(canvas,x1,y1,x2,y2){
                canvas.moveTo(x1,y1);
                canvas.lineTo(x2,y2);
            },
            resetRover(){
                this.drawRover(-this.rectAngle);
                this.rectAngle = 0;
            },
            subAngle() {
                this.rectAngle+=5;
                this.drawRover(5);
            },
            addAngle(){
                this.rectAngle-=5;
                this.drawRover(-5);
            }

        }
    }
</script>

<style>
    .tdrover {
        height: 140px;
        width: 140px;
        margin-left: 20px;
        margin-top: 20px;
        /*border: 1px solid gray;*/
    }
    #backxz, #backyz{
        position: absolute;
        top: 0;
        left: 0;
    }
    #xz, #yz{
        position: relative;
        margin: 0;
    }

</style>
