module.exports = {
    'env': {
        'browser': true,
        'es2021': true,
        'node': true
    },
    'extends': [
        'eslint:recommended',
        'plugin:vue/recommended'
    ],
    'parserOptions': {
        'parser': 'babel-eslint',
        'ecmaVersion': 12,
        'sourceType': 'module'
    },
    'plugins': [
        'vue'
    ],
    'rules': {
        // BASIC:
        'indent': [
            'error',
            4
        ],
        'linebreak-style': [
            'error',
            'unix'
        ],
        'quotes': [
            'error',
            'single'
        ],
        'semi': [
            'error',
            'always'
        ],

        // EXCEPTIONS:
        'vue/no-v-html': 'off',
        'vue/require-default-prop': 'off',
        'no-console': process.env.NODE_ENV === 'production' ? 'off' : 'off',
        'no-debugger': process.env.NODE_ENV === 'production' ? 'error' : 'off'
    }
};
