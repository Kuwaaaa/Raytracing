res=`find . -name image.ppm`
res=${res#*/}

if [ "$2" ]
then 
    echo "aa"
fi


if [ $res == $1 ] 
then 
    echo "found"
fi
