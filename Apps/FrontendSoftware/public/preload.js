/** Electron preloader used to limit exposure of the renderer process to the
 * Electron backend.
 * 
 * One instance of this preloader exists **per renderer window** (excluding the
 * splashscreen, which has its own preloader).
 * 
 * Author: Connor W. Colombo
 * Created: 10/07/2022
 * Last Updated: 10/08/2022
 */

const { contextBridge, ipcRenderer } = require('electron');

// Min time for the splash screen to show (so at least 1 full animation cycle can complete):
const MIN_SPLASH_TIME_MS = 8000;

const startTime = Date.now();

// Build the API:
const API = {
    send: {
        // Outbound messages (from renderer window to Electron main):
        signalAppLoadedAndReady: () => {
            // Signal sent when the window is loaded and ready to be shown.
            // Main responds with `receive.onWindowShown`,
            const signal = 'app-loaded-and-ready';
            if(Date.now() - startTime > MIN_SPLASH_TIME_MS) {
                // Tell the Electron backend that the app is loaded and ready to use:
                ipcRenderer.send(signal);
            } else {
                // Tell Electron once MIN_SPLASH_TIME_MS has elapsed since start:
                setTimeout(() => {
                    ipcRenderer.send(signal);
                }, (MIN_SPLASH_TIME_MS - (Date.now() - startTime) + 1));
            }
        }
    },
    receive: {
        // Inbound messages (from Electron main to renderer window):
        onWindowShown: (callback) => {
            // Event fired when the window is shown to the user.
            // Sent in response to `send.signalAppLoadedAndReady`
            ipcRenderer.on('window:shown', callback);
        }
    }
};

// Expose the API depending on whether contextIsolation is available:
if(process.contextIsolated){
    // Expose only the necessary bits of the Electron API:
    contextBridge.exposeInMainWorld('ShieldedElectronAPI', API);
} else {
    // We don't have contextIsolation, so just expose the API through the window:
    window.ShieldedElectronAPI = {};
    // Copy the API object so it remains alive after the preloader exits
    // (for when electron hot reloads):
    Object.assign(window.ShieldedElectronAPI, API);
}


