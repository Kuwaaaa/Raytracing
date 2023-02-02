outfile=${1%.*}'.out'
px=${1#*.}
cpl="gcc"

if px=="cc"
    then
        cpl="g++"
    fi

${cpl} $1 -o ${outfile}
./${outfile}
    