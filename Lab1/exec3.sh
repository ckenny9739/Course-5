#!/usr/local/cs/execline-2.1.4.5/bin/execlineb

redirfd -w 1 output.txt
redirfd -r 0 thousandLines.txt

pipeline {
    cat tenMB.txt -
}
pipeline {
    sort
}
sed -e "s/\'$'\
/ /g"
