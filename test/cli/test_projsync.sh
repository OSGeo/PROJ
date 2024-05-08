#!/bin/bash

# Test projsync

set -e

TEST_CLI_DIR=$(dirname $0)
EXE=$1
if test -z "${EXE}"; then
    echo "Usage: ${0} <path to 'projsync' program>"
    exit 1
fi
if test ! -x ${EXE}; then
    echo "*** ERROR: Can not find '${EXE}' program!"
    exit 1
fi

echo "============================================"
echo "Running ${0} using ${EXE}:"
echo "============================================"

if ! curl -s https://cdn.proj.org/files.geojson >/dev/null 2>/dev/null; then
    if ! wget https://cdn.proj.org/files.geojson -O /dev/null 2>/dev/null; then
        echo "Cannot download https://cdn.proj.org/files.geojson. Skipping test"
        exit 0
    fi
fi

TMP_OUT=test_projsync_out.txt

rm -rf tmp_user_writable_directory
mkdir -p tmp_user_writable_directory

echo "Testing $EXE --list-files --target-dir tmp_user_writable_directory"
if ! $EXE --list-files --target-dir tmp_user_writable_directory > ${TMP_OUT}; then
    echo "--list-files failed"
    cat ${TMP_OUT}
    exit 100
fi
cat ${TMP_OUT} | grep "at_bev_README.txt,,at_bev" >/dev/null || (cat ${TMP_OUT}; exit 100)

export PROJ_USER_WRITABLE_DIRECTORY=tmp_user_writable_directory

if test ! -f ${PROJ_USER_WRITABLE_DIRECTORY}/cache.db; then
    echo "*** ERROR: Can not find ${PROJ_USER_WRITABLE_DIRECTORY}/cache.db!"
    exit 100
fi
if test ! -f ${PROJ_USER_WRITABLE_DIRECTORY}/files.geojson; then
    echo "*** ERROR: Can not find ${PROJ_USER_WRITABLE_DIRECTORY}/files.geojson!"
    exit 100
fi

echo "Testing $EXE --all"
if ! $EXE --all --dry-run > ${TMP_OUT}; then
    echo "--all failed"
    cat ${TMP_OUT}
    exit 100
fi
cat ${TMP_OUT} | grep "Would download https://cdn.proj.org/at_bev_README.txt" >/dev/null || (cat ${TMP_OUT}; exit 100)


echo "Testing $EXE --source-id fr_ign"
if ! $EXE --source-id fr_ign --dry-run > ${TMP_OUT}; then
    echo "--source-id fr_ign failed"
    cat ${TMP_OUT}
    exit 100
fi
cat ${TMP_OUT} | grep "fr_ign_README.txt" >/dev/null || (cat ${TMP_OUT}; exit 100)


echo "Testing $EXE --source-id non_existing"
if $EXE --source-id non_existing >${TMP_OUT} 2>&1 ; then
    echo "--source-id non_existing failed"
    cat ${TMP_OUT}
    exit 100
fi
cat ${TMP_OUT} | grep "Warning: 'non_existing' is a unknown value for --source-id" >/dev/null || (cat ${TMP_OUT}; exit 100)
cat ${TMP_OUT} | grep "fr_ign" >/dev/null || (cat ${TMP_OUT}; exit 100)


echo "Testing $EXE --area-of-use France"
if ! $EXE --area-of-use France --dry-run > ${TMP_OUT}; then
    echo "--area-of-use France failed"
    cat ${TMP_OUT}
    exit 100
fi
cat ${TMP_OUT} | grep "fr_ign_ntf_r93.tif" >/dev/null || (cat ${TMP_OUT}; exit 100)


echo "Testing $EXE --area-of-use non_existing"
if $EXE --area-of-use non_existing >${TMP_OUT} 2>&1 ; then
    echo "--area-of-use non_existing failed"
    cat ${TMP_OUT}
    exit 100
fi
cat ${TMP_OUT} | grep "Warning: 'non_existing' is a unknown value for --area-of-use." >/dev/null || (cat ${TMP_OUT}; exit 100)
cat ${TMP_OUT} | grep "Australia" >/dev/null || (cat ${TMP_OUT}; exit 100)


echo "Testing $EXE --file ntf_r93"
if ! $EXE --file ntf_r93 > ${TMP_OUT}; then
    echo "--file ntf_r93 failed"
    cat ${TMP_OUT}
    exit 100
fi
cat ${TMP_OUT} | grep "Downloading https://cdn.proj.org/fr_ign_ntf_r93.tif" >/dev/null || (cat ${TMP_OUT}; exit 100)

if test ! -f ${PROJ_USER_WRITABLE_DIRECTORY}/fr_ign_ntf_r93.tif; then
    echo "*** ERROR: Can not find ${PROJ_USER_WRITABLE_DIRECTORY}/fr_ign_ntf_r93.tif!"
    exit 100
fi

echo "Testing $EXE --file ntf_r93 a second time"
if ! $EXE --file ntf_r93 > ${TMP_OUT}; then
    echo "--file ntf_r93 failed"
    cat ${TMP_OUT}
    exit 100
fi
cat ${TMP_OUT} | grep "https://cdn.proj.org/fr_ign_ntf_r93.tif already downloaded" >/dev/null || (cat ${TMP_OUT}; exit 100)

rm -f ${PROJ_USER_WRITABLE_DIRECTORY}/fr_ign_ntf_r93.tif


echo "Testing $EXE --file non_existing"
if $EXE --file non_existing >${TMP_OUT} 2>&1 ; then
    echo "--file non_existing failed"
    cat ${TMP_OUT}
    exit 100
fi
cat ${TMP_OUT} | grep "Warning: 'non_existing' is a unknown value for --file." >/dev/null || (cat ${TMP_OUT}; exit 100)
cat ${TMP_OUT} | grep "fr_ign_ntf_r93.tif" >/dev/null || (cat ${TMP_OUT}; exit 100)


echo "Testing $EXE --bbox 2,49,3,50"
if ! $EXE --bbox 2,49,3,50 --dry-run > ${TMP_OUT}; then
    echo "--bbox 2,49,3,50 failed"
    cat ${TMP_OUT}
    exit 100
fi
cat ${TMP_OUT} | grep "fr_ign_ntf_r93.tif" >/dev/null || (cat ${TMP_OUT}; exit 100)
cat ${TMP_OUT} | grep "us_nga_egm96_15.tif" >/dev/null || (cat ${TMP_OUT}; exit 100)


echo "Testing $EXE --bbox 2,49,3,50 --exclude-world-coverage"
if ! $EXE --bbox 2,49,3,50 --exclude-world-coverage --dry-run > ${TMP_OUT}; then
    echo "--bbox 2,49,3,50 --exclude-world-coverage failed"
    cat ${TMP_OUT}
    exit 100
fi
cat ${TMP_OUT} | grep "fr_ign_ntf_r93.tif" >/dev/null || (cat ${TMP_OUT}; exit 100)
if cat ${TMP_OUT} | grep "us_nga_egm96_15.tif" >/dev/null; then
    cat ${TMP_OUT}
    exit 100
fi


echo "Testing $EXE --bbox 170,-90,-170,90"
if ! $EXE --bbox 170,-90,-170,90 --dry-run > ${TMP_OUT}; then
    echo "--bbox 170,-90,-170,90 failed"
    cat ${TMP_OUT}
    exit 100
fi
cat ${TMP_OUT} | grep "nz_linz_nzgeoid2009.tif" >/dev/null || (cat ${TMP_OUT}; exit 100)
cat ${TMP_OUT} | grep "us_noaa_alaska.tif" >/dev/null || (cat ${TMP_OUT}; exit 100)


echo "Testing $EXE --bbox 170,-90,190,90"
if ! $EXE --bbox 170,-90,190,90 --dry-run > ${TMP_OUT}; then
    echo "--bbox 170,-90,190,90 failed"
    cat ${TMP_OUT}
    exit 100
fi
cat ${TMP_OUT} | grep "nz_linz_nzgeoid2009.tif" >/dev/null || (cat ${TMP_OUT}; exit 100)
cat ${TMP_OUT} | grep "us_noaa_alaska.tif" >/dev/null || (cat ${TMP_OUT}; exit 100)


echo "Testing $EXE --bbox -190,-90,-170,90"
if ! $EXE --bbox -190,-90,-170,90 --dry-run > ${TMP_OUT}; then
    echo "--bbox -190,-90,-170,90 failed"
    cat ${TMP_OUT}
    exit 100
fi
cat ${TMP_OUT} | grep "nz_linz_nzgeoid2009.tif" >/dev/null || (cat ${TMP_OUT}; exit 100)
cat ${TMP_OUT} | grep "us_noaa_alaska.tif" >/dev/null || (cat ${TMP_OUT}; exit 100)

cat > dummy.geojson <<EOF
{
"type": "FeatureCollection",
"features": [
{ "type": "Feature", "properties": { "url": "https://example.com/removed_in_1.7.tif", "name": "removed_in_1.7", "area_of_use": "Australia", "type": "VERTICAL_OFFSET_GEOGRAPHIC_TO_VERTICAL", "source_crs_code": "EPSG:7843", "source_crs_name": "GDA2020", "target_crs_code": "EPSG:9458", "target_crs_name": "AVWS height", "source": "Geoscience Australia", "source_country": "Australia", "source_id": "au_ga", "source_url": "http://www.ga.gov.au", "description": "GDA2020 (EPSG:7843) to AVWS height (EPSG:9458)", "file_size": 60878523, "sha256sum": "c06f24dae030587cc2556a316782ade83b6d91f1b8d03922ddd0ce58f3868baa", "version_removed": "1.7" }, "geometry": { "type": "Polygon", "coordinates": [ [ [ 93.0083333, -61.0083333 ], [ 174.0083333, -61.0083333 ], [ 174.0083333, -8.0083333 ], [ 93.0083333, -8.0083333 ], [ 93.0083333, -61.0083333 ] ] ] } },
{ "type": "Feature", "properties": { "url": "https://example.com/added_in_99_99.tif", "name": "added_in_99_99.tif", "area_of_use": "Australia", "type": "VERTICAL_OFFSET_GEOGRAPHIC_TO_VERTICAL", "source_crs_code": "EPSG:7843", "source_crs_name": "GDA2020", "target_crs_code": "EPSG:9458", "target_crs_name": "AVWS height", "source": "Geoscience Australia", "source_country": "Australia", "source_id": "au_ga", "source_url": "http://www.ga.gov.au", "description": "GDA2020 (EPSG:7843) to AVWS height (EPSG:9458)", "file_size": 60878523, "sha256sum": "c06f24dae030587cc2556a316782ade83b6d91f1b8d03922ddd0ce58f3868baa", "version_added": "99.99" }, "geometry": { "type": "Polygon", "coordinates": [ [ [ 93.0083333, -61.0083333 ], [ 174.0083333, -61.0083333 ], [ 174.0083333, -8.0083333 ], [ 93.0083333, -8.0083333 ], [ 93.0083333, -61.0083333 ] ] ] } },
{ "type": "Feature", "properties": { "url": "https://example.com/without_version.tif", "name": "without_version.tif", "area_of_use": "Australia", "type": "VERTICAL_OFFSET_GEOGRAPHIC_TO_VERTICAL", "source_crs_code": "EPSG:7843", "source_crs_name": "GDA2020", "target_crs_code": "EPSG:9458", "target_crs_name": "AVWS height", "source": "Geoscience Australia", "source_country": "Australia", "source_id": "au_ga", "source_url": "http://www.ga.gov.au", "description": "GDA2020 (EPSG:7843) to AVWS height (EPSG:9458)", "file_size": 60878523, "sha256sum": "c06f24dae030587cc2556a316782ade83b6d91f1b8d03922ddd0ce58f3868baa" }, "geometry": { "type": "Polygon", "coordinates": [ [ [ 93.0083333, -61.0083333 ], [ 174.0083333, -61.0083333 ], [ 174.0083333, -8.0083333 ], [ 93.0083333, -8.0083333 ], [ 93.0083333, -61.0083333 ] ] ] } }
]
}
EOF

echo "Testing $EXE --source-id au_ga --verbose --dry-run --local-geojson-file dummy.geojson"
if ! $EXE --source-id au_ga --verbose --dry-run --local-geojson-file dummy.geojson > ${TMP_OUT}; then
    echo "--source-id au_ga --verbose --dry-run --local-geojson-file dummy.geojson"
    cat ${TMP_OUT}
    exit 100
fi
rm dummy.geojson
cat ${TMP_OUT} | grep "Skipping removed_in_1.7 as it is no longer useful starting with PROJ-data 1.7" >/dev/null || (cat ${TMP_OUT}; exit 100)
cat ${TMP_OUT} | grep "Skipping added_in_99_99.tif as it is only useful starting with PROJ-data 99.99" >/dev/null || (cat ${TMP_OUT}; exit 100)
cat ${TMP_OUT} | grep "Would download https://cdn.proj.org/without_version.tif" >/dev/null || (cat ${TMP_OUT}; exit 100)

# Cleanup
rm -rf ${PROJ_USER_WRITABLE_DIRECTORY}
rm ${TMP_OUT}

exit 0
