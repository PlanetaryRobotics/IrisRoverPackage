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

            const stylesheetElem = document.createElement('link');
            stylesheetElem.rel = 'stylesheet';
            stylesheetElem.type = 'text/css';
            stylesheetElem.href = join(fleurRoot, 'assets/styles/fleur_display.css');
            document.head.appendChild(stylesheetElem);

            // trigger fleur scripts
            window.onload();

            this.$store.commit('fleurInitialized');
        } else {
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
  .fleur-div {
    width:100%;
    height:100%;
  }
</style>