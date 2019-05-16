set -e

[ -z ${STUID} ]   && echo "STUID must be set (RTFM)"   && exit
[ -z ${STUNAME} ] && echo "STUNAME must be set (RTFM)" && exit

cwd=${PWD##*/}
echo "${STUID} (${STUNAME}) submitting ${TASK}..."

wiki=`curl -m 1 -Ls -w '%{url_effective}' -o /dev/null 'http://moon.nju.edu.cn/~jyywiki' | grep -o 'http://[^/]*/'`
# wiki='http://114.212.87.156:5000/' # hot fix

tarball=$(mktemp -q).tar.bz2
bash -c "cd .. && tar cj ${cwd} > ${tarball}"
curl -F "task=${TASK}" -F "id=${STUID}" -F "name=${STUNAME}" -F "submission=@${tarball}" ${wiki}upload
