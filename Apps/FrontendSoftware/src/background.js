'use strict'
/**
 * Manages the background Electron processes and the link between the Application 
 * Window and the OS. Notably, this includes invoking window creation on application 
 * startup and launching the renderer (Vue), by loading the app.
 * This is the first non-engine process invoked when the application is started (see `package.json`).
 * 
 * Author: Connor W. Colombo
 * Created: 1/2019
 * Last Updated: 10/01/2020
 */

/* global __static */
import path from 'path'

// Electron Dependencies:
import { app, protocol, BrowserWindow } from 'electron'
import { autoUpdater } from 'electron-updater'
import { initSplashScreen, OfficeTemplate } from 'electron-splashscreen';
import log from 'electron-log';
// autoUpdater.logger = log;
// autoUpdater.transports.file.level = 'info';

// Vue Dependencies:
import {
  createProtocol,
  installVueDevtools
} from 'vue-cli-plugin-electron-builder/lib'
const isDevelopment = process.env.NODE_ENV !== 'production'

// Handle all unhandled exceptions for diagnostics (and reporting):
const unhandled = require('electron-unhandled');
unhandled();

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
  if(process && process.env && process.env.npm_config_argv){
    let argv = JSON.parse(process.env.npm_config_argv).remain; // grab unused arguments
    let keys = argv.filter((v,i) => !(i % 2)); // grab keys
    let vals = argv.filter((v,i) => i % 2); // grab values associated with keys
    let splashlessIdx = keys.indexOf("splashless"); // Index of db-mission value
    if(splashlessIdx != -1){
      if( parseInt(vals[splashlessIdx]) ){
        no_splash = true;
      }
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
    logo: path.join(__static, 'icon.png'),
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
    // Check for Updates to the Github Releases Page:
    autoUpdater.checkForUpdatesAndNotify();
  }
  if (isDevelopment && !process.env.IS_TEST) win.webContents.openDevTools()

  win.on('closed', () => {
    win = null
  })
}

// Spawns the application:
async function spawn(){
  initProgram();
  if (isDevelopment && !process.env.IS_TEST) {
    // Install Vue Devtools
    await installVueDevtools()
  }
  renderWindow();
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
    spawn();
  }
})

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.on('ready', async () => {
  spawn();
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

// #### AUTO-UPDATE MESSAGES #### //

function sendUpdateMessage(msg){
  if(win){
    win.webContents.send('message', msg);
  }
}

autoUpdater.on('checking-for-update', () => {
  sendUpdateMessage('Checking for update...');
});

autoUpdater.on('update-available', () => {
  sendUpdateMessage('Update available!');
});

autoUpdater.on('update-not-available', () => {
  sendUpdateMessage('No updates available.');
});

autoUpdater.on('download-progress', (progress) => {
  sendUpdateMessage(`Download Speed: ${progress.bytesPerSecond}bps — ${progress.percent}% Complete — ${progress.transferred} / ${progress.total}`);
});

autoUpdater.on('update-downloaded', () => {
  sendUpdateMessage(`Update download complete. Installing now. App will close in 10s.`);
  setTimeout( () => {autoUpdater.quitAndInstall();}, 10000 );
  
});

autoUpdater.on('error', (err) => {
  sendUpdateMessage(`Error during auto-update. Info: ${err.toString()}`);
});
