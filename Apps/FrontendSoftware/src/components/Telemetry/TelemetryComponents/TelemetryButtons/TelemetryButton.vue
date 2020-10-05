<template>
    <button v-bind:style="{'background-color': module.color}"
            :id="module.id"
            class="button">
        {{ module.title }}
    </button>
</template>

<script>
    export default {
        name: "TelemetryButton",
        props: ['module'],
        methods: {
        }
    }
</script>

<style scoped>
    .button{
        margin: 15px 15px 15px 15px;
        padding: 3px 17px;
    }
</style>
