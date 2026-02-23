if (typeof window === 'undefined') {
    global.window = {}
}
const jsonpickle = require('jsonpickle')
const fs = require('fs')

const jps = String(fs.readFileSync('scratchpad.json', {encoding:'utf8', flag: 'r'}))

console.log(jps)

let data = jsonpickle.decode(jps)

print(data)

let x = x + 1