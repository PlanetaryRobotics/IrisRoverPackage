<template>
    <div class="home">
<!--         <Telemetry></Telemetry>-->
<!--        <TelemetrySub></TelemetrySub>-->
        <ErrorAnalysis></ErrorAnalysis>
    </div>
</template>

<script>
    // import Telemetry from '@/components/Telemetry/Telemetry.vue'
    // import TelemetrySub from '@/components/Telemetry/TelemetrySub.vue'

    import { mapState } from 'vuex';
    import ErrorAnalysis from "../../components/Telemetry/ErrorAnalysis";

    export default {
        name: 'home',
        components: {
            ErrorAnalysis,
            // Telemetry,
            // TelemetrySub
        },
        computed: {
            ...mapState({
                headers: state => state.headers,
            })
        }
    }
</script>

<style lang="scss">
    #app{
        height:100%;
    }
    .home{
        height:100%;
        overflow: hidden;
    }

</style>
