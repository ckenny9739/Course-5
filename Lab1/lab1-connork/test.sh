function should_fail() {
    result=$?;

    echo -n "==> $1 ";

    if [ $result -lt 1 ]; then
	echo "FAILURE";
	exit 1;
    else
	echo;
    fi
}

function should_succeed() {
    result=$?;

    echo -n "==> $1 ";

    if [ $result -gt 0 ]; then
	echo "FAILURE";
	exit 1;
    else
	echo;
    fi
}

tmp_file=/tmp/foo
tmp_file2=/tmp/foo2
> "$tmp_file"
> "$tmp_file2"


./simpsh --rdonly cantpossiblyexist 2>&1 | grep "No such file" > /dev/null
should_succeed "reports missing file";


./simpsh --rdonly Makefile | grep "No such file" > /dev/null;
should_fail "does not report file that exists"


./simpsh --verbose --command 1 2 3 echo foo 2>&1 | grep "Bad file descriptor" > /dev/null
should_succeed "using a non existent file descriptor should report the error"


> "$tmp_file"
(./simpsh \
     --verbose \
     --wronly "$tmp_file" \
     --command 1 2 3 echo foo 2>&1 \
				     --command 0 0 0 echo foo ) 2>/dev/null 1>/dev/null

grep "foo" "$tmp_file" > /dev/null
should_succeed "commands after failing commands should succeed"


./simpsh --verbose --command 2>&1 | grep "command requires an argument" > /dev/null
should_fail "empty command should have no options"


./simpsh --verbose --command 1 2 3 2>&1 | grep "command requires at least 4 options" > /dev/null
should_succeed "command requires at least 4 arguments"

./simpsh --verbose --command 1 2 3 2>&1 | grep "command requires at least 4 options" > /dev/null
should_succeed "command reports malformed command options to user on stderr"


./simpsh \
    --wronly /tmp/foo \
    --verbose \
    --command 0 0 0 echo foo \
    | grep "command 0 0 0 echo foo" > /dev/null
should_succeed "command tracks all command options"


./simpsh --wronly /tmp/foo --command 0 0 0 echo "foo";
grep foo /tmp/foo > /dev/null;
should_succeed "command can write to write only file"


> "$tmp_file"
echo_path=$(which echo);
./simpsh --wronly /tmp/foo --command 0 0 0 "$echo_path" "foo";
grep foo /tmp/foo > /dev/null;
should_succeed "path command can write to write only file"


> "$tmp_file"
big_string=$(cat misc/foo.txt);
./simpsh --wronly /tmp/foo --command 0 0 0 echo "$big_string";
grep oooooooooooooooooo /tmp/foo > /dev/null;
should_succeed "path command can write long strings to write only file"


> "$tmp_file"
./simpsh --rdonly /tmp/foo --command 0 0 0 echo "foo";
grep foo /tmp/foo > /dev/null;
# NOTE that failure of `echo "foo"` end up in stderr
should_fail "shouldn't be able to write to read only file"


echo "foo" > "$tmp_file"
cat "$tmp_file" | wc -l | grep 1 > /dev/null
should_succeed "the temporary file should have one line"
# the cat of $tmp_file should be empty and not add another line to tmp_file
./simpsh --wronly "$tmp_file" --command 0 0 0 cat "$tmp_file"
cat "$tmp_file" | wc -l | grep 1 > /dev/null
should_succeed "shouldn't be able to write to read only file"


echo "foo" > "$tmp_file"
echo "bar" > "$tmp_file2"

cat "$tmp_file" | grep "foo" > /dev/null
should_succeed "the temporary file should have 'foo'"

cat "$tmp_file2" | grep "bar" > /dev/null
should_succeed "the temporary file 2 should have 'bar'"

# cat of /tmp/foo should end up in the /tmp/file2
./simpsh --rdonly "$tmp_file" --wronly "$tmp_file2" --command 0 1 0 cat "$tmp_file"
cat "$tmp_file2" | grep "foo" > /dev/null && cat "$tmp_file2" | wc -l | grep 1 > /dev/null
should_succeed "should be able to cat from one file to the other (replace bar with foo)"

# TODO: test that verbose outputs each of the options in the right order
# TODO: test with larger number file descriptors
# TODO: exit status is the max exit status from all commands
# > When simpsh exits other than in response to a signal, it should exit with status equal to the maximum of all the exit statuses of all the subcommands that it ran and successfully waited for. However, if there are no such subcommands, or if the maximum is zero, simpsh should exit with status 0 if all options succeeded, and with status 1 one of them failed. For example, if a file could not be opened, simpsh must exit with nonzero status.

echo "Success"
