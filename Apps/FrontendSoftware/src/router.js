import Vue from 'vue';
import Router from 'vue-router';

// Real Views:
import Login from './views/Login.vue';
import Home from './views/Home.vue';

// Sandboxes:
// import AtomicTest from './views/sandboxes/AtomicTest.vue' // NB: just importing this breaks the app ...
// import MapTest from './views/sandboxes/MapTest.vue';
// import POIDashboardTest from './views/sandboxes/POIDashboardTest';

import { process } from '@electron/remote';

Vue.use(Router);

export default new Router({
    mode: 'hash', // history doesn't work consistently in production
    base: process.env.BASE_URL,
    routes: [
        {
            path: '/',
            name: 'login',
            component: Login
        },
        {
            path: '/home',
            name: 'home',
            component: Home
        }
        // {
        //     path: '/sandbox_mapTest',
        //     name: 'sandbox_mapTest',
        //     component: MapTest
        // },
        // {
        //     path: '/sandbox_POIDashboardTest',
        //     name: 'sandbox_POIDashboardTest',
        //     component: POIDashboardTest
        // }
    ]
});
