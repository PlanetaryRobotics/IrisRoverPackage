'use strict'

import path from 'path'
import { app, protocol, BrowserWindow } from 'electron'
import { initSplashScreen, OfficeTemplate } from 'electron-splashscreen';
import {
  createProtocol,
  installVueDevtools
} from 'vue-cli-plugin-electron-builder/lib'
const isDevelopment = process.env.NODE_ENV !== 'production'

// Keep a global reference of the window object, if you don't, the window will
// be closed automatically when the JavaScript object is garbage collected.
let win;
let hideSplashscreen;

const ICON =
  process.platform == 'darwin' ? path.join(__dirname, 'assets/icons/mac/logo.png.hqx')
  : process.platform == 'win32' ? path.join(__dirname, 'assets/icons/win/logo.png.ico')
  : path.join(__dirname, 'assets/icons/png/64x64.png');

// Standard scheme must be registered before the app is ready
protocol.registerStandardSchemes(['app'], { secure: true })
function initProgram(){
  // Determine if a splash-screen should be skipped:
  let no_splash = false; // default
  // Process command-line arguments:
  let argv = JSON.parse(process.env.npm_config_argv).remain; // grab unused arguments
  let keys = argv.filter((v,i) => !(i % 2)); // grab keys
  let vals = argv.filter((v,i) => i % 2); // grab values associated with keys
  let splashlessIdx = keys.indexOf("splashless"); // Index of db-mission value
  if(splashlessIdx != -1){
    if( parseInt(vals[splashlessIdx]) ){
      no_splash = true;
    }
  }

  let windowOptions = {
    width: 1300,
    height: 800,
    minWidth: 869,
    minHeight: 580,
    icon: ICON,
    show: no_splash // only hide if a splash-screen is shown
  }
  if(!isDevelopment){
    windowOptions = Object.assign(windowOptions, {
      frame: false,
      show: false // don't show until content rendered (handled by Login.vue)
    });
  }

  // Create the browser window.
  win = new BrowserWindow(windowOptions);

  // Setup Splash Screen
  hideSplashscreen = initSplashScreen({
    mainWindow: win,
    icon: ICON,
    url: OfficeTemplate,
    width: 500,
    height: 300,
    brand: '',
    productName: 'Iris Rover',
    logo: path.join(__dirname, 'assets/iris_icon_multisize.ico'), // TODO: Doesn't show logo
    website: '',
    text: 'Initializing . . .'
  });

  if(no_splash){
    hideSplashscreen();
  }
}

function renderWindow() {
  if (process.env.WEBPACK_DEV_SERVER_URL) {
    // Load the url of the dev server if in development mode
    win.loadURL(process.env.WEBPACK_DEV_SERVER_URL)
  } else {
    createProtocol('app')
    // Load the index.html when not in development
    win.loadURL('app://./index.html')
  }
  if (isDevelopment && !process.env.IS_TEST) win.webContents.openDevTools()

  win.on('closed', () => {
    win = null
  })
}

// Quit when all windows are closed.
app.on('window-all-closed', () => {
  // On macOS it is common for applications and their menu bar
  // to stay active until the user quits explicitly with Cmd + Q
  if (process.platform !== 'darwin') {
    app.quit()
  }
})

app.on('activate', () => {
  // On macOS it's common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (win === null) {
    createWindow()
  }
})

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.on('ready', async () => {
  initProgram();
  if (1 || isDevelopment && !process.env.IS_TEST) {
    // Install Vue Devtools
    await installVueDevtools()
  }
  renderWindow();
});

// Exit cleanly on request from parent process in development mode.
if (isDevelopment) {
  if (process.platform === 'win32') {
    process.on('message', data => {
      if (data === 'graceful-exit') {
        app.quit()
      }
    })
  } else {
    process.on('SIGTERM', () => {
      app.quit()
    })
  }
}
