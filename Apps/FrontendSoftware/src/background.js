'use strict';
/**
 * Manages the background Electron processes and the link between the Application 
 * Window and the OS. Notably, this includes invoking window creation on application 
 * startup and launching the renderer (Vue), by loading the app.
 * This is the first non-engine process invoked when the application is started (see `package.json`).
 * 
 * Author: Connor W. Colombo
 * Created: 1/2019
 * Last Updated: 10/07/2022
 */

/* global __static */
import path from 'path';

// Electron Dependencies:
import { app, protocol, BrowserWindow, ipcMain } from 'electron';
import { autoUpdater } from 'electron-updater';
// import log from 'electron-log';
// autoUpdater.logger = log;
// autoUpdater.transports.file.level = 'info';

// Setup remote link:
require('@electron/remote/main').initialize();

// Vue Dependencies:
import {createProtocol} from 'vue-cli-plugin-electron-builder/lib';
import installExtension, { VUEJS_DEVTOOLS } from 'electron-devtools-installer';
const isProduction = process.env.NODE_ENV === 'production';
const isDevelopment = !isProduction;

// Handle all unhandled exceptions for diagnostics (and reporting):
const unhandled = require('electron-unhandled');
unhandled();

// Keep a global reference of the window object, if you don't, the window will
// be closed automatically when the JavaScript object is garbage collected.
let win; // main window
let splash; // splash screen window

const ICON =
  process.platform == 'darwin' ? path.join(__dirname, 'assets', 'icons', 'mac', 'logo.png.hqx')
      : process.platform == 'win32' ? path.join(__dirname, 'assets', 'icons', 'win', 'logo.png.ico')
          : path.join(__dirname, 'assets', 'icons', 'logo.png_64x64.png');

let URL_PREFIX;

// Standard scheme must be registered before the app is ready
protocol.registerSchemesAsPrivileged([{
    scheme: 'app',
    privileges: {
        standard: true,
        secure: true,
        supportFetchAPI: true
    }
}]);

async function createSplashScreen() {
    let width = 750;
    let height = 750;
    splash = new BrowserWindow({
        width: width,
        height: height,
        minWidth: width,
        minHeight: height,
        maxWidth: width,
        maxHeight: height,
        resizable: false,
        minimizable: false,
        maximizable: false,
        closable: true,
        fullscreenable: false,
        frame: false,
        titleBarStyle: 'hidden',
        hasShadow: false,
        backgroundColor: 'white',
        transparent: true,
        opacity: 0.8,
        skipTaskbar: true,
        alwaysOnTop: false,
        icon: ICON,
        title: 'Loading Iris Terminal . . .',
        webPreferences: {
            preload: path.join(__static, 'splash', 'splash_preload.js'),
            contextIsolation: true
        }
    });
    
    await splash.loadURL(`${URL_PREFIX}splash/splash.html`);
    splash.center();
}

async function initProgram(){
    // Determine if a splash-screen should be skipped:
    let no_splash = false; // default
    // Process command-line arguments:
    if(process && process.env && process.env.npm_config_argv){
        let argv = JSON.parse(process.env.npm_config_argv).remain; // grab unused arguments
        let keys = argv.filter((v,i) => !(i % 2)); // grab keys
        let vals = argv.filter((v,i) => i % 2); // grab values associated with keys
        let splashlessIdx = keys.indexOf('splashless'); // Index of db-mission value
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
        show: no_splash, // only hide if a splash-screen is shown
        webPreferences: {
            contextIsolation: false, // don't use separate JS context
            nodeIntegration: true, // allow use of node in renderer
            nodeIntegrationInWorker: true,
            preload: path.join(__static, 'preload.js')
        }
    };
    if(isProduction){
        windowOptions = Object.assign(windowOptions, {
            frame: false,
            show: false // don't show until content rendered (handled by Login.vue)
        });
    }

    // Create the browser window.
    win = new BrowserWindow(windowOptions);

    // Enable remote access to the window's webContents:
    require('@electron/remote/main').enable(win.webContents);

    if (process.env.WEBPACK_DEV_SERVER_URL) {
        // Load urls from the dev server if in development mode
        URL_PREFIX = `${process.env.WEBPACK_DEV_SERVER_URL}/`;
    } else {
        // Load urls from production server via app protocol:
        createProtocol('app');
        URL_PREFIX = 'app://./';
    }

    if(!no_splash){
        createSplashScreen();
    } else {
        splash = null; // no slash screen
    }
}

async function renderWindow() {
    // Load window:
    await win.loadURL(`${URL_PREFIX}index.html`);
    // If in production, check for Updates to the Github Releases Page:
    if (!process.env.WEBPACK_DEV_SERVER_URL) {
        autoUpdater.checkForUpdatesAndNotify();
    }

    win.on('closed', () => {
        win = null;
    });
}

// Spawns the application:
async function spawn(){
    initProgram();
    if (isDevelopment && !process.env.IS_TEST) {
        // Install Vue Devtools
        installExtension(VUEJS_DEVTOOLS)
            .then( (name) => {
                console.log(`Added extension ${name}.`);
                // Open dev tools:
                win.webContents.openDevTools();
            } )
            .catch( (err) => console.log('Error occurred while attempting to add an extension: ', err) );
    }
    renderWindow();
}

// Quit when all windows are closed.
app.on('window-all-closed', () => {
    // On macOS it is common for applications and their menu bar
    // to stay active until the user quits explicitly with Cmd + Q
    if (process.platform !== 'darwin') {
        app.quit();
    }
});
 
app.on('activate', () => {
    // On macOS it's common to re-create a window in the app when the
    // dock icon is clicked and there are no other windows open.
    if (win === null || BrowserWindow.getAllWindows().length === 0) {
        spawn();
    }
});

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
                app.quit();
            }
        });
    } else {
        process.on('SIGTERM', () => {
            app.quit();
        });
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
    sendUpdateMessage('Update download complete. Installing now. App will close in 10s.');
    setTimeout( () => {autoUpdater.quitAndInstall();}, 10000 );
  
});

autoUpdater.on('error', (err) => {
    sendUpdateMessage(`Error during auto-update. Info: ${err.toString()}`);
});

// Electron IPC Handlers:
ipcMain.on('app-loaded-and-ready', () => {
    // Transition from slash-screen to main application window.
    // Called when the application is loaded and ready to go.
    if(splash != null){
        splash.close();
    }
    if(win != null){
        win.show();
    }
});

ipcMain.handle('get-version', () => app.getVersion());
