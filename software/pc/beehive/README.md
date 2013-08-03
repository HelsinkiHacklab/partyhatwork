# Beehive

Simple API to manage multi-node xbee networks

## Requirements

### python-serial

    pip install pyserial

### python-xbee

But not from pypi, those packages are buggy...

    wget https://github.com/tomstrummer/python-xbee/archive/master.zip
    unzip master
    cd python-xbee-master/
    python setup.py install
