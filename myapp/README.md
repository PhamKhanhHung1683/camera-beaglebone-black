# Build static OpenCV (cross-compile)

1. Install Dependencies

```bash 
# Required packages
sudo apt-get install build-essential cmake pkg-config ninja-build \
  gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf

# Optional (for faster builds)
sudo apt-get install ccache
```

2. Clone OpenCV source code

```bash
git clone -b 4.8.0 https://github.com/opencv/opencv.git opencv-4.8.0
```

3. Configure and build OpenCV

```bash
cd opencv-4.8.0
mkdir build
cd build
```

```bash
cmake -GNinja \
  -DCMAKE_TOOLCHAIN_FILE=../toolchain/bbb.toolchain.cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF \
  -DOPENCV_GENERATE_PKGCONFIG=ON \
  -DBUILD_TESTS=OFF \
  -DBUILD_PERF_TESTS=OFF \
  -DBUILD_EXAMPLES=OFF \
  -DBUILD_opencv_apps=OFF \
  -DBUILD_LIST=core,imgproc,videoio,objdetect,gapi \
  -DWITH_CUDA=OFF -DWITH_OPENCL=OFF \
  -DWITH_PNG=OFF -DWITH_JPEG=OFF -DWITH_TIFF=OFF -DWITH_WEBP=OFF -DWITH_OPENJPEG=OFF -DWITH_JASPER=OFF -DWITH_OPENEXR=OFF \
  -DWITH_IMGCODEC_HDR=OFF -DWITH_IMGCODEC_SUNRASTER=OFF -DWITH_IMGCODEC_PXM=OFF -DWITH_IMGCODEC_PFM=OFF \
  -DWITH_V4L=ON -DWITH_FFMPEG=OFF -DWITH_GSTREAMER=OFF -DWITH_GTK=OFF -DWITH_QT=OFF \
  -DWITH_PROTOBUF=OFF \
  -DCMAKE_INSTALL_PREFIX=install \
  -DBUILD_JAVA=OFF -DBUILD_opencv_python2=OFF -DBUILD_opencv_python3=OFF \
  -DBUILD_ZLIB=OFF -DBUILD_ITT=OFF -DWITH_IPP=OFF \
  ..
```

```bash
ninja
ninja install
```

# Build app (cross-compile)
```bash
mkdir build
cd build
```

```bash
cmake -GNinja \
  -DCMAKE_TOOLCHAIN_FILE=../toolchain/bbb.toolchain.cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH=$PWD/../opencv-4.8.0/build/install \
  ..
ninja
```
