<template>
    <div class="tab-container">
        <div class="tabs is-centered">
            <ul>
                <li v-for="tab in tabs" :class="{'is-active' : tab.isActive}">
                    <a :href="tab.href" @click="selectTab(tab)">
                        <span class="text">{{tab.title}}</span>
                        <div class="cross" @click="removeTab(tab)"></div>
                    </a>
                </li>
            </ul>
        </div>

        <div class="tab-contents">
            <slot></slot>
        </div>
    </div>
</template>

<script>
    export default {
        name: "Tabs",
        data() {
            return {
                tabs: [],
            }
        },
        created() {
            this.tabs = this.$children;
        },
        methods: {
            selectTab(currentTab) {
                this.tabs.forEach(tab => {
                    tab.isActive = (tab.title === currentTab.title);
                });
            },
            removeTab(currentTab) {

                this.$emit('handleremove', currentTab.title);
            }
        },

    }
</script>

<style scoped lang="scss">
    @import '@/styles/_colors.scss';

.tabs {
    border-bottom: 1px solid $color-grey-dark;
    position: absolute;
    left: 25%;
    top: 74px;
    margin-left: 2px;
    /*margin-right: 46px;*/
    width: 74.8%;

    ul {
        display: inline-flex;
        li {
            list-style: none;
            padding: 0.5rem 0.5rem 0 0.5rem;
            margin-right: 4px;
            background-color: $color-background;
            border-radius: 4px 4px 0 0;
            &.is-active {
                background-color: $color-grey-dark;
                a {
                    color: $color-grey-light;
                }
            }
            a {
                color: $color-grey;
                text-decoration: none;
                display: inline-flex;

                .cross{
                    width:20px;
                    height:20px;
                    position:relative;
                    margin-left: 1.5rem;
                }
                .cross:before,.cross:after{
                    content:'';
                    position:absolute;
                    width:16px;
                    height:1px;
                    background-color: $color-grey-light;
                    top:8px;
                }
                .cross:before{
                    -webkit-transform:rotate(45deg);
                    -moz-transform:rotate(45deg);
                    transform:rotate(45deg);
                }
                .cross:after{
                    -webkit-transform:rotate(-45deg);
                    -moz-transform:rotate(-45deg);
                    transform:rotate(-45deg);
                }
            }

        }
    }
}
</style>
