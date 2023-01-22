echo '--------------------------- Render start ---------------------------'

outfile=${1%%.*}'.out'

g++ $1 -o $outfile
rm image.ppm
./$outfile > 'image.ppm'

echo '--------------------------- Render end   ---------------------------'
