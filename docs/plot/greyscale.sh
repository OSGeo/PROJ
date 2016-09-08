

for f in $(ls *.png)
do

    convert $f -fx '(r+g+b)/3' -colorspace Gray $f
done;
