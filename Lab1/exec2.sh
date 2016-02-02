#!/usr/local/cs/execline-2.1.4.5/bin/execlineb

redirfd -w 1 output.txt
redirfd -r 0 tenMB.txt

pipeline {
   sed "s/[aA]/a/g;s/[bB]/b/g;s/[cC]/c/g;s/[dD]/d/g;s/[eE]/e/g;s/[fF]/f/g;s/[gG]/g/g;s/[hH]/h/g;s/[iI]/i/g;s/[jJ]/j/g;s/[kK]/k/g;s/[lL]/l/g;s/[mM]/m/g;s/[nN]/n/g;s/[oO]/o/g;s/[pP]/p/g;s/[qQ]/q/g;s/[rR]/r/g;s/[sS]/s/g;s/[tT]/t/g;s/[uU]/u/g;s/[vV]/v/g;s/[wW]/w/g;s/[xX]/x/g;s/[yY]/y/g;s/[zZ]/z/g"
}
pipeline {
    cat output.txt -
}
uniq -u
