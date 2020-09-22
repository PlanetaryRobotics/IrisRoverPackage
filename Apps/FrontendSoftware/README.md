# IrisRover Mission Control Interface 2020 v.1

## **Project setup**
Install `node@12.5.0`, `npm`, `yarn`, and a c-compiler (`xcode` if on a mac).
```
npm install
npm audit fix
yarn install
npm run postinstall
```

----
----


## **Development**

### **Compile with hot-reloading for development**
```
npm run electron:serve
```

----

### **Adding Packages**
If you're adding a package, use `npm` and be sure to save it. If it's going to be used for functional code, save it normally with `--save`:
```
npm install --save package-name
```
However, if it's a development tool, is **only** going to be used for development, and doesn't need to be included in the final package, then use `--save-dev`:
```
npm install --save-dev package-name
```

----

### **Special command-line arguments for use in development**
The following arguments can be chained together indefinitely. Presently, all arguments must come in name-value pairs (You can't just add an argument name without also giving it a value; if you do, all subsequent arguments will fail to load.)

`splashless`: Automatically closes the splash-screen on startup for dev purposes. NOTE: Must be given the arg `1` to actually skip. Example:
```
npm run electron:serve splashless 1
```

`route-to`: Bypasses the login and starts the program at the given page registered in `router.js`.
Note: The given page must be listed in the router and must be at root level (eg. `path :'/sandbox_mapTest'` not `path :'/sandbox/mapTest'`).
Example:
Go to the `sandbox_mapTest` page in the router:
```
npm run electron:serve route-to sandbox_mapTest
```

`db-mission` and `db-pass`: Used when bypassing the login. `db-mission` specifies the mission (partition and database address) to be accessed, `db-pass` specifies the passcode for that partition.
Example:
```
npm run electron:serve db-mission Dev db-pass Passcode
```

----
----

## **Testing**

### **Lint and fix files**
```
npm run lint
```

### **Run tests**
```
npm run test
```

----
----

## **Deployment**

### 1. **Update `email`.**
Update the `author.email` field in `package.json` to a reliable contact for the individual responsible for this build.

### 2. **Make sure all icons are built for production**
```
npm run electron:generate-icons
```

### 3. **Compile and minify for production**
#### 3a. For your current platform:
```
npm run electron:build
```
#### 3b. For all distribution platforms:
To run the following, you must be on a mac with a Parallels Windows 10 VM installed, Parallels Toolbox installed, Shared Folders set to All Disks in Parallels for the VM, and the Parallels VM and Parallels Toolbox open.
```
npm run electron:build -- --linux AppImage --win nsis --mac dmg
```

### **Looking change build settings?**
Package metadata is handled by `package.json` and platform specific build settings are managed in `pluginOptions.electronBuilder.builderOptions` of `vue.config.js`.

### **Looking to add extra build functionality?**
Recipes are available at the Vue CLI Plugin Electron Builder [Recipe Page](https://nklayman.github.io/vue-cli-plugin-electron-builder/guide/recipes.html).


----
----


## **Repository Structure**
### Naming
The `master` branch of the `CubeRoverPackage` repository is reserved for the latest fully-integrated, tested, and stable package, all of the code for which has undergone code-review.

So, the effective `master` branch for the Frontend is the `frontend-stable`, which contains the latest tested and stable code but still requires E2E testing and documentation.

For each feature or bug fix being developed, each developer will branch off the `-stable` branch for their respective process with a branch named either
`frontend/<developer_name__or__unique_id>/<feature_name>`
or
`frontend/<developer_name__or__unique_id>/bugfix_<bugfix_name>`
depending on what is being developed. For example:
`frontend/cwcolomb/user_permissions`
or
`frontend/c2/bugfix_changestream_timeout`.

### Usage
Code from these developer branches will only be merged to the `frontend-stable` branch after a code-review for the developed feature and any associated unit tests has been initiated by submitting a PR on GitHub and passed.

If a developer's branch requires updated code from `frontend-stable` in order to get a new feature, a PR will be initiated from `frontend-stable` to the dev branch.

This structure helps ensure not only that our software will be as flight-ready as possible but also that, if anything does go wrong during mission, we'll be able to perform thorough root-cause analysis afterwards to determine why the failure occurred in terms of both code and engineering processes so that future planetary missions can avoid replicating the same failing.
