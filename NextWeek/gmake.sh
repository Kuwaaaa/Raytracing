echo '--------------------------- GMAKE.sh start ---------------------------'

outfile=${1%%.*}'.out'
outname="image.ppm"

if [ "$2" ]
then
    outName=$2
fi
g++ $1 -o $outfile
rm $outname
./$outfile > "image.ppm"
echo "Render to $outfile"

echo '--------------------------- GMAKE.sh end   ---------------------------'
