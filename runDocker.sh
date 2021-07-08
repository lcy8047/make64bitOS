#!/bin/bash
PWNDIR=/Users/ichang-yul/Desktop/make64bitOS/os
docker run --rm -it -v $PWNDIR:/os ubuntu20:os /bin/bash
