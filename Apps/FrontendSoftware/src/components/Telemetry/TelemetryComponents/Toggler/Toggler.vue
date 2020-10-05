<template>
    <div class="toggle__container">
        <span class="toggle__container">
            <button class="toggleButton left"
                    :class="{selected:'A-Z' === selectedTab}"
                    @click="setTab('A-Z')">
                A-Z
            </button>
            <button class="toggleButton right"
                    :class="{selected:'Z-A' === selectedTab}"
                    @click="setTab('Z-A')">
                Z-A
            </button>
        </span>
    </div>
</template>

<script>
    export default {
        name: "Toggler",
        data() {
            return {
                selectedTab: "A-Z",
            }
        },
        methods:{
            setTab(tab){
                this.selectedTab = tab;
            }
        }
    }
</script>

<style lang="scss" scoped>
    .toggle__container{
        margin-left: 40px;
    }
    .toggleButton{
        background-color: #585858;
        color: white;
        padding: 0.5rem 1.5rem;
        border: none;
        margin: 0;

        &.selected{
            background-color:#8A6DFF;
        }
        &.left {
            border-radius: 5px 0 0 5px;
        }
        &.right {
            border-radius: 0 5px 5px 0;
        }
    }
</style>
