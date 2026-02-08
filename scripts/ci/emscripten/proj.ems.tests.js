const assert = require("node:assert/strict");
const ProjModuleFactory = require("./projModule.js");

// Small class to help to manage memory.
class Keeper {
    constructor(proj, debug = false) {
        if (!proj) {
            throw new Error("proj cannot be empty in class Keeper");
        }

        this.proj = proj;
        this.debug = debug;
        this.to_free = [];
        this.to_destroy = [];
    };

    add(ptr, proj_destroy = true) {
        if (proj_destroy) {
            if (this.debug)
                console.debug("add destroy", ptr);
            this.to_destroy.push(ptr);
        } else {
            if (this.debug)
                console.debug("add free", ptr);
            this.to_free.push(ptr);
        }
        return ptr;
    };

    call(name, ...args) {
        const ptr = this.proj[name](...args);
        return this.add(ptr, true);
    }

    malloc(ptrSize) {
        const ptr = this.proj._malloc(ptrSize);
        return this.add(ptr, false);
    };

    string(str) {
        const ptr = this.proj.stringToNewUTF8(str);
        return this.add(ptr, false);
    };

    destroy() {
        this.to_destroy.reverse();
        for (const p of this.to_destroy) {
            if (this.debug)
                console.debug("call destroy", p);
            this.proj._proj_destroy(p);
        }
        this.to_destroy = [];

        this.to_free.reverse();
        for (const p of this.to_free) {
            if (this.debug)
                console.debug("call free", p);
            this.proj._free(p);
        }
        this.to_free = [];
    };
};

const tests = {
    test_proj_info : (proj) => {
        const r = proj.proj_info_ems();
        const compilation_date =
            proj.UTF8ToString(proj._get_compilation_date());
        // Print it for visual inspection
        console.log(r);
        console.log(compilation_date);
        assert(compilation_date.length > 5);
        assert(r.major >= 9);
        if (r.major === 9) {
            assert(r.minor >= 8);
        }
        assert(typeof r.minor === "number")
        assert(typeof r.patch === "number")
        const rel = `${r.major}.${r.minor}.${r.patch}`;
        assert(r.release.includes(rel));
        assert(r.version.includes(rel));
    },

    test_projinfo : (proj) => {
        function get_projinfo(args) {
            let msg = "";
            const myCallback = (level, message) => { msg += message; };
            const rc = proj.projinfo_ems(0, args, myCallback);
            return [ rc, msg ];
        } {
            let [rc, msg] = get_projinfo([ "EPSG:32633", "-o", "WKT1:GDAL" ]);
            assert.equal(rc, 0);
            assert.ok(msg.includes('PROJCS["WGS 84 / UTM zone 33N"'));
            assert.ok(msg.includes('AUTHORITY["EPSG","32633"]'));
        } {let [rc, msg] = get_projinfo([ "EPSG:32633", "-o", "invalid" ]);
           assert.equal(rc, 1);
           assert.ok(msg.includes('Unrecognized value '));} {
            let [rc, msg] = get_projinfo(
                [ "EPSG:4326", "EPSG:32633", "-o", "proj", "--single-line" ]);
            assert.equal(rc, 0);
            assert.ok(msg.includes('Candidate operations'));
            assert.ok(
                msg.includes(('+proj=pipeline +step +proj=axisswap +order=2,1 ',
                              '+step +proj=unitconvert +xy_in=deg +xy_out=rad ',
                              '+step +proj=utm +zone=33 +ellps=WGS84')));
        }
    },

    test_convert : (proj) => {
        let ctx = proj._proj_context_create();
        if (ctx === 0) {
            throw new Error("proj_context_create returned NULL");
        }
        const sourceCRS = proj.stringToNewUTF8("EPSG:4326");
        const targetCRS = proj.stringToNewUTF8("EPSG:32633");
        const P = proj._proj_create_crs_to_crs(ctx, sourceCRS, targetCRS, 0);
        if (P === 0) {
            throw new Error("proj_create_crs_to_crs returned NULL.");
        }
        // PJ_COORD is 4 doubles: (x, y, z, t) or (lon, lat, z, t)
        // We need 4 * 8 = 32 bytes of memory
        let coordPtr = proj._malloc(32);
        const coordView = new Float64Array(proj.HEAPF64.buffer, coordPtr, 4);
        coordView[0] = 52;
        coordView[1] = 13.5;
        coordView[2] = 0;
        coordView[3] = Infinity; // HUGE_VAL

        let res = proj._proj_trans_array(P, 1, 1, coordPtr);
        if (res != 0) {
            let msgPtr = proj._proj_context_errno_string(ctx, res);
            let msg = proj.UTF8ToString(msgPtr);
            throw new Error(`_proj_trans_array error ${msg}`);
        }
        assert(Math.abs(coordView[0] - 397027.0183) < 1e-3);
        assert(Math.abs(coordView[1] - 5762100.4897) < 1e-3);

        proj._free(coordPtr);
        proj._proj_destroy(P);
        proj._free(sourceCRS);
        proj._free(targetCRS);
        proj._proj_destroy(ctx);
    },

    test_axes : (proj) => {
        let ctx = proj._proj_context_create();
        if (ctx === 0) {
            throw new Error("proj_context_create returned NULL");
        }
        function get_str(ptr) {
            const strPtr = proj.getValue(ptr, 'i32'); // It's a pointer (i32)
            const str = proj.UTF8ToString(strPtr);
            return str;
        }
        function get_axes(crs) {
            let keep = new Keeper(proj);
            try {
                // TODO support compound CRS

                const ptrSize = proj._get_ptr_size();
                assert.ok(ptrSize == 4 || ptrSize == 8);
                const doubleSize = 8; // Doubles are 8 bytes
                const sourceCRS = keep.string(crs);
                const P_crs = keep.call("_proj_create", ctx, sourceCRS);
                const P_cs =
                    keep.add(proj._proj_crs_get_coordinate_system(ctx, P_crs));
                const axis_count = proj._proj_cs_get_axis_count(ctx, P_cs);
                const outNamePtr = keep.malloc(ptrSize);
                const outAbbrevPtr = keep.malloc(ptrSize);
                const outDirectionPtr = keep.malloc(ptrSize);
                const outConvFactorPtr = keep.malloc(doubleSize);
                const outUnitPtr = keep.malloc(ptrSize);
                res = {
                    name : [],
                    abbr : [],
                    direction : [],
                    conv_factor : [],
                    unit : []
                };
                for (let i = 0; i < axis_count; i++) {
                    const r = proj._proj_cs_get_axis_info(
                        ctx, P_cs, i, outNamePtr, outAbbrevPtr, outDirectionPtr,
                        outConvFactorPtr, outUnitPtr, 0, 0);
                    if (r != 1) {
                        throw new Error("error calling proj_cs_get_axis_info");
                    }

                    res.name.push(get_str(outNamePtr));
                    res.abbr.push(get_str(outAbbrevPtr));
                    res.direction.push(get_str(outDirectionPtr));
                    res.conv_factor.push(
                        proj.getValue(outConvFactorPtr, 'double'));
                    res.unit.push(get_str(outUnitPtr));
                }
            } finally {
                keep.destroy();
            }
            return res;
        }
        const ax4326 = get_axes("EPSG:4326");
        assert.deepEqual(ax4326, {
            name : [ 'Geodetic latitude', 'Geodetic longitude' ],
            abbr : [ 'Lat', 'Lon' ],
            direction : [ 'north', 'east' ],
            conv_factor : [ 0.017453292519943295, 0.017453292519943295 ],
            unit : [ 'degree', 'degree' ]
        });

        const ax25833 = get_axes("EPSG:25833");
        assert.deepEqual(ax25833, {
            name : [ 'Easting', 'Northing' ],
            abbr : [ 'E', 'N' ],
            direction : [ 'east', 'north' ],
            conv_factor : [ 1, 1 ],
            unit : [ 'metre', 'metre' ]
        });

        const ax3855 = get_axes("EPSG:3855");
        assert.deepEqual(ax3855, {
            name : [ 'Gravity-related height' ],
            abbr : [ 'H' ],
            direction : [ 'up' ],
            conv_factor : [ 1 ],
            unit : [ 'metre' ]
        });
        proj._proj_destroy(ctx);
    }
};

if (typeof ProjModuleFactory === 'undefined') {
    console.error(
        "'ProjModuleFactory' is not defined. Have you loaded projModule.js.");
    process.exit(1);
} else {
    ProjModuleFactory()
        .then(module => {
            proj = module;
            console.log("Starting tests");
            let counter = 0;
            for (let test of Object.keys(tests)) {
                console.log(`+++++ Running test "${test}"`)
                tests[test](proj);
                counter++;
            };
            console.log(`Number of tests run: ${counter}`)
            process.exit(0);
        })
        .catch(err => {
            console.error(`Tests failed: ${err}`);
            process.exit(1);
        });
}
