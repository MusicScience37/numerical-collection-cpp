#!/bin/bash -e

cd $(dirname $0)/../doc/icon_logo

convert icon192.png icon64.png icon48.png icon32.png icon16.png icon.ico
identify icon.ico
