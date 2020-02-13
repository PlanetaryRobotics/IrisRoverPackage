<template>
  <div id="mapNavigationLeft">

      <!-- TOP OPTIONS -->
      <div class="topOptions">
        <svg class="icon" width="21" height="22" viewBox="0 0 21 22" fill="none" xmlns="http://www.w3.org/2000/svg">
          <path d="M5.7551 5.2785L8.20465 20.0049C8.3652 20.9701 9.67325 21.1542 10.0939 20.2708L12.2381 15.7685C12.4163 15.3943 12.8068 15.1688 13.22 15.2016L18.1912 15.5959C19.1666 15.6732 19.6612 14.4484 18.9056 13.8268L7.37687 4.34217C6.66331 3.75513 5.60349 4.36702 5.7551 5.2785Z" stroke="#F6F6F6" stroke-linejoin="round"/>
        </svg>

        <svg class="icon" width="17" height="17" viewBox="0 0 17 17" fill="none" xmlns="http://www.w3.org/2000/svg">
          <line x1="8.5" y1="4.5" x2="8.5" y2="12.5" stroke="#F6F6F6" stroke-linecap="round" stroke-linejoin="round"/>
          <line x1="4.5" y1="8.5" x2="12.5" y2="8.5" stroke="#F6F6F6" stroke-linecap="round" stroke-linejoin="round"/>
          <circle cx="8.5" cy="8.5" r="8" stroke="#F6F6F6"/>
        </svg>

        <svg class="icon" width="9" height="18" viewBox="0 0 9 18" fill="none" xmlns="http://www.w3.org/2000/svg">
          <line x1="4.5" y1="10.5" x2="4.5" y2="17.5" stroke="#F6F6F6" stroke-linecap="round" stroke-linejoin="round"/>
          <circle cx="4.5" cy="4.5" r="4" stroke="#F6F6F6"/>
        </svg>

        <svg class="icon" width="17" height="17" viewBox="0 0 17 17" fill="none" xmlns="http://www.w3.org/2000/svg">
          <line x1="0.707107" y1="13" x2="4" y2="16.2929" stroke="#F6F6F6" stroke-linecap="round"/>
          <line x1="16.2929" y1="4" x2="13" y2="0.707107" stroke="#F6F6F6" stroke-linecap="round"/>
          <line x1="3.25" y1="13.5429" x2="13.5429" y2="3.25" stroke="#F6F6F6" stroke-linecap="round"/>
        </svg>
      </div>

      <!-- BUFFER -->
      <div class="buffer"/>

      <!-- BOTTOM OPTIONS-->
      <div class="bottomOptions">
        <svg class="icon" @click = "togglePolarPlot" width="25" height="25" viewBox="0 0 25 25" fill="none" xmlns="http://www.w3.org/2000/svg">
          <circle cx="12.5" cy="12.5" r="7" stroke="#E3E3E3"/>
          <circle cx="12.5" cy="12.5" r="12" stroke="#E3E3E3"/>
          <circle cx="12.5" cy="12.5" r="2.5" fill="#E3E3E3"/>
        </svg>

        <svg class="icon" width="38" height="42" viewBox="0 0 38 42" fill="none" xmlns="http://www.w3.org/2000/svg">
          <rect width="38" height="42" rx="2" fill="#575757"/>
          <path d="M12.9375 34.0591L12.9375 10.4693L25.6648 10.4693L25.6648 34.0591L12.9375 34.0591Z" fill="#BEBEBE"/>
          <path d="M12.9375 34.0591L12.9375 10.4693L25.6648 10.4693L25.6648 34.0591L12.9375 34.0591Z" fill="url(#paint0_linear)"/>
          <rect x="9" y="8.05127" width="3.28125" height="5.92019" fill="#C1C1C1"/>
          <rect x="26.1818" y="8.1106" width="3.28125" height="5.92019" fill="#C1C1C1"/>
          <rect x="9" y="31.0513" width="3.28125" height="5.92019" fill="#C1C1C1"/>
          <rect x="26.1818" y="31.1104" width="3.28125" height="5.9202" fill="#C1C1C1"/>
          <path d="M17.9326 24.0769H18.3316L19.4866 22.3899H19.5006L20.6626 24.0769H21.0686L19.7036 22.1449L20.8656 20.4579H20.4666L19.4936 21.9069H19.4796L18.4786 20.4579H18.0726L19.2836 22.1519L17.9326 24.0769Z" fill="black"/>
          <mask id="path-8-inside-1" fill="white">
          <path fill-rule="evenodd" clip-rule="evenodd" d="M22.7074 7.72284L19.529 5.10254L16.3507 7.72284H22.7074ZM14.409 9.37391V9.32363L14.348 9.37391H14.409Z"/>
          </mask>
          <path fill-rule="evenodd" clip-rule="evenodd" d="M22.7074 7.72284L19.529 5.10254L16.3507 7.72284H22.7074ZM14.409 9.37391V9.32363L14.348 9.37391H14.409Z" fill="white"/>
          <path d="M19.529 5.10254L21.4374 2.78777L19.529 1.21446L17.6207 2.78777L19.529 5.10254ZM22.7074 7.72284V10.7228H31.0623L24.6157 5.40806L22.7074 7.72284ZM16.3507 7.72284L14.4424 5.40806L7.99574 10.7228H16.3507V7.72284ZM14.409 9.37391V12.3739H17.409V9.37391H14.409ZM14.409 9.32363H17.409V2.96227L12.5007 7.00886L14.409 9.32363ZM14.348 9.37391L12.4397 7.05914L5.99304 12.3739H14.348V9.37391ZM17.6207 7.41731L20.799 10.0376L24.6157 5.40806L21.4374 2.78777L17.6207 7.41731ZM18.2591 10.0376L21.4374 7.41731L17.6207 2.78777L14.4424 5.40806L18.2591 10.0376ZM16.3507 10.7228H22.7074V4.72284H16.3507V10.7228ZM17.409 9.37391V9.32363H11.409V9.37391H17.409ZM16.2564 11.6887L16.3174 11.6384L12.5007 7.00886L12.4397 7.05914L16.2564 11.6887ZM14.409 6.37391H14.348V12.3739H14.409V6.37391Z" fill="#00A3FF" mask="url(#path-8-inside-1)"/>
          <defs>
          <linearGradient id="paint0_linear" x1="12.7848" y1="34.0588" x2="30.0257" y2="14.6853" gradientUnits="userSpaceOnUse">
          <stop stop-color="#F4F4F4"/>
          <stop offset="1" stop-color="white" stop-opacity="0"/>
          </linearGradient>
          </defs>
        </svg>
      </div>
  </div>
</template>

<script>

export default {
  name: "MapNavigationLeft",
  methods: {
    togglePolarPlot() {
      this.$store.commit("togglePolarPlotButton");
    }
  }
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_functional.scss';

#mapNavigationLeft {
  z-index: 10;
  display: flex;
  flex-direction: column;

  .topOptions {
    display: flex;
    flex-direction: column;
    justify-content: center;
    align-items: center;
  }
  .buffer {
    flex-grow: 1;
  }
  .bottomOptions {
    display: flex;
    flex-direction: column;
    justify-content: center;
    align-items: center;

    >svg:last-child {
      margin-bottom: 2rem;
    }
  }
}

.icon {
  margin-top: 2rem;

  &:hover {
    cursor: pointer;
    stroke: $color-primary;
  }
}

</style>
