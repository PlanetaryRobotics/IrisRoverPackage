// vue.config.js

module.exports = {
    configureWebpack: {
        devtool: 'source-map',
        externals: {
            'saslprep': 'require(\'saslprep\')'
        }
    },
    pluginOptions: {
        electronBuilder: {
            contextIsolation: false,
            nodeIntegration: true,
            nodeIntegrationInWorker: true,
            builderOptions: {
                // options placed here will be merged with default configuration and passed to electron-builder
                'appId': 'com.iris.terminal',
                'copyright': 'Copyright © 2023 ${author}',
                'publish': ['github'],
                'mac': {
                    'target': 'dmg',
                    'icon': 'build/icon.png'
                },
                'dmg': {
                    'title': '${productName}',
                    'background': 'build/background.png',
                    'icon': 'build/icon.png',
                    'contents': [
                        {
                            'x': 320,
                            'y': 320
                        },
                        {
                            'x': 240,
                            'y': 150,
                            'type': 'link',
                            'path': '/Applications'
                        }
                    ]
                },
                'win': {
                    'target': 'nsis',
                    'icon': 'build/icon.png',
                    'legalTrademarks': 'Copyright © 2023 ${author}'
                },
                'linux': {
                    'target': [
                        'AppImage',
                        'deb'
                    ],
                    category: 'Science'
                }
            }
        }
    }
};
