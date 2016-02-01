printf "Setup may take up to 20 seconds"

for i in {1..1000}
do
    base64 /dev/urandom | head -c 10 >> thousandLines.txt
    echo \n >> thousandLines.txt
done

base64 /dev/urandom | head -c 10000000 > tenMB.txt
echo "output" > output.txt
echo "error" > error.txt
