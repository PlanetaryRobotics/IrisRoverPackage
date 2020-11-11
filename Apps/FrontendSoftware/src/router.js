import Vue from 'vue'
import Router from 'vue-router'

// Real Views:
import Login from './views/Login.vue'
import Home from './views/Home.vue'

// Sandboxes:
// import AtomicTest from './views/sandboxes/AtomicTest.vue' // NB: just importing this breaks the app ...
import MapTest from './views/sandboxes/MapTest.vue'
import POIDashboardTest from './views/sandboxes/POIDashboardTest';
import ImageViewerTest from './views/sandboxes/ImageViewerTest';
import HomeTestImgViewer from './views/sandboxes/HomeTestImgViewer.vue';

import { remote } from 'electron'

Vue.use(Router);

export default new Router({
  mode: 'hash', // history doesn't work consistently in production
  base: remote.process.env.BASE_URL,
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
    },
    {
      path: '/home_ImageViewerModsTest',
      name: 'home_ImageViewerModsTest',
      component: HomeTestImgViewer
    },
    {
      path: '/sandbox_mapTest',
      name: 'sandbox_mapTest',
      component: MapTest
    },
    {
      path: '/sandbox_POIDashboardTest',
      name: 'sandbox_POIDashboardTest',
      component: POIDashboardTest
    },
    {
      path: '/sandbox_imageViewerTest',
      name: 'sandbox_imageViewerTest',
      component: ImageViewerTest
    }
  ]
});
