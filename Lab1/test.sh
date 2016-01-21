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

./simpsh \
    --rdonly fileA.txt \
    --rdonly fileB.txt \
    --wronly fileC.txt \
    --command 1 0 2 sort \
    --command 1 0 2 tr a-z A-Z

dog=`cat fileC.txt`
result=`grep bad fileC.txt`
if [ "$dog" != "$result" ];
then
    echo "Test E passed"
else
    echo "Test E failed"
fi

