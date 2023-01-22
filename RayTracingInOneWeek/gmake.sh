echo '--------------------------- GMAKE.sh start ---------------------------'

outfile=${1%%.*}'.out'


g++ $1 -o $outfile
rm image.ppm
./$outfile > 'image.ppm'

echo '--------------------------- GMAKE.sh end   ---------------------------'
