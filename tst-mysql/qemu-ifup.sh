#!/bin/sh

brctl stp virbr0 off
brctl addif virbr0 tap1
ifconfig tap1 up
