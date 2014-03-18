pushd ../x11grid
update=0
update=`make -n | grep ^ar | wc -l | tr -d ' '`
[[ $update != 0 ]] && rm x11grid
make
#[[ $update != 0 ]] && ./x11grid
popd
[[ $update != 0 ]] && rm *.o
[[ $update != 0 ]] && rm trees
update=`make -n | grep ^g++ | wc -l | tr -d ' '`
[[ $update != 0 ]] && rm trees
[[ $update != 0 ]] && rm tester
#make tester
make 
echo "Starting trees at `date`" > trees.out
#./stress
./trees  -o journal.string -string -redblack 2>/dev/null
#./trees -int -bst 2>/dev/null
#./trees -int -redblack 
#./trees -int -bst 2>/dev/null
#./trees -double -redblack 2>/dev/null
#./trees -double -bst 2>/dev/null
#./trees 2>/dev/null
#startx ./trees

echo "Done with trees at `date`" >> trees.out


