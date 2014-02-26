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
make tester
if [[ -f tester ]]; then 
	./tester 
	if [[ $? != 0 ]]; then
		echo "Test failed"
	else
		echo "Test passed"
	fi

	make 
	./trees -string -redblack 2>/dev/null
	#./trees -int -bst 2>/dev/null
	#./trees -int -redblack 2>/dev/null
	#./trees -int -bst 2>/dev/null
	#./trees -double -redblack 2>/dev/null
	#./trees -double -bst 2>/dev/null
	#./trees 2>/dev/null
	#startx ./trees
fi


