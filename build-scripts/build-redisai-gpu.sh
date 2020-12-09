#!/bin/sh

#Install RedisAI
if [[ -f ./RedisAI/install-gpu/redisai.so ]]; then
    echo "RedisAI GPU has already been downloaded and installed"
else

    # check for cudnn includes
    if [ -z "$CUDA_HOME" ]; then
        echo "ERROR: CUDA_HOME is not set"
        exit 1
    else
        echo "Found CUDA_HOME: $CUDA_HOME"
    fi

    # check for cudnn includes
    if [ -z "$CUDNN_INCLUDE_DIR" ]; then
        echo "ERROR: CUDNN_INCLUDE_DIR is not set"
        exit 1
    else
        echo "Found CUDNN_INCLUDE_DIR: $CUDNN_INCLUDE_DIR "
        if [ -f "$CUDNN_INCLUDE_DIR/cudnn.h" ]; then
            echo "Found cudnn.h at $CUDNN_INCLUDE_DIR"
        else
            echo "ERROR: could not find cudnn.h at $CUDNN_INCLUDE_DIR"
            exit 1
        fi
    fi

    # check for cudnn library
    if [ -z "$CUDNN_LIBRARY" ]; then
        echo "ERROR: CUDNN_LIBRARY is not set"
        exit 1
    else
        echo "Found CUDNN_LIBRARY: $CUDNN_LIBRARY"
        if [ -f "$CUDNN_LIBRARY/libcudnn.so" ]; then
            echo "Found libcudnn.so at $CUDNN_LIBRARY"
        else
            echo "ERROR: could not find libcudnn.so at $CUDNN_LIBRARY"
            exit 1
        fi
    fi


    if [[ ! -d "./RedisAI" ]]; then
        git clone --recursive https://github.com/RedisAI/RedisAI.git RedisAI
        cd RedisAI
        git checkout tags/v1.0.2
        cd ..
    else
        echo "RedisAI downloaded"
    fi
    cd RedisAI
    echo "Downloading RedisAI GPU dependencies"
    CC=gcc CXX=g++ bash get_deps.sh gpu
    echo "Building RedisAI GPU v1.0.2"
    CC=gcc CXX=g++ ALL=1 make -C opt clean build GPU=1 WITH_PT=$1 WITH_TF=$2 WITH_TFLITE=$3 WITH_ORT=$4

    if [ -f "./install-gpu/redisai.so" ]; then
        echo "Finished installing RedisAI"
        cd ../
    else
        echo "ERROR: RedisAI failed to build"
        exit 1
    fi
fi