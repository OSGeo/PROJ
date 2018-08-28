# Use Imagemagick to convert all images to grayscale,
# printing progress information
mogrify -monitor -colorspace Gray *.png
