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
make
[[ -f trees ]] && ./trees -int -redblack 2>/dev/null
#[[ -f trees ]] && ./trees -int -bst 2>/dev/null
#[[ -f trees ]] && ./trees -double -redblack 2>/dev/null
#[[ -f trees ]] && ./trees -double -bst 2>/dev/null
#[[ -f trees ]] && ./trees 2>/dev/null
#[[ -f trees ]] && startx ./trees

