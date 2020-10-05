<template>
    <div class="toggle__container">
        <span class="toggle__container">
            <button class="toggleButton left"
                    :class="{selected:'Low' === selectedTab}"
                    type="button"
                    @click="setTab('Low')">
                Low
            </button>
            <button class="toggleButton middle"
                    :class="{selected:'Medium' === selectedTab}"
                    type="button"
                    @click="setTab('Medium')">
                Medium
            </button>
            <button class="toggleButton right"
                    :class="{selected:'High' === selectedTab}"
                    type="button"
                    @click="setTab('High')">
                High
            </button>
        </span>
    </div>
</template>

<script>
    export default {
        name: "PriorityToggler",
        data() {
            return {
                selectedTab: "Low",
            }
        },
        methods:{
            setTab(tab){
                this.selectedTab = tab;
                this.$emit('selectTab', tab);
            }
        }
    }
</script>

<style lang="scss" scoped>
    .toggle__container{
        margin-left: 10px;
    }
    .toggleButton{
        background-color: #585858;
        color: white;
        padding: 0.5rem 1.5rem;
        border: none;
        margin: 5px auto;
        font-size: 8px;

        &.selected{
            background-color:#8A6DFF;
        }
        &.left {
            border-radius: 5px 0 0 5px;
            &::before{
                background-color: #FFD039;
            }
        }
        &.middle {
            border-radius: 0;
            &::before{
                background-color: #FF6B00;
            }
        }
        &.right {
            border-radius: 0 5px 5px 0;
            &::before{
                background-color: #FF2F46;
            }
        }
        &::before {
            margin-right: 5px;
            content: '';
            display: inline-block;
            width: 7px;
            height: 7px;
            -moz-border-radius: 7.5px;
            -webkit-border-radius: 7.5px;
            border-radius: 7.5px;

        }
    }
</style>
