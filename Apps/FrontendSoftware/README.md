# CubeRover Interface 2019 v.1

## Project setup
```
npm install
npm run postinstall
```

### Compiles and hot-reloads for development
```
npm run electron:serve
```

### Special command-line arguments for use in development
*route-to*: Bypasses the login and starts the program at the given page registered in `router.js`.
Note: The given page must be listed in the router and must be at root level (eg. `path :'/sandbox_mapTest'` not `path :'/sandbox/mapTest'`).
Example:
Go to the `sandbox_mapTest` page in the router:
```
npm run electron:serve route-to sandbox_mapTest
```

*db-mission* and *db-pass*: Used when bypassing the login. `db-mission` specifies the mission (partition and database address) to be accessed, `db-pass` specifies the passcode for that partition.
Example:
```
npm run electron:serve db-mission Dev db-pass Passcode
```

### Compiles and minifies for production
```
npm run electron:build
```

### Run your tests
```
npm run test
```

### Lints and fixes files
```
npm run lint
```
