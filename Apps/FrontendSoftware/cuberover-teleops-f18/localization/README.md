# CubeRover Localization Software

## Requirements

### Required packages
Install these dependencies before proceeding.
```
sudo apt-get install build-essential
sudo apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
sudo apt-get install libcanberra-gtk-module
```

### OpenCV 4.1.0

The steps outlined herein must be followed very closely to ensure that all necessary OpenCV dependencies are installed.

#### 1. Clone OpenCV source
The following commands can be placed in any working directory. I happened to run these in the `Downloads` folder, in which case you would prepend the following commands with `cd ~/Downloads`.
```
git clone https://github.com/opencv/opencv.git
cd opencv
git checkout tags/4.1.0
cd ../
```

#### 2. Clone OpenCV contrib repository for our additional feature recognition modules (SIFT, SURF)
The command `git checkout` specifies the repository version. This prevents future updates from breaking our software.
```
git clone https://github.com/opencv/opencv_contrib.git
cd opencv_contrib
git checkout tags/4.1.0
cd ../
```

#### 3. Create build directory in the OpenCV repository
```
cd opencv
mkdir build
cd build
```

#### 4. Generate the makefiles with CMake
```
cmake -DOPENCV_ENABLE_NONFREE:BOOL=ON -DOPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules ../
```

#### 5. Build (this step takes a while ~20mins).
The -j4 denotes how many threads can run simultaniously on your machine. This parameter only affects speed. I keep the number low to be safe and avoid crashing the build step.
```
make -j4
```

#### 6. Install the library.
This command moves the binaries generated in the previous step to a system folder for easy access.
```
sudo make install
```

#### 7. (optional) Remove extra files.
Delete the `opencv` and `opencv_contrib` folders in the working directory that you downloaded them to. The compiled binaries are already installed in a system folder so these repositories are no longer necessary unless you would like to reference examples.
```
cd ../../
rm -r opencv
rm -r opencv_contrib
```

## Building
We must build the localization software in order to run it. It is a simple process.

#### 1. Change directory
Move into the `/build` directory
```
cd build
```

#### 2. Run CMake
CMake creates a `Makefile` that will eventually be used to compile our project. It tells the compiler where to find the OpenCV library files.
```
cmake .
```

#### 3. Compile
This command finds the file named `Makefile` and runs the commands inside of it to build our program.
```
make
```

## Usage

### Object Registration
Use the `pnp_registration` binary.

```
./pnp_registration --image=../data/deck.jpg --model=../data/deck_ORB.yml --mesh=../data/deck.ply

```

The options are:

|**Option**|**Default**|**Explanation**|
|---|---|---|
|help h | | Display common parameters and usage|
|image i       |      | path to input image|
|model         |      | path to output yml model|
|mesh              |      | path to ply mesh|
|keypoints k   |2000  | number of keypoints to detect (only for ORB)|
|feature       |ORB   | feature name (ORB, KAZE, AKAZE, BRISK, SIFT, SURF, BINBOOST, VGG)|


### Object Detection
Use the `pnp_detection` binary.

```
./pnp_detection --video=../data/deck.mov --model=../data/deck_ORB.yml --mesh=../data/deck.ply
```

The options are:

|**Option**|**Default**|**Explanation**|
|---|---|---|
|help h | | Display common parameters and usage|
|video v           |      | path to recorded video|
|model             |      | path to yml model|
|mesh              |      | path to ply mesh|
|keypoints k       |2000  | number of keypoints to detect|
|ratio r           |0.7   | threshold for ratio test|
|iterations it     |500   | RANSAC maximum iterations count|
|error e           |6.0   | RANSAC reprojection error|
|confidence c      |0.99  | RANSAC confidence|
|inliers in        |30    | minimum inliers for Kalman update|
|method  pnp       |0     | PnP method: (0) ITERATIVE - (1) EPNP - (2) P3P - (3) DLS - (5) AP3P|
|fast f            |true  | use of robust fast match|
|feature           |ORB   | feature name (ORB, KAZE, AKAZE, BRISK, SIFT, SURF, BINBOOST, VGG)|
|FLANN             |false | use FLANN library for descriptors matching|
|save              |      | path to the directory where to save the image results|
|displayFiltered   |false | display filtered pose (from Kalman filter)|

## Troubleshooting

#### Issue: CMake error after moving repository
```
CMake Error: The current CMakeCache.txt directory /home/yourusername/Documents/cuberov
er-teleops/localization/build/CMakeCache.txt is different than the directory /home/you
rusername/git/cuberover-teleops/localization/build where CMakeCache.txt was created.
This may result in binaries being created in the wrong place. If you are not sure,
reedit the CMakeCache.txt
```
This error is a result of moving your project directory after building it. It occure after running the commad `cmake .` in the build process. Simply delete `CMakeCache.txt` and run the command again.

#### Issue: Running the main program
```
libc++abi.dylib: terminating with uncaught exception of type cv::Exception: OpenCV(4.1
.0-dev) /Users/dylanhawley/Downloads/opencv/modules/core/src/copy.cpp:254: error: (-21
5:Assertion failed) channels() == CV_MAT_CN(dtype) in function 'copyTo'

Abort trap: 6
```
Occurs during `./pnp_detection`. This means that the program cannot find your `.yml` object registration file. This can happen under two conditions:
  - The specified filename/filepath is incorrect. Remember, it is best practice to store the `.yml` file in the `/data` directory, in which case you would have to go up a directory with `../data/filename.yml`.
  - The object registration file was never created. See the [Usage](#usage) section of this README to see how this is done.