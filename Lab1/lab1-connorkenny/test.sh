test=$'File A\nabc\nbcd\ncde'
echo "$test" > fileA.txt
echo "File B" > fileB.txt
echo "File C" > fileC.txt

./simpsh \
    --rdonly fileA.txt \
    --wronly fileB.txt \
    --wronly fileC.txt \
    --command 0 1 2 sort

dog=`cat fileB.txt`
result=$'abc\nbcd\ncde\nFile A'

if [ "$dog" = "$result" ];
then
    echo "Test A passed"
else
    echo "Test A failed"
fi

./simpsh \
    --rdonly fileA.txt \
    --wronly fileB.txt \
    --wronly fileC.txt \
    --command 0 1 2 tr a-z A-Z

dog=`cat fileB.txt`
result=$'FILE A\nABC\nBCD\nCDE'
if [ "$dog" = "$result" ];
then
    echo "Test B passed"
else
    echo "Test B failed"
fi

./simpsh \
    --rdonly fileA.txt \
    --wronly fileB.txt \
    --wronly fileC.txt \
    --command 0 1 2 sort \
    --command 0 1 2 tr a-z A-Z

dog=`cat fileB.txt`
result=$'FILE A\nABC\nBCD\nCDE'
if [ "$dog" = "$result" ];
then
    echo "Test C passed"
else
    echo "Test C failed"
fi

./simpsh \
    --wronly fileA.txt \
    --rdonly fileB.txt \
    --wronly fileC.txt \
    --command 1 0 2 sort \
    --command 1 0 2 tr a-z A-Z

dog=`cat fileB.txt`
result=$'FILE A\nABC\nBCD\nCDE'
if [ "$dog" = "$result" ];
then
    echo "Test D passed"
else
    echo "Test D failed"
fi

#tests --creat on both existing and pre-existing files
echo "$test" > fileA.txt
./simpsh \
    --rdonly fileA.txt \
    --creat --wronly fileD.txt \
    --creat --wronly fileC.txt \
    --command 0 1 2 sort
dog=`cat fileD.txt`
result=$'abc\nbcd\ncde\nFile A'
if [ "$dog" = "$result" ];
then
    echo "--creat successful"
else
    echo "--creat failed, expected:"
    echo "$result"
    echo "Received:"
    echo "$dog"
fi

#tests --append --rdwr
echo "File B" > fileB.txt
./simpsh \
    --rdwr fileA.txt \
    --append --rdwr fileB.txt \
    --wronly fileC.txt \
    --command 0 1 2 sort
dog=`cat fileB.txt`
result=$'File B\nabc\nbcd\ncde\nFile A'
if [ "$dog" = "$result" ];
then
    echo "--append successful"
    echo "--rdwr successful"
else
    echo "--append or --rdwr failed, expected"
    echo "$result"
    echo "Received:"
    echo "$dog"
fi

#tests --abort --ignore --catch
./simpsh \
    --ignore 11 \
    --abort
echo "--ignore successful"

./simpsh \
    --ignore 11 \
    --abort \
    --catch 11 \
    --abort \

if [ "$?" == "11" ];
then
    echo "--catch successful"
else
    echo "--catch failed"
fi

rm fileA.txt
rm fileB.txt
rm fileC.txt
rm fileD.txt
