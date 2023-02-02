echo '--------------------------- Render start ---------------------------'

outfile=${1%%.*}'.out'
outName="image.ppm"

if [ "$2" ]
then
    outName=$2
fi
g++ $1 -o $outfile
rm $outName
./$outfile > $outName
echo "render to $outName."

echo '--------------------------- Render end   ---------------------------'