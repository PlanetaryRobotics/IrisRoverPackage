<template>
    <div id = "map-container">
      <!-- GRID --> 
      <canvas id="map-canvas">
      </canvas>
      <!-- LANDER --> 
      <svg id = "lander" width="125" height="125" viewBox="0 0 125 125" fill="none" xmlns="http://www.w3.org/2000/svg">
        <path fill-rule="evenodd" clip-rule="evenodd" d="M70.3692 6.98312C68.4976 6.48162 66.4989 6.91488 65.0029 8.14642L41.9683 27.1092C41.9682 27.1093 41.9682 27.1093 41.9683 27.1094C41.9683 27.1094 41.9683 27.1095 41.9682 27.1095L14.0287 37.5766C12.2142 38.2564 10.8396 39.7707 10.3381 41.6423L6.98347 54.1617C6.48194 56.0334 6.9152 58.0321 8.14677 59.5281L23.879 78.6383C25.7431 80.9027 28.9712 81.4875 31.5112 80.021L62.2654 62.2651C62.2654 62.2651 62.2655 62.265 62.2654 62.265C62.2654 62.2649 62.2654 62.2649 62.2655 62.2648L93.0196 44.5089C95.5597 43.0424 96.6672 39.9545 95.6382 37.2079L86.9544 14.0282C86.2746 12.2137 84.7603 10.8391 82.8886 10.3376L70.3692 6.98312ZM117.547 70.3691C118.049 68.4974 117.616 66.4987 116.384 65.0027L100.652 45.8925C98.7877 43.628 95.5597 43.0432 93.0197 44.5097L62.2656 62.2656C62.2655 62.2656 62.2654 62.2656 62.2654 62.2655C62.2653 62.2655 62.2652 62.2654 62.2652 62.2655L31.5111 80.0214C28.971 81.4879 27.8634 84.5758 28.8924 87.3225L37.5762 110.502C38.256 112.317 39.7703 113.691 41.642 114.193L54.1614 117.547C56.0331 118.049 58.0317 117.615 59.5277 116.384L82.5622 97.4212C82.5623 97.4211 82.5624 97.4211 82.5625 97.4212C82.5625 97.4213 82.5626 97.4213 82.5627 97.4213L110.502 86.9541C112.317 86.2743 113.691 84.7601 114.193 82.8884L117.547 70.3691Z" fill="url(#paint0_angular)"/>
        <rect x="29.5847" y="80.9775" width="53.0658" height="53.0658" rx="6" transform="rotate(-75 29.5847 80.9775)" fill="url(#paint1_angular)"/>
        <defs>
        <radialGradient id="paint0_angular" cx="0" cy="0" r="1" gradientUnits="userSpaceOnUse" gradientTransform="translate(62.2654 62.2653) rotate(60) scale(45.5813 45.5815)">
        <stop stop-color="#A9A9A9"/>
        <stop offset="1" stop-color="#F2F2F2"/>
        </radialGradient>
        <radialGradient id="paint1_angular" cx="0" cy="0" r="1" gradientUnits="userSpaceOnUse" gradientTransform="translate(56.1176 107.51) rotate(-44.8958) scale(74.1023)">
        <stop stop-color="#FFD600"/>
        <stop offset="1" stop-color="#FFF9C2"/>
        </radialGradient>
        </defs>
      </svg>
      <!-- ROVER --> 
      <svg id = "rover" width="33" height="55" viewBox="0 0 33 55" fill="none" xmlns="http://www.w3.org/2000/svg">
        <path d="M6.1875 49.1006L6.1875 9.10059L26.1875 9.10059L26.1875 49.1006L6.1875 49.1006Z" fill="#BEBEBE"/>
        <path d="M6.1875 49.1006L6.1875 9.10059L26.1875 9.10059L26.1875 49.1006L6.1875 49.1006Z" fill="url(#paint0_linear)"/>
        <rect y="5" width="5.15625" height="10.0386" fill="#C1C1C1"/>
        <rect x="27" y="5.10059" width="5.15625" height="10.0386" fill="#C1C1C1"/>
        <rect y="44" width="5.15625" height="10.0386" fill="#C1C1C1"/>
        <rect x="27" y="44.1006" width="5.15625" height="10.0386" fill="#C1C1C1"/>
        <path d="M13.8131 32H14.4971L16.4771 29.108H16.5011L18.4931 32H19.1891L16.8491 28.688L18.8411 25.796H18.1571L16.4891 28.28H16.4651L14.7491 25.796H14.0531L16.1291 28.7L13.8131 32Z" fill="black"/>
        <mask id="path-7-inside-1" fill="white">
        <path fill-rule="evenodd" clip-rule="evenodd" d="M21.5402 4.44311L16.5457 0L11.5511 4.44311H21.5402ZM8.4999 7.24276V7.1575L8.40405 7.24276H8.4999Z"/>
        </mask>
        <path fill-rule="evenodd" clip-rule="evenodd" d="M21.5402 4.44311L16.5457 0L11.5511 4.44311H21.5402ZM8.4999 7.24276V7.1575L8.40405 7.24276H8.4999Z" fill="white"/>
        <path d="M16.5457 0L18.5396 -2.24144L16.5457 -4.01528L14.5517 -2.24144L16.5457 0ZM21.5402 4.44311V7.44311H29.4261L23.5341 2.20168L21.5402 4.44311ZM11.5511 4.44311L9.55716 2.20168L3.66526 7.44311H11.5511V4.44311ZM8.4999 7.24276V10.2428H11.4999V7.24276H8.4999ZM8.4999 7.1575H11.4999V0.473412L6.50591 4.91607L8.4999 7.1575ZM8.40405 7.24276L6.41007 5.00133L0.518178 10.2428H8.40405V7.24276ZM14.5517 2.24144L19.5462 6.68455L23.5341 2.20168L18.5396 -2.24144L14.5517 2.24144ZM13.5451 6.68455L18.5396 2.24144L14.5517 -2.24144L9.55716 2.20168L13.5451 6.68455ZM11.5511 7.44311H21.5402V1.44311H11.5511V7.44311ZM11.4999 7.24276V7.1575H5.4999V7.24276H11.4999ZM10.398 9.4842L10.4939 9.39894L6.50591 4.91607L6.41007 5.00133L10.398 9.4842ZM8.4999 4.24276H8.40405V10.2428H8.4999V4.24276Z" fill="#00A3FF" mask="url(#path-7-inside-1)"/>
        <defs>
        <linearGradient id="paint0_linear" x1="5.9475" y1="49.1002" x2="35.357" y2="18.4739" gradientUnits="userSpaceOnUse">
        <stop stop-color="#F4F4F4"/>
        <stop offset="1" stop-color="white" stop-opacity="0"/>
        </linearGradient>
        </defs>
      </svg>
    </div>
</template>

<script>

import $ from 'jquery'

export default {
  name: "Grid",
  data() {
    return {
      lander: {
        x: 0,
        y: 0,
      },
    }
  },
  mounted() {
    window.addEventListener('resize', this.handleResize);
    this.handleResize();
  },
  methods: {
    handleResize() {
      this.drawGrid();
      this.drawLander();
      this.drawRover();
    },
    drawLander() {
      let svg = $('#lander')[0];
      let left = this.lander.x - svg.getBBox().width/2;
      let top = this.lander.y - svg.getBBox().height/2;
 
      $('#lander').css('left', left + "px");
      $('#lander').css('top', top + "px");
    },
    drawRover() {
      // TODO: Hardcode for now, needs to be retrieved from store
      let Y_DISPLACEMENT = $('#rover')[0].getBBox().height * -3 - $('#rover')[0].getBBox().height/2;
      let X_DISPLACEMENT = 0;

      let svg = $('#rover')[0];
      let left = this.lander.x - svg.getBBox().width/2;
      let top = this.lander.y + Y_DISPLACEMENT - svg.getBBox().height/2;
 
      $('#rover').css('left', left + "px");
      $('#rover').css('top', top + "px");
    },
    drawGrid() {

      var component = this;

      // Use height of rover
      var size = $('#rover')[0].getBBox().height;
      var padding = 0;

      var parent = $("#map-container")[0];
      var canvas = document.getElementById("map-canvas");
      var context = canvas.getContext("2d");

      // Hide the canvas so we can get the parent's responsive bounds
      var displayBackup = canvas.style.display;
      canvas.style.display = "none";

      // Get parent width WITHOUT canvas
      canvas.width = parent.clientWidth;
      canvas.height = parent.clientHeight;

      // Redisplay canvas
      canvas.style.display = displayBackup;

      // For reset purposes
      context.clearRect(0,0,canvas.width,canvas.height);

      function drawBoard(){
        //Verticals
        for (var x = size; x <= canvas.width; x += size) {
            context.strokeStyle = "white";
            if (x >= (canvas.width/2) && x < (canvas.width/2 + size)){
              context.lineWidth = 3;
              component.lander.x = x;
            } else {
              context.lineWidth = 1;
            }
            context.beginPath();
            context.moveTo(0.5 + x + padding, padding);
            context.lineTo(0.5 + x + padding, canvas.height);
            context.stroke();
        }

        // Horizontals
        for (var x = size; x <= canvas.height; x += size) {
            context.strokeStyle = "white";
            if (x >= (canvas.height/2) && x < (canvas.height/2 + size)){
              context.lineWidth = 3;
              component.lander.y = x;
            } else {
              context.lineWidth = 1;
            }
            context.beginPath();
            context.moveTo(padding, 0.5 + x + padding);
            context.lineTo(canvas.width, 0.5 + x + padding);
            context.stroke();
        }
      }
      drawBoard();
    }
  }
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_functional.scss';

#lander, #rover {
  position: absolute;
  left: 0px; 
  top:0px;
}

#lander {
  z-index: 1
}

#rover {
  x-index: 2
}

#map-canvas {
  background: $color-near-black;
}

#map-container {
  height: 100%;
  position: relative;
  padding: 2px;
}

</style>
