#!/bin/bash

CONTAINER=event_queue_devel_image
IMAGE_NAME=event_queue_devel_image

THIS_SCRIPT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
PROJECT_DIR=`readlink -f ${THIS_SCRIPT_DIR}/../`

DOCKER_MOUNT_ARGS="-v ${PROJECT_DIR}:/workspace/"

docker run --name ${IMAGE_NAME} --privileged --rm \
    ${DOCKER_MOUNT_ARGS} \
    -e USER=$USER -e USERID=$UID \
    --net=host \
    -it ${CONTAINER}
