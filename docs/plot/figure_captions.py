import sys
import os
import json
import pathlib
import re


def update_caption(image_filename, proj_string, doc_directory,
                   doc_extension='rst'):
    """
    Update the figure caption with a proj-string

    This function assumes that within the "doc_directory" there is a file with
    the same basename as the "image_filename" (e.g. for the image "aea.png",
    the function will assume that there is a documentation file "aea.rst").
    Within that documentation file, the function will try to find a section
    starting with "..figure:: ./images/" and ending with a line like
    "   proj-string:...".
    This line will be replaced with something like
    "   proj-string: ``+proj=aea``"

    :param filename: The name of the image that's used in the documentation
    :param proj_string: The proj-string that was used to generate the image
    :param doc_directory: The directory containing the projection documentation .rst files
    :return:
    """

    basename, image_extension = os.path.splitext(image_filename)
    doc_path = os.path.join(doc_directory, basename + os.extsep + doc_extension)
    p = pathlib.Path(doc_path)

    if p.is_file():
        replacement_string = r'\1   proj-string: ``{}``\2'.format(proj_string)
        new_doc_content = re.sub(
            r'(\.\. figure\:\: \.\/images\/.*?^)[ \t]*proj-string.*?($)',
            replacement_string,
            p.read_text(),
            flags=re.MULTILINE + re.DOTALL)
        p.write_text(new_doc_content)

    else:
        print("File {} not found.".format(doc_path))


def main():
    """
    Main function of caption replacement script.

    Parses json-file with plot setups and runs the caption replacement
    for each plot setup.
    """

    if os.path.exists(sys.argv[1]):
        # first argument is the JSON plot definition setup file
        with open(sys.argv[1]) as plotsetup:
            plotdefs = json.load(plotsetup)
    else:
        raise ValueError('No plot definition file entered')

    plots = []
    # second argument is the projections documentation directory
    outdir = sys.argv[2]

    for i, plotdef in enumerate(plotdefs):
        if plots != [] and plotdef['name'] not in plots:
            continue

        print(i, plotdef['projstring'])
        if 'skip' in plotdef.keys():
            print('skipping')
            continue

        update_caption(plotdef['name'],
                       plotdef['projstring'],
                       outdir)


if __name__ == "__main__":
    sys.exit(main())
