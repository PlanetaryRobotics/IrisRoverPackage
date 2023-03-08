<template>
  <div class="fleur-div">
    <div id="screen">
      <div id="component">
        <!-- left pane contains a list of active faults -->
        <div id="component-left">
          <div id="component-left-spacer">
            <!-- DYNAMIC FAULT COMPONENTS -->
          </div>
        </div>

        <!-- center pane contains fault indication boxes -->
        <div id="component-center">
          <div id="component-center-box">
            <div id="center-box-top">
              <div
                id="fault-zone-1"
                class="fault-zone component-center-box-hover"
              />
              <div
                id="fault-zone-2"
                class="fault-zone component-center-box-hover"
              />
              <div
                id="fault-zone-3"
                class="fault-zone component-center-box-hover"
              />
            </div>
            <div id="center-box-middle">
              <div class="center-box-middle-sides">
                <div
                  id="fault-zone-4"
                  class="fault-zone component-center-box-hover"
                />
                <div
                  id="fault-zone-5"
                  class="fault-zone component-center-box-hover"
                />
              </div>
              <div id="center-box-middle-center" />
              <div class="center-box-middle-sides">
                <div
                  id="fault-zone-6"
                  class="fault-zone component-center-box-hover"
                />
                <div
                  id="fault-zone-7"
                  class="fault-zone component-center-box-hover"
                />
              </div>
            </div>
            <div id="center-box-bottom">
              <div
                id="fault-zone-8"
                class="fault-zone component-center-box-hover"
              />
              <div
                id="fault-zone-9"
                class="fault-zone component-center-box-hover"
              />
              <div
                id="fault-zone-10"
                class="fault-zone component-center-box-hover"
              />
            </div>
          </div>
        </div>

        <!-- right pane contains the directors notebook -->
        <div id="component-right">
          <div id="component-right-spacer">
            <div id="fault-hover-box">
              <div id="fault-hover-box-header">
                <div id="component-left-item-header-spacer">
                  <div>
                    <div id="fault-hover-box-header-title">
                      Hover a fault
                    </div>
                    <div class="fault-hover-box-header-title-spacer " />
                    <div id="item-header-title-flowlink">
                      <a @click="openModal">Check&nbsp;flow</a>
                    </div>
                  </div>
                  <div>
                    <div id="item-header-title-status">
                      Status:&nbsp;&nbsp;<span style="color: green;">OK</span>
                    </div>
                    <div class="item-header-title-spacer" />
                    <div id="item-header-title-time">
                      Time:&nbsp;--:--
                    </div>
                  </div>
                </div>
              </div>
              <div id="fault-hover-box-body">
                <div id="fault-hover-box-body-spacer">
                  <div id="fault-hover-body-description">
                    Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Magna eget est lorem ipsum dolor.
                  </div>
                  <div id="fault-hover-body-content">
                    Morbi tincidunt ornare massa eget egestas purus viverra accumsan in. Duis ut diam quam nulla porttitor massa id.
                  </div>
                </div>
              </div>
            </div>
            <textarea id="notebook" />
          </div>
        </div>
      </div>
    </div>

    <!-- popup modal -->
    <div
      v-show="modalDisplay"
      id="nodeModal"
      class="modal"
    >
      <div class="modal-content">
        <span
          class="close"
          @click="closeModal"
        >&times;</span>
        <p id="modal-content" /><div id="chart" />
      </div>
    </div>
  </div>
</template>

<script>
import {join} from 'path';
/* global forceRefresh */

export default {
    name: 'FTA',
    data() {
        return {
            modalDisplay: false
        };
    },
    async mounted() {
        if (!this.$store.state.fleurInitialized) {
            // otherwise diagram.js can't find d3
            if (!global.d3) {
                global.d3 = require('d3');
            }

            const fleurRoot = join('FLEUR', 'static');
            const scripts = [
                join(fleurRoot, 'assets/js/diagram.js'),
                'https://cdnjs.cloudflare.com/ajax/libs/socket.io/3.1.3/socket.io.min.js',
                join(fleurRoot, 'assets/js/fleur_display_socket.js'),
                join(fleurRoot, 'assets/js/fleur_display.js')
            ];
            for (const url of scripts) {
                await this.$loadScript(url);
            }

            const fontlink1 = document.createElement('link');
            fontlink1.rel = 'preconnect';
            fontlink1.href = 'https://fonts.googleapis.com';
            document.head.appendChild(fontlink1);

            const fontlink2 = document.createElement('link');
            fontlink2.rel = 'preconnect';
            fontlink2.href = 'https://fonts.gstatic.com';
            fontlink2.crossOrigin = 'true';
            document.head.appendChild(fontlink2);

            // define function for fleur script
            global.openModal = this.openModal;

            // trigger fleur script
            window.onload();

            this.$store.commit('fleurInitialized');
        } else {
            // in fleur script
            forceRefresh();
        }
    },
    methods: {
        openModal () {
            this.modalDisplay = true;
        },
        closeModal () {
            this.modalDisplay = false;
        }
    }
};
</script>

<style>
  @keyframes fadeIn {
    0% { opacity: 0; }
    100% { opacity: 1; }
  }

  /* fleur root */
  .fleur-div {
    width: 100%;
    height: 100%;
    padding: 0;
    margin: 0;
    opacity: 1;
    background: #121212;
    animation: fadeIn 5s;
  }

  /* remove all styles from outside fleur */
  .fleur-div * {
    all: revert;
  }

  /* START POPUP MODAL */
  .fleur-div .modal {
    position: fixed;
    z-index: 1;
    padding-top: 20px;
    left: 0;
    top: 0;
    width: 100%;
    height: 100%;
    overflow: auto;
    background-color: rgb(0,0,0);
    background-color: rgba(0,0,0,0.4);
  }

  .fleur-div .modal-content {
    width: 85%;
    height: 80%;
    background-color: rgb(60, 60, 60);
    background-color: rgba(33, 33, 33, 0.9);
    margin: auto;
    padding: 20px;
    border: 2px solid rgb(30, 30, 30);
    color: white;
  }

  .fleur-div .close {
    color: #aaaaaa;
    float: right;
    font-size: 28px;
    font-weight: bold;
  }

  .fleur-div .close:hover,
  .fleur-div .close:focus {
    color: white;
    text-decoration: none;
    cursor: pointer;
  }

  /* END POPUP MODAL */


  /* START SANKEY DIAGRAM */

  .fleur-div #chart {
    width: 100%;
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: center;
  }
  .fleur-div .node rect {
    pointer-events: all;
    cursor: move;
    fill-opacity: .9;
    shape-rendering: optimizeSpeed ;
    border: 2px solid black;
  }
  .fleur-div .node text {
    pointer-events: none;
    fill: white;
    font-family: 'Space Grotesk', sans-serif;
    font-weight: 500;
    text-shadow: 0 1px 0 #0000;
  }
  .fleur-div .link {
    fill: none;
    stroke: limegreen;
    stroke-opacity: .4;
    cursor: pointer;
  }
  .fleur-div .link:hover {
    stroke-opacity: .5;
  }

  /* END SANKEY DIAGRAM */



  /* START MAIN INTERFACE */
  .fleur-div #screen {
    width: 100%;
    height: 100%;
    display: flex;
    flex-flow: column nowrap;
    align-items: center;
    justify-content: center;
    background-color: #121212
  }

  .fleur-div #header {
    flex: 1;
    width: 100%;
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: center;
    border-bottom: 1px solid #2a2a2a;

  }
  .fleur-div #back-box {
    margin-left: 3%;
    display: flex;
    justify-content: left;
  }
  .fleur-div #back-box > a {
    font-size: 4vh;
    color: lightblue;
    text-decoration: none;
  }
  .fleur-div #status-box {
    color: white;
    font-size: 1.5vh;
    margin-right: 3%;
  }
  .fleur-div #status-box-value {
  }
  .fleur-div #header-center{
    flex: 1;
    width: 100%;
    height: 80%;
    display: flex;
    align-items: center;
    justify-content: center;
  }
  .fleur-div .phase {
    flex: 1;
    width: 100%;
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: center;
  }
  .fleur-div .phase > button {
    width: 70%;
    height: 70%;
    outline: 1px solid lightblue;
    border-radius: 10px;
    border-color: #121212;
    font-size: 1.6vh;
    color: lightblue;
    background: #121212;

    /* better font here */
    font-family: 'Share Tech Mono', monospace;
    font-weight: 600;
  }
  .fleur-div .phase > button:hover {
    outline: 2px solid orange;
    cursor: pointer;
  }
  .fleur-div #component {
    flex: 18;
    width: 100%;
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: center;
  }
  .fleur-div #component-left {
    flex: 3;
    width: 100%;
    height: 100%;
    display: flex;
    flex-flow: column nowrap;
    align-items: center;
    justify-content: center;
    font-family: 'Roboto', sans-serif;
  }
  .fleur-div #component-left-spacer {
    width: 90%;
    height: 90%;
    align-items: center;
    justify-content: flex-start;
    border: 1px solid #2a2a2a;
    border-right: 0px;
    overflow-y: auto;
    padding-left: 3%; /* adjust for scrollbar */

    border-radius: 10px 0px 0px 10px;
  }
  .fleur-div #component-left-spacer::-webkit-scrollbar {
    width: 4px;
    background-color: transparent;
  }
  .fleur-div #component-left-spacer::-webkit-scrollbar-track {
    background-color: rgb(21, 21, 21);
  }

  .fleur-div #component-left-spacer::-webkit-scrollbar-thumb {
    background-color: #2a2a2a;
    border-radius: 0px 0px 0px 0px;
  }

  .fleur-div .component-left-item {
    width: 95%;
    height: 25%;
    margin-top: 3%;
    display: flex;
    flex-flow: column nowrap;
    align-items: center;
    justify-content: center;
    outline: 1px solid rgb(193, 193, 193);
    background-color: rgb(8, 8, 8);
    color: rgb(193, 193, 193);
    /*filter: contrast(95%);*/
    border: 2px solid transparent;
  }
  .fleur-div .component-left-item:hover {
    cursor: pointer;
    /*filter: contrast(300%);*/
    outline: 2px solid lightblue !important;
  }
  .fleur-div #component-left-item-header {
    flex: 1;
    width: 100%;
    height: 20%;
    display: flex;
    align-items: center;
    justify-content: center;
  }
  .fleur-div #component-left-item-header-spacer {
    width: 95%;
    height: 100%;
    display: flex;
    flex-flow: column nowrap;
    align-items: center;
    justify-content: center;
    border-bottom: 1px solid lightblue;
  }
  .fleur-div #component-left-item-header-spacer > div {
    flex: 1;
    width: 100%;
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: center;
  }
  .fleur-div .item-header-title-spacer {
    flex: 1;
  }
  .fleur-div #item-header-title {
    flex: 1;
    width: 100%;
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: left;
    font-size: 1.8vh;
  }
  .fleur-div #item-header-title-status {
    flex: 1;
    width: 100%;
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: left;
    font-size: 1.5vh;
  }
  .fleur-div #item-header-title-flowlink {
    flex: 1;
    width: 100%;
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: right;
    font-size: 1.5vh;
  }
  .fleur-div #item-header-title-flowlink > a,
  .fleur-div #item-header-title-flowlink > a:visited {
    text-decoration: none;
    color: rgb(89, 186, 218);
  }
  .fleur-div #item-header-title-flowlink > a:hover {
    color: orange;
  }
  .fleur-div #item-header-title-time {
    flex: 1;
    width: 100%;
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: right;
    font-size: 1.5vh;
  }
  .fleur-div #component-left-item-body {
    flex: 2;
    width: 100%;
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: center;
  }
  .fleur-div #component-left-item-body-spacer {
    width: 95%;
    height: 85%;
    display: flex;
    flex-flow: column nowrap;
    align-items: center;
    justify-content: center;
    /* border: 1px solid rgb(21, 21, 21); */
  }
  .fleur-div #item-body-description {
    flex: 1;
    width: 100%;
    height: 100%;
    font-size: 1.2vh;
  }
  .fleur-div #item-body-content {
    flex: 1;
    width: 100%;
    height: 100%;
    font-size: 1.2vh;
  }

  .fleur-div #component-center {
    flex: 4;
    width: 100%;
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: center;
  }
  .fleur-div #component-center-box {
    width: 85vh;
    height: 85vh;
    display: flex;
    flex-flow: column nowrap;
    align-items: center;
    justify-content: center;
    /* border: 2px solid lightblue; */
  }
  .fleur-div .component-center-box-hover:hover {
    filter: drop-shadow(6px 6px 8px lightblue) invert(10%);
    cursor: pointer;
  }
  .fleur-div .fault-zone {
    flex: 1;
    width: 100%;
    height: 100%;
    display: flex;
    flex-wrap: wrap;
    outline: 1px solid lightblue;
    /*noinspection CssUnknownTarget*/
    background: url('/FLEUR/static/assets/images/starfield.JPG');
    background-size: cover;

  }
  .fleur-div .fault-zone>* {
    flex: 3 3 17%; /* this needs some tuning */
    outline: .25px solid rgba(255, 255, 255, .5);
    transition: background 6s;
  }
  .fleur-div .fault-zone > div:hover {
    outline: 2px solid orange;
    z-index: 1;
  }
  .fleur-div #center-box-top {
    flex: 1;
    display: flex;
    align-items: center;
    justify-content: center;
    width: 100%;
    height: 100%;
  }
  .fleur-div #center-box-top > div:first-child {
    background-size: cover;
  }
  .fleur-div #center-box-middle {
    flex: 2;
    display: flex;
    align-items: center;
    justify-content: center;
    width: 100%;
    height: 100%;
  }
  .fleur-div .center-box-middle-sides {
    flex: 1;
    display: flex;
    flex-flow: column nowrap;
    align-items: center;
    justify-content: center;
    width: 100%;
    height: 100%;
  }
  .fleur-div #center-box-middle-center {
    flex: 1;
    width: 100%;
    height: 100%;
    outline: 1px solid lightblue;
    /*noinspection CssUnknownTarget*/
    background: url('/FLEUR/static/assets/images/rover-placeholder-image.JPG');
    background-size: cover;
  }

  .fleur-div #center-box-bottom {
    flex: 1;
    display: flex;
    align-items: center;
    justify-content: center;
    width: 100%;
    height: 100%;
  }

  .fleur-div #component-right {
    flex: 2;
    width: 100%;
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: center;
  }
  .fleur-div #component-right-spacer {
    width: 90%;
    height: 90%;
    display: flex;
    flex-flow: column nowrap;
    align-items: center;
    justify-content: center;
  }
  .fleur-div #fault-hover-box {
    flex: 2;
    width: 100%;
    height: 100%;
    display: flex;
    flex-flow: column nowrap;
    align-items: center;
    justify-content: center;
    outline: 2px solid #2a2a2a;
    margin-bottom: 10%;

    outline: 1px solid rgb(193, 193, 193);
    background: rgba(0, 26, 51, .2);
    background-size: cover;
    color: rgb(193, 193, 193);

    font-family: 'Roboto', sans-serif;
  }
  .fleur-div .fault-hover-box-spacer {
    width: 80%;
    height: 80%;
    display: flex;
    align-items: flex-start;
    justify-content: left;
  }
  .fleur-div #fault-hover-box-header {
    flex: 1;
    width: 100%;
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: center;
    font-size: 1.8vh;
    margin-top: 5%;
  }
  .fleur-div #fault-hover-box-body {
    flex: 8;
    width: 100%;
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: center;
    font-size: 1.5vh;
  }

  .fleur-div #notebook {
    flex: 3;
    width: 100%;
    height: 100%;
    background-color: rgb(21, 21, 21);
    color: white;
    resize: none;
  }
  .fleur-div #fault-hover-box-header {
    flex: 1;
    width: 90%;
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: center;
  }

  .fleur-div #fault-hover-box-header-title {
    flex: 1;
    width: 100%;
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: left;
    font-size: 1.8vh;
  }
  .fleur-div #fault-hover-box-body-spacer {
    width: 95%;
    height: 85%;
    display: flex;
    flex-flow: column nowrap;
    align-items: center;
    justify-content: center;
  }
  .fleur-div #fault-hover-box-body {
    flex: 2;
    width: 90%;
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: center;
  }
  .fleur-div #fault-hover-body-description {
    flex: 1;
    width: 100%;
    height: 100%;
    font-size: 1.2vh;
  }
  .fleur-div #fault-hover-body-content {
    flex: 1;
    width: 100%;
    height: 100%;
    font-size: 1.2vh;
  }
  /* END MAIN INTERFACE */
</style>