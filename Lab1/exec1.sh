#!/usr/local/cs/execline-2.1.4.5/bin/execlineb

redirfd -w 1 output.txt
pipeline {
    sort tenMB.txt
}
pipeline {
    cat output.txt -
}
tr A-Z a-z

