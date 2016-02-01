./simpsh \
    --verbose \
    --profile \
    --rdonly tenMB.txt \
    --pipe \
    --pipe \
    --creat --trunc --wronly output.txt \
    --creat --append --wronly error.txt \
    --command 3 5 6 tr A-Z a-z \
    --command 0 2 6 sort \
    --command 1 4 6 cat output.txt - \
    --wait

printf "\n"

./simpsh \
    --verbose \
    --profile \
    --rdonly tenMB.txt \
    --pipe \
    --pipe \
    --creat --trunc --wronly output.txt \
    --creat --append --wronly error.txt \
    --command 3 5 6 uniq -u \
    --command 0 2 6 sed -e 's/\(.*\)/\L\1/' \
    --command 1 4 6 cat output.txt - \
    --wait

printf "\n"

./simpsh \
    --verbose \
    --profile \
    --rdonly thousandLines.txt \
    --pipe \
    --pipe \
    --creat --trunc --wronly output.txt \
    --creat --append --wronly error.txt \
    --append --rdwr tenMB.txt \
    --command 1 4 6 sort \
    --command 0 2 6 cat tenMB.txt \
    --command 3 5 6 sed ':a;N;$!ba;s/\n/ /g'  \
    --wait
