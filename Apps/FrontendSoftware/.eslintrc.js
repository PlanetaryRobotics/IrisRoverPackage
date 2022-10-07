module.exports = {
    'env': {
        'browser': true,
        'es2020': true,
        'node': true
    },
    'extends': [
        'eslint:recommended',
        'plugin:vue/recommended'
    ],
    'parserOptions': {
        'parser': 'babel-eslint',
        'ecmaVersion': 11,
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

        // GENERAL EXCEPTIONS:
        'no-console': process.env.NODE_ENV === 'production' ? 'off' : 'off',
        'no-debugger': process.env.NODE_ENV === 'production' ? 'error' : 'off',

        // VUE EXCEPTIONS:
        'vue/no-v-html': 'off',
        'vue/require-default-prop': 'off',
        'vue/attribute-hyphenation': 'off', // this breaks a lot since we're adding lint late into dev
        'vue/no-mutating-props': process.env.NODE_ENV === 'production' ? 'warn' : 'warn'
    }
};
