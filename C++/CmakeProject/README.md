# Cmake

Either install OpenCV locally or use docker.

- Docker instuctions:

    ```bash
    docker pull frameau/opencv-ceres
    xhost +si:localuser:root
    docker run -ti --rm \
           --network host \
           --env="DISPLAY" \
           --env="QT_X11_NO_MITSHM=1" \
           --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" \
           --volume="$HOME/.Xauthority:/home/$USER/.Xauthority:rw" \
           --volume="${PWD}:/home/$USER/ANMS-Codes" \
           frameau/opencv-ceres
    ```

Regarless of the environment and dependency installation method, compiling procedure:

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release .. #for Release mode
cmake -DCMAKE_BUILD_TYPE=Debug .. #for Debug mode
make
```
