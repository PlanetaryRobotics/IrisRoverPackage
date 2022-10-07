// Preloader for the splash screen that grabs app information from Electron:
const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('versions', {
    node: () => process.versions.node,
    chrome: () => process.versions.chrome,
    electron: () => process.versions.electron,
    irisTerminal: async () => ipcRenderer.invoke('get-version')
});