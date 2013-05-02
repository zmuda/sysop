rm -rf ZmudaMichal
mkdir ZmudaMichal
cp -r $1 ZmudaMichal
tar cvzf ZmudaMichal-$1.tar.gz ZmudaMichal
rm -rf ZmudaMichal
