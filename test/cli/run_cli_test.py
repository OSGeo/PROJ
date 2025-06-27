#!/usr/bin/env python3
"""Run a CLI test provided as a YAML file.

Requirements
------------

- Python 3.7 or later
- Either ruamel.yaml (preferred) or pyyaml, from pip, conda or similar
- Self-testing needs pytest (`python3 -m pytest`)

Usage
-----

A minimal example to run all tests defined in `test_proj.yaml` is:
```
$ python3 run_cli_test.py test_proj.yaml
```

To see full usages, run `python3 run_cli_test.py --help`

Test file
---------

Test files are defined in YAML format, and are designed to run with one
executable. The top-level keys are:

- `exe`: (required) executable name, e.g. `proj`. This can be overridden
  from the command-line using (e.g.) `--exe /path/to/build/bin/proj`
- `env`: (optional) mapping of environment variables common to all tests
- `comment`: (optional) comment to describe purpose of tests
- `tests`: (required) list of tests, described next

Each test item have the optional keys:

- `args`: command-line arguments passed as a single string, or as a list of
  individual string arguments
- `comment`: comment to describe the test
- `env`: mapping of environment variables
- `input`: standard input for program, either as string or bytes
- `stdout`: expected standard output from program, either as string or bytes
- `stderr`: expected standard error from program as a string
- `out`: expected combined `stdout` and `stderr` as a string;
   this option can't be used with `stdout` and/or `stderr`
- `exitcode`: expected exit code (or return code) from the program, default 0
- `skipif`: a Python expression to evaluate if test should be skipped;
  to access the environment variable mapping, use (e.g.) `env.get('VAR')`;
  other special variables include `byteorder` (either "big" or "little") and
  `platform` (usually "linux", "darwin", or "win32")

Each test may have additional features:

- `file` - either a mapping or list of mappings with required keys `name` (str)
  and `content` (str or bytes), used to specify create one or more files,
  then removed after test. The `name` can specify a file to be written to the
  current working directory, or an absolult path to another location. The
  path may have a special `$tmpdir/file.txt` to write into a temporary
  directory, described next.
- `tmpdir` - create a temporary directory, and copy zero or more files
  listed in `copy`.

Lastly, test results can be post-processed using several commands, processed
in the same order they are specified:

- `grep`: capture lines that match pattern
- `grep-v`: capture lines that don't match pattern
- `sub`: substitute pattern with replacement, similar to `sed s/pat/repl/`
- `head`: select the first *n* lines
- `tail`: select the last *n* lines
- `sort`: sort the lines

Development
-----------

Running self-tests with pytest:
```
python3 -m pytest run_cli_test.py -v
```

Running coverage scan:
```
python3 -m coverage run --source=. -m pytest run_cli_test.py
python3 -m coverage html
```
"""
###############################################################################
#
#  Project:  PROJ
#  Purpose: Test command-line interface
#  Author:  Mike Taves <mwtoews at gmail.com>
#
###############################################################################
#  Copyright (c) 2024, Mike Taves <mwtoews at gmail.com>
#
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included
#  in all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
#  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#  DEALINGS IN THE SOFTWARE.
###############################################################################

__author__ = "Mike Taves"
__version__ = "0.1"

import difflib
import functools
import os
import re
import shlex
import shutil
import subprocess
import sys
import tempfile
from collections.abc import Mapping
from copy import deepcopy
from dataclasses import dataclass, field
from pathlib import Path
from textwrap import dedent
from typing import List, Optional, Union

# at runtime, import either ruamel.yaml or pyyaml lib

# always show unbuffered output
print = functools.partial(print, flush=True)


def sub_env_var(txt: str, env: dict) -> str:
    """Substitute environment variables into string, if found.

    Also change pathsep for Windows from ':' to ';'.
    """
    if os.pathsep != ":" and ":" in txt:
        txt = txt.replace(":", os.pathsep)
    if "$" not in txt:
        return txt
    pat_subvar = [
        (r"\$([a-zA-Z_]\w*)", "${var}"),  # applies to $VAR
        (r"\$\{([a-zA-Z_]\w*)\}", "${{{var}}}"),  # and to ${VAR}
    ]
    for pat, subvar in pat_subvar:
        for var in set(re.findall(pat, txt)):
            if var in env:
                txt = txt.replace(subvar.format(var=var), env[var])
            else:
                print(f"env var {var} not found", file=sys.stderr)
        if "$" not in txt:
            return txt
    return txt


def eval_skipif(expr: str, env: dict) -> bool:
    """Evaluate an expression to determine if test should be skipped."""
    import ast

    if not expr:
        return False

    # Add a few useful system constants
    eval_env = {"env": env}
    if "byteorder" in expr:
        eval_env["byteorder"] = sys.byteorder  # type: ignore
    if "platform" in expr:
        eval_env["platform"] = sys.platform  # type: ignore

    # Declare "safe" ast node types
    # fmt: off
    safe_nodes = [
        "Expression", "Load", "Name", "Compare", "Subscript", "FormattedValue", "Index",
        "Dict", "Set", "List", "Tuple", "Constant", "JoinedStr", "Num", "Str", "NameConstant",
        "BoolOp", "BinOp", "UnaryOp", "IfExp", "And", "Or",
        "Eq", "NotEq", "Lt", "LtE", "Gt", "GtE", "Is", "IsNot", "In", "NotIn",
        "Add", "Sub", "Mult", "Div", "Mod", "FloorDiv", "Pow",
        "Not", "BitOr", "BitAnd", "BitXor", "LShift", "RShift", "Invert",
    ]
    # fmt: on
    expr_tree = ast.parse(expr, mode="eval")
    # First scan to find acceptable nodes to ignore
    ignore_ids = set()
    for node in ast.walk(expr_tree):
        if isinstance(node, ast.Call):
            child_nodes = list(ast.iter_child_nodes(node))
            nnode = child_nodes[0] if len(child_nodes) > 0 else None
            if isinstance(nnode, ast.Attribute) and 1 <= len(child_nodes) <= 3:
                if nnode.value.id == "env" and nnode.attr == "get":  # type: ignore
                    # Allow: env.get("key", "default")
                    ignore_ids.add(id(node))
                    for child in child_nodes:
                        ignore_ids.add(id(child))
    # Second scan to raise issues with expression
    for node in ast.walk(expr_tree):
        if isinstance(node, ast.Name) and node.id not in eval_env:
            raise ValueError(f"undefined name {node.id!r}")
        elif id(node) in ignore_ids:
            continue
        node_name = type(node).__name__
        if node_name not in safe_nodes:
            raise ValueError(f"unsafe expression {expr!r}, contains {node_name!r}")

    res = eval(expr, eval_env)
    return bool(res)


@dataclass
class TmpDir:
    """Create a temporary directory, and optionally copy files."""

    copy: list = field(default_factory=list)

    def __post_init__(self):
        self._prepared = False
        if not isinstance(self.copy, list):
            raise ValueError("tmpdir copy must be a list")

    def prepare(self, env):
        """Create temporary directory and copy files into it."""
        if self._prepared:
            return
        self.strpth = tempfile.mkdtemp()
        env["tmpdir"] = self.strpth  # used by sub_env_var
        self.pth = Path(self.strpth)
        self.copy_files = []
        for idx, pth in enumerate(self.copy):
            sub_pth = sub_env_var(pth, env)
            if sub_pth != pth:
                self.copy[idx] = sub_pth
            src_pth = Path(sub_pth)
            if not src_pth.exists():
                raise FileNotFoundError(f"cannot find {src_pth}")
            dst_pth = self.pth / src_pth.name
            if dst_pth.exists():
                raise FileExistsError(f"file exists {dst_pth}")
            dst_pth.write_bytes(src_pth.read_bytes())
            self.copy_files.append(src_pth)
        self._prepared = True

    def sub_var(self, txt) -> str:
        """Substitute string with $tmpdir into this temporary path."""
        if not self._prepared:
            raise ValueError(f"{self.__class__} not prepared")
        if "$tmpdir" in txt:
            txt = re.sub(r"(\$tmpdir)\b", self.strpth, txt)
        return txt

    def cleanup(self):
        if self._prepared:
            shutil.rmtree(self.strpth)
            self._prepared = False

    def __del__(self):
        if hasattr(self, "cleanup"):
            self.cleanup()


@dataclass
class File:
    """Write a named file with content.

    File name should not already exist in filesystem. Path can
    include "$tmpdir" if already using TmpDir.
    """

    name: str
    content: Union[str, bytes]

    def __post_init__(self):
        self._prepared = False
        if not isinstance(self.name, (str, os.PathLike)):
            raise ValueError("file name must be str or path-like")
        if isinstance(self.content, str):
            if len(self.content) > 0 and not self.content.endswith("\n"):
                self.content += "\n"
        elif not isinstance(self.content, bytes):
            raise ValueError("file content must be str or bytes")

    def prepare(self, env):
        if self._prepared:
            return
        self.name = sub_env_var(self.name, env)
        self.pth = Path(self.name)
        if self.pth.exists():
            raise FileExistsError(f"{self.name} exists!")
        file_contents = self.content
        if isinstance(file_contents, str):
            self.pth.write_text(file_contents, encoding="utf-8")
        else:
            self.pth.write_bytes(file_contents)
        self._prepared = True

    def cleanup(self):
        if self._prepared:
            if self.pth.exists():
                self.pth.unlink()
            self._prepared = False

    def __del__(self):
        if hasattr(self, "cleanup"):
            self.cleanup()


@dataclass
class Test:
    args: List[str] = field(default_factory=list)
    comment: Optional[str] = None
    env: dict = field(default_factory=dict)
    input: Union[None, str, bytes] = None  # or "in"
    stdout: Union[None, List[str], bytes] = None
    stderr: Union[None, List[str]] = None
    out: Union[None, str, List[str]] = None
    out_cmds: dict = field(default_factory=dict)
    exitcode: int = 0
    skipif: Optional[str] = None
    file: List[File] = field(default_factory=list)
    tmpdir: Optional[TmpDir] = None

    __test__ = False  # prevent pytest from autodiscover
    expected_out_cmds = ["grep", "grep-v", "sub", "head", "tail", "sort"]

    @classmethod
    def from_item(cls, verbose=0, **item):
        cls_kwargs = {}
        out_cmds = {}  # keep order
        for key in item.keys():
            value = item[key]
            if key in cls.expected_out_cmds:
                out_cmds[key] = value
            else:
                if key == "in":  # rename reserved Python name
                    key = "input"
                cls_kwargs[key] = value
        return cls(**cls_kwargs, out_cmds=out_cmds)

    def __post_init__(self):
        """Do some checks and modify data, but hold-off creating files etc."""
        self._prepared = False
        self.skip = False

        if isinstance(self.args, str):
            self.args = shlex.split(self.args)
        if isinstance(self.input, str) and not self.input.endswith("\n"):
            self.input += "\n"

        def text2list(text):
            if not text:
                return []
            if not text.endswith("\n"):
                text += "\n"
            return text.splitlines(keepends=True)

        self.check_stdout = self.stdout is not None
        if self.check_stdout and isinstance(self.stdout, str):
            self.stdout = text2list(self.stdout)
        self.check_stderr = self.stderr is not None
        if self.check_stderr and isinstance(self.stderr, str):
            self.stderr = text2list(self.stderr)
        self.check_out = self.out is not None
        if self.check_out and isinstance(self.out, str):
            if self.check_stdout or self.check_stderr:
                raise ValueError("out cannot be used with stdout or stderr")
            self.stdout = text2list(self.out)

        for key in self.env.keys():
            value = self.env[key]
            if not isinstance(value, str):
                self.env[key] = str(value)

        if self.tmpdir is not None and not isinstance(self.tmpdir, TmpDir):
            self.tmpdir = TmpDir(**self.tmpdir)

        if self.file:
            # make into list of File
            if not isinstance(self.file, list):
                self.file = [self.file]
            for idx, file in enumerate(self.file):
                if not isinstance(file, File):
                    self.file[idx] = File(**file)

        for key in list(self.out_cmds):
            value = self.out_cmds[key]
            if key in {"grep", "grep-v"}:
                if isinstance(value, str):
                    self.out_cmds[key] = [value]
                elif not isinstance(value, list):
                    raise ValueError(
                        f"unsupported grep type {type(value)} for {value}",
                    )
            elif key == "sub":
                if (
                    not isinstance(value, list)
                    or len(value) != 2
                    or not all(isinstance(val, str) for val in value)
                ):
                    raise ValueError("sub should be a list of 2 str")

    def prepare(self, exe: str, global_env: dict):
        if not self._prepared:
            self.exe = exe
            self.global_env = global_env.copy()
            if self.skipif is not None:
                self.skip = eval_skipif(self.skipif, self.global_env)
                if self.skip:
                    return
            if self.tmpdir is not None:
                self.tmpdir.prepare(self.global_env)
                for idx, arg in enumerate(self.args):
                    arg2 = self.tmpdir.sub_var(arg)
                    if arg != arg2:
                        self.args[idx] = arg2
            for key in self.env.keys():
                self.env[key] = sub_env_var(str(self.env[key]), self.global_env)
            self.global_env.update(self.env)
            for file in self.file:
                file.prepare(self.global_env)
            self._prepared = True

    def get_cmd(self) -> str:
        """Get an approximate POSIX shell command to replicate test."""
        if self.skip:
            raise ValueError("test is skipped")
        elif not self._prepared:
            raise ValueError("test not prepared")
        cmd = ""
        if self.tmpdir is not None:
            cmd += f"mkdir {shlex.quote(self.tmpdir.strpth)}\n"
            for pth in self.tmpdir.copy_files:
                cmd += f"cp {shlex.quote(str(pth))} {shlex.quote(self.tmpdir.strpth)}\n"
        for file in self.file:
            efile_name = shlex.quote(file.name)
            file_content = file.content
            if len(file_content) == 0:
                cmd += f"touch {efile_name}\n"
            elif isinstance(file_content, str):
                num_file_lines = sum(
                    1 if len(line) > 0 else 0 for line in file_content.splitlines()
                )
                if num_file_lines == 1:
                    cmd += "echo "
                    file_content_rstrip = file_content.rstrip()
                    if file_content_rstrip.isprintable():
                        cmd += shlex.quote(file_content_rstrip)
                    else:
                        # best attempt to capture Unicode and other
                        cmd += "-e " + repr(file_content_rstrip)
                    cmd += f" > {efile_name}\n"
                else:
                    cmd += f"cat << EOF > {efile_name}\n{file_content}EOF\n"
            elif isinstance(file_content, bytes):
                cmd += f"printf {file_content.decode('utf-8')!r} > {efile_name}\n"
        num_input = 0
        if isinstance(self.input, str):
            num_input = sum(
                1 if len(line) > 0 else 0 for line in self.input.splitlines()
            )
            if num_input == 1:
                cmd += "echo "
                input_rstrip = self.input.rstrip()
                if input_rstrip.isprintable():
                    cmd += shlex.quote(input_rstrip)
                else:
                    # best attempt to capture Unicode and other
                    cmd += f"-e {input_rstrip!r}"
                cmd += " | "
        elif isinstance(self.input, bytes):
            cmd += f"printf {repr(self.input)[1:]} | "
        for key, value in self.env.items():
            cmd += f"{key}={shlex.quote(value)} "
        cmd += shlex.quote(self.exe)
        for arg in self.args:
            cmd += " " + shlex.quote(arg)
        if isinstance(self.input, str) and num_input > 1:
            cmd += " <<EOF\n" + self.input + "EOF"
        pipe = "2>&1 |" if self.check_out else "|"  # assume only stdout
        for key, value in self.out_cmds.items():
            if key in {"head", "tail"}:
                cmd += f" {pipe} {key} -n {value}"
            elif key == "grep":
                for lval in value:
                    cmd += f" {pipe} grep {shlex.quote(re.escape(lval))}"
            elif key == "grep-v":
                for lval in value:
                    cmd += f" {pipe} grep -v {shlex.quote(re.escape(lval))}"
            elif key == "sub":
                expr = "s/{}/{}/".format(*[re.escape(lval) for lval in value])
                cmd += f" {pipe} sed {shlex.quote(expr)}"
            elif key == "sort":
                cmd += f" {pipe} {key}"
            else:
                raise NotImplementedError(f"unsupported out cmd: {key}={value}")
        if isinstance(self.stdout, bytes):
            cmd += " | base64 -"
        if self.file:
            cmd += "\nrm"
            for file in self.file:
                cmd += f" {shlex.quote(file.name)}"
        if self.tmpdir is not None:
            cmd += f"\nrm -rf {shlex.quote(self.tmpdir.strpth)}"
        return cmd

    def run(self):
        """Run CLI test, check and return TestResult."""
        if self.skip:
            raise ValueError("test is skipped")
        elif not self._prepared:
            raise ValueError("test not prepared")
        run_kwargs = {"env": self.global_env, "text": False}
        if isinstance(self.input, str):
            run_kwargs["input"] = self.input.encode()
        elif self.input:
            run_kwargs["input"] = self.input
        if self.check_out:
            # combine stdout and stderr into stdout
            run_kwargs.update({"stdout": subprocess.PIPE, "stderr": subprocess.STDOUT})
        else:  # separate stdout and stderr streams
            run_kwargs["capture_output"] = True

        # actually run exe
        proc = subprocess.run([self.exe] + self.args, **run_kwargs)

        def bytes2listofstr(bytes):
            if not bytes:
                return []
            if os.linesep != "\n":  # Handle Windows EOL
                return (
                    bytes.decode("utf-8", errors='ignore')
                    .replace(os.linesep, "\n")
                    .splitlines(keepends=True)
                )
            return bytes.decode("utf-8", errors='ignore').splitlines(keepends=True)

        stdout_is_text = isinstance(self.stdout, list) or isinstance(self.out, list)
        stdout = bytes2listofstr(proc.stdout) if stdout_is_text else proc.stdout
        stderr = bytes2listofstr(proc.stderr)

        # post-process results with out_cmds, in the order they were described
        for key, value in self.out_cmds.items():
            if key == "head":
                if stdout_is_text:
                    stdout = stdout[:value]
                stderr = stderr[:value]
            elif key == "tail":
                if stdout_is_text:
                    stdout = stdout[-value:]
                stderr = stderr[-value:]
            elif key == "grep":
                for lval in value:
                    grp = re.compile(lval)
                    if stdout_is_text:
                        stdout = [line for line in stdout if grp.search(line)]
                    stderr = [line for line in stderr if grp.search(line)]
            elif key == "grep-v":
                for lval in value:
                    grp = re.compile(lval)
                    if stdout_is_text:
                        stdout = [line for line in stdout if not grp.search(line)]
                    stderr = [line for line in stderr if not grp.search(line)]
            elif key == "sub":
                pat, repl = value
                sub = re.compile(pat)
                if stdout_is_text:
                    stdout = [sub.sub(repl, line) for line in stdout]
                stderr = [sub.sub(repl, line) for line in stderr]
            elif key == "sort":
                if stdout_is_text:
                    stdout = sorted(stdout)
                stderr = sorted(stderr)
            else:
                raise NotImplementedError(f"unsupported out cmd: {key}={value}")

        return TestResult(stdout, stderr, proc.returncode, expected=self)

    def cleanup(self):
        if self._prepared:
            for file in getattr(self, "file", []):
                file.cleanup()
            if getattr(self, "tmpdir", None) is not None:
                self.tmpdir.cleanup()
            self._prepared = False

    def __del__(self):
        if hasattr(self, "cleanup"):
            self.cleanup()


@dataclass
class TestResult:
    """Returned by Test.run()."""

    stdout: Union[None, List[str], bytes]
    stderr: Union[None, List[str]]
    exitcode: int
    expected: Test

    __test__ = False  # prevent pytest from autodiscover

    def __post_init__(self):
        """Evaluate if test has failed."""
        self.failed = set()
        if self.expected.check_stdout and self.expected.stdout != self.stdout:
            self.failed.add("stdout")
        if self.expected.check_stderr and self.expected.stderr != self.stderr:
            self.failed.add("stderr")
        if self.expected.check_out and self.expected.stdout != self.stdout:
            self.failed.add("out")
        if self.expected.exitcode != self.exitcode:
            self.failed.add("exitcode")

    def __bool__(self):
        """False for failed test result."""
        return not bool(self.failed)

    def _get_differences(self, which, dispwhich=None):
        ret = getattr(self, which)
        exp = getattr(self.expected, which)
        if dispwhich is None:
            dispwhich = which
        if isinstance(exp, bytes) or isinstance(ret, bytes):
            return "Returned different binary data"
        if ret and exp is None:
            plr = "" if len(ret) == 1 else "s"
            txt = f"Returned {len(ret)} line{plr} to {dispwhich} but none was expected"
        elif not ret and exp:
            plr = "" if len(exp) == 1 else "s"
            txt = f"No {dispwhich} was returned but expected {len(exp)} line{plr}"
        else:
            diff = difflib.ndiff(exp, ret)
            txt = f"Different {dispwhich} returned (+) than expected (-):\n"
            txt += "".join(diff).rstrip()
        return txt

    def describe_fail(self, file=sys.stderr):
        """Describe (print) how test has failed."""
        if "stdout" in self.failed:
            print(self._get_differences("stdout"), file=file)
        if "stderr" in self.failed:
            print(self._get_differences("stderr"), file=file)
        if "out" in self.failed:
            print(self._get_differences("stdout", "stdout+stderr"), file=file)
        if "exitcode" in self.failed:
            print(
                f"Expected exit code {self.expected.exitcode} but returned {self.exitcode}",
                file=file,
            )

    def describe_nonfail(self, file=sys.stdout):
        """Describe (print) non-failing test results."""
        if not self.failed.intersection({"stdout", "out"}) and self.stdout:
            print("Returned stdout:", file=file)
            if isinstance(self.stdout, list):
                print("".join(self.stdout), file=file)
            else:
                print(self.stdout, file=file)
        if "stderr" not in self.failed and self.stderr:
            print("Returned stderr:", file=file)
            if isinstance(self.stderr, list):
                print("".join(self.stderr), file=file)
            else:
                print(self.stderr, file=file)

    def update_expected(self, tester: "Tester"):
        """Update expected result for tester to re-write."""
        if not tester.update:
            raise ValueError("tester cannot be updated, since update=False")
        test = tester.data["tests"][self.num]  # type: ignore

        def do_update(key: str, thing):
            if isinstance(thing, list):
                thing_str = "".join(thing)
                # Clip newline for one-liners, but prevent
                # LiteralScalarString from changing "|" to "|-"
                cn = test[key].__class__.__name__
                if len(thing) == 1 and cn != "LiteralScalarString":
                    thing_str = thing_str.rstrip()
                test[key] = thing_str
            else:
                test[key] = thing
            print(f"Updated expected {key}")
            return

        if "out" in self.failed:
            do_update("out", self.stdout)
        if "stdout" in self.failed:
            do_update("stdout", self.stdout)
        if "stderr" in self.failed:
            do_update("stderr", self.stderr)
        if "exitcode" in self.failed:
            do_update("exitcode", self.exitcode)


@dataclass
class Tester:
    """Holds list of tests and other common variables."""

    exe: str
    comment: Optional[str] = None
    env: dict = field(default_factory=dict)
    tests: List[Test] = field(default_factory=list, repr=False)
    update: bool = False
    verbose: int = 1

    __test__ = False  # prevent pytest from autodiscover
    expected_data_keys = {"exe", "comment", "env", "tests"}

    def __len__(self):
        return len(self.tests)

    def __getitem__(self, key):
        return self.tests[key]

    def __post_init__(self):
        for num, item in enumerate(self.tests):
            if not isinstance(item, Test):
                test = Test.from_item(verbose=self.verbose, **item)
                self.tests[num] = test
        for key in self.env.keys():
            value = self.env[key]
            if not isinstance(value, str):
                self.env[key] = str(value)

    @classmethod
    def from_yaml(cls, filename, update=False, preferred=None, verbose=1):
        supported_libs = {None, "ruamel.yaml", "pyyaml"}
        if preferred not in supported_libs:
            raise NotImplementedError(f"{preferred} not in {supported_libs}")
        yaml_lib = None
        if preferred == "ruamel.yaml" or preferred is None:
            try:
                from ruamel.yaml import YAML

                yaml_lib = "ruamel.yaml"
            except ImportError:
                pass
        if yaml_lib is None:
            try:
                import yaml
                from yaml.resolver import Resolver

                # https://stackoverflow.com/a/36470466/
                # remove resolver entries for On/Off/Yes/No
                for ch in "OoYyNn":
                    if ch not in Resolver.yaml_implicit_resolvers:
                        continue
                    if len(Resolver.yaml_implicit_resolvers[ch]) == 1:
                        del Resolver.yaml_implicit_resolvers[ch]
                    else:
                        Resolver.yaml_implicit_resolvers[ch] = [
                            x
                            for x in Resolver.yaml_implicit_resolvers[ch]
                            if x[0] != "tag:yaml.org,2002:bool"
                        ]
                yaml_lib = "pyyaml"
            except ImportError:
                pass
        if preferred is None:
            preferred = "either ruamel.yaml or pyyaml"
        if yaml_lib is None:
            raise ImportError(
                f"this script needs {preferred} "
                "which can be installed using pip, conda, apt, yum, ..."
            )
        if update:
            if yaml_lib != "ruamel.yaml":
                raise ImportError("ruamel.yaml required to update YAML file")
        if yaml_lib == "pyyaml":
            with open(filename, encoding="utf-8") as fp:
                data = yaml.safe_load(fp)
        elif yaml_lib == "ruamel.yaml":
            typ = "rt" if update else "safe"
            yaml = YAML(typ=typ)
            yaml.width = 200
            yaml.preserve_quotes = True
            with open(filename, encoding="utf-8") as fp:
                data = yaml.load(fp)
        else:
            raise NotImplementedError(yaml_lib)

        if not isinstance(data, Mapping):
            raise ValueError(f"expected dict-like structure to {filename}")
        if verbose:
            unused = set(data.keys()).difference(cls.expected_data_keys)
            if unused:
                print(f"unused top-level data: {unused}", file=sys.stderr)
        cls_kwargs = {
            key: data[key] for key in data.keys() if key in cls.expected_data_keys
        }
        if update:  # dereference object to modify
            cls_kwargs["tests"] = deepcopy(data["tests"])
        cls_kwargs.update({"verbose": verbose, "update": update})
        obj = cls(**cls_kwargs)
        if update:
            # Add some extra attributes to use later
            obj.data = data
            obj.yaml = yaml
            obj.filename = filename
        if verbose:
            plr = "" if len(obj) == 1 else "s"
            print(f"Read {filename} using {yaml_lib} with {len(obj)} test{plr}")
        return obj

    def run(self, glob_env, exitfirst=False) -> int:
        """Run all tests."""
        failures = []
        num_fail = 0
        num_pass = 0
        num_skip = 0
        glob_env = glob_env.copy()
        glob_env.update(self.env)
        for num, test in enumerate(self.tests):
            test.prepare(self.exe, glob_env)
            if self.verbose > 1:
                print("-" * 28)
                print(f"Test {num + 1}" + (f": {test.comment}" if test.comment else ""))
                print(test.get_cmd())
            if test.skip:
                num_skip += 1
                if self.verbose == 1:
                    print("x", end="")
                elif self.verbose > 1:
                    print(f"Test skipped: {test.skipif}")
                continue
            elif self.verbose > 1 and test.skipif is not None:
                print(f"Test not skipped: {test.skipif}")
            result = test.run()
            if result.failed:
                num_fail += 1
                result.num = num
                if self.verbose == 1:
                    print("F", end="")
                    result.cmd = test.get_cmd()
                    failures.append(result)
                elif self.verbose > 1:
                    print("Test failed", file=sys.stderr)
                    result.describe_nonfail()
                    result.describe_fail()
                if exitfirst:
                    test.cleanup()
                    break
            else:
                num_pass += 1
                if self.verbose == 1:
                    print(".", end="")
                elif self.verbose > 1:
                    result.describe_nonfail()
                    print("Test passed")
            test.cleanup()
        if self.verbose == 1:
            print()

        # show/update failures after all tests have run
        for result in failures:
            if self.verbose == 1:
                print("-" * 28)
                print(f"Test {result.num + 1} failed", file=sys.stderr)
                if result.expected.comment is not None:
                    print(f"comment: {result.expected.comment}")
                if result.expected.skipif is not None:
                    print(f"Test not skipped: {result.expected.skipif}")
                print(result.cmd)
                result.describe_nonfail()
                result.describe_fail()
            if self.update:
                result.update_expected(self)

        if self.update:
            filename, data, yaml = self.filename, self.data, self.yaml  # type: ignore
            with open(filename, "w", encoding="utf-8") as fp:
                yaml.dump(data, fp)
            if self.verbose:
                print(f"Updated {filename}")

        if self.verbose:
            if self.verbose > 1 or num_fail > 0:
                print("-" * 28)
            msg_list = []
            if num_pass:
                plr = "" if num_pass == 1 else "s"
                msg_list.append(f"{num_pass} test{plr} passed")
            if num_fail:
                plr = "" if num_fail == 1 else "s"
                msg_list.append(f"{num_fail} test{plr} failed")
            if num_skip:
                plr = "" if num_skip == 1 else "s"
                msg_list.append(f"{num_skip} test{plr} skipped")
            if len(self) > 1 and len(msg_list) == 1:
                msg = f"All {msg_list[0]}"
            else:
                msg = ", ".join(msg_list)
        if num_fail:
            if self.verbose:
                print(msg, file=sys.stderr)
            return 1
        if self.verbose:
            print(msg)
        return 0


def main(filename, exe=None, env=None, exitfirst=False, update=False, verbose=1):
    tester = Tester.from_yaml(filename, update=update, verbose=verbose)
    if exe is not None:
        tester.exe = exe
    glob_env = os.environ.copy()
    # Add/update PWD, which may be out-of-sync
    glob_env["PWD"] = str(Path.cwd())
    glob_env.update(tester.env)
    if env is not None:
        glob_env.update(env)
    if verbose:
        print("Relevant environment variables:")
        for key, value in glob_env.items():
            if key.startswith("PROJ") or "NABLED" in key:
                print(f"  {key}={value}")
    # Check exe
    exe_path = Path(tester.exe)
    full_exe_path = str(exe_path)
    if exe_path.is_dir():
        raise FileNotFoundError(f"'{exe_path}' is a directory, not a path to an EXE")
    elif tester.exe[1:3] == ":/":
        # CMake passes paths with forward slashes, which breaks D:/path/to/invproj.exe
        if full_exe_path[1:3] == ":/":  # on MSYS2 force to :\
            full_exe_path = full_exe_path.replace("/", "\\")
        tester.exe = full_exe_path
    elif exe_path.is_absolute():
        if not exe_path.exists():
            raise FileNotFoundError(f"cannot find '{tester.exe}' (does not exist)")
    else:
        full_exe_path = shutil.which(tester.exe)
        if full_exe_path is None:
            raise FileNotFoundError(f"cannot find '{tester.exe}' (not found on PATH)")
    if verbose:
        print(f"Testing '{full_exe_path}'")
    return tester.run(glob_env, exitfirst=exitfirst)


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument("filename")
    parser.add_argument(
        "--exe", help="path to executable; overrides exe from test file (if defined)"
    )
    parser.add_argument(
        "--env", action="append", help="environment variable, e.g. 'PROJ_NETWORK=ON'"
    )
    parser.add_argument(
        "-x",
        "--exitfirst",
        action="store_true",
        help="exit instantly on first failed test",
    )
    parser.add_argument(
        "--update",
        action="store_true",
        help="update filename with new results",
    )
    parser.add_argument(
        "--verbose",
        type=int,
        default=1,
        help="0 silent (except errors), 1 (default) some messages, 2 more more messages",
    )
    args = parser.parse_args()

    if args.env is not None:
        env_d = {}
        for item in args.env:
            if item.count("=") != 1:
                raise ValueError(f"env {item!r} does not use KEY=VALUE style")
            key, value = item.split("=")
            env_d[key] = value
        args.env = env_d
    sys.exit(main(**vars(args)))


# %% Tests for this utility


def test_sub_env_var():
    env = {"foo": "baz"}
    assert sub_env_var("foo=bar", env) == "foo=bar"
    assert sub_env_var("$foo/bar", env) == "baz/bar"
    assert sub_env_var("line with ${foo}. Or $foo.", env) == "line with baz. Or baz."
    assert sub_env_var("what $food this?", env) == "what $food this?"


def test_eval_skipif():
    import pytest

    assert eval_skipif("", {}) is False
    assert eval_skipif("0", {}) is False
    assert eval_skipif("not True is False", {}) is True
    assert eval_skipif("None is not True", {}) is True
    env = {"foo": "bar"}
    assert eval_skipif("env['foo'] == 'bar'", env) is True
    assert eval_skipif("env['foo'][1] == 'a'", env) is True
    assert eval_skipif("env.get('foo') == 'bar'", env) is True
    assert eval_skipif("env.get('no') == 'bar'", env) is False
    assert eval_skipif("env.get('no', 'bar') == 'bar'", env) is True
    assert eval_skipif("""f"re{env['foo']}" == 'rebar'""", env) is True
    assert eval_skipif("[1] != [2]", {}) is True
    assert eval_skipif("1 in (1,)", {}) is True
    assert eval_skipif("1 in {1, 2}", {}) is True
    assert eval_skipif("1 not in {3: 4}", {}) is True
    assert eval_skipif("2 - 1 < 2 + 1 * 2 and 1 / 2 <= 2 // 2", {}) is True
    assert eval_skipif("1 > 2 ** 2 or 1 >= 2", {}) is False
    assert eval_skipif("1 | 4 >> 1 if 4 % 1 << 2 else 2 & ~3 ^ 1", {}) is True

    # special values
    assert eval_skipif("byteorder == 'little'", {}) is (sys.byteorder == "little")
    assert eval_skipif("platform in ('linux', 'darwin')", {}) is (
        sys.platform in ("linux", "darwin")
    )

    # errors
    with pytest.raises(ValueError, match="undefined name 'abs'"):
        eval_skipif("abs == 'bar'", {})  # missing variable
    with pytest.raises(ValueError, match="unsafe expression"):
        eval_skipif("sys.exit()", {})
    with pytest.raises(ValueError, match="unsafe expression"):
        eval_skipif("eval('sys.exit()')", {})
    with pytest.raises(ValueError, match="unsafe expression"):
        eval_skipif("exec('sys.exit()')", {})
    with pytest.raises(ValueError, match="unsafe expression"):
        eval_skipif("foo.upper() == 'X'", {"foo": "x"})  # Call, Attribute


def test_tmpdir():
    import pytest

    td = TmpDir()
    assert td.copy == []
    env = {}
    td.prepare(env)
    td.prepare(env)  # check subsequent call
    assert list(env.keys()) == ["tmpdir"]
    assert td.strpth == env["tmpdir"]
    assert td.pth.exists()
    td.cleanup()
    assert not td.pth.exists()

    prevdir = Path().cwd()
    with tempfile.TemporaryDirectory() as otd:
        os.chdir(otd)
        text_pth = Path("my file.txt")
        text_pth.write_text("some\nlines\n")
        bin_pth = Path("file.bits")
        bin_pth.write_bytes(b"\x02\x03\x04\x05")
        td = TmpDir(["my file.txt", "file.bits"])
        assert td.copy == ["my file.txt", "file.bits"]
        td.prepare(env)
        assert td.pth.exists()
        assert set(pth.name for pth in td.pth.iterdir()) == {"my file.txt", "file.bits"}
        assert text_pth.read_text() == (td.pth / "my file.txt").read_text()
        assert bin_pth.read_bytes() == (td.pth / "file.bits").read_bytes()
        assert td.sub_var("$tmpdir/my file.txt") == f"{td.strpth}/my file.txt"
        assert td.sub_var("${tmpdir}/my file.txt") == "${tmpdir}/my file.txt"
        assert td.sub_var("$tmpdirs/my file.txt") == "$tmpdirs/my file.txt"
        td.cleanup()
        assert not td.pth.exists()
    os.chdir(prevdir)

    # errors
    with pytest.raises(ValueError, match="copy must be a list"):
        TmpDir("not a list")
    td = TmpDir(["/not/a/file.txt"])
    with pytest.raises(ValueError, match="not prepared"):
        td.sub_var("xyz")
    with pytest.raises(FileNotFoundError, match=r"cannot find /not/a/file\.txt"):
        td.prepare({})
    td = TmpDir(["$DIR/file.txt"])
    with pytest.raises(FileNotFoundError, match=r"cannot find /nope/file\.txt"):
        td.prepare({"DIR": "/nope"})


def test_file():
    import pytest

    prevdir = Path().cwd()
    with tempfile.TemporaryDirectory() as otd:
        os.chdir(otd)
        td_pth = Path(otd)

        text_file = File("my file.txt", "some\nlines\n")
        text_pth = td_pth / "my file.txt"
        assert not text_pth.exists()
        env = {}
        text_file.prepare(env)
        text_file.prepare(env)  # check subsequent call
        assert text_pth != text_file.pth
        assert env == {}
        assert text_pth.read_text() == "some\nlines\n"
        text_file.cleanup()
        assert not text_pth.exists()

        bin_file = File("file.bits", b"\x02\x03\x04\x05")
        bin_file.prepare(env)
        assert (td_pth / "file.bits").read_bytes() == b"\x02\x03\x04\x05"
        bin_file.cleanup()
    os.chdir(prevdir)

    # errors
    with pytest.raises(ValueError, match="file name must be str or path-like"):
        File(None, "some\nlines\n")
    with pytest.raises(ValueError, match="file content must be str or bytes"):
        File("my file.txt", None)


def test_tester_from_yaml():
    fd, fname = tempfile.mkstemp()
    with os.fdopen(fd, "w") as fp:
        fp.write(
            dedent(
                """\
                exe: cat
                env:
                  FOO: 1
                tests:
                - in: one
                  stdout: one
                  sort:
                - env:
                    FOO: 2
                  in: two
                  sub: ["w", "RrR"]
                  out: tRrRo
                """
            )
        )
    tester = Tester.from_yaml(fname)
    os.unlink(fname)

    assert tester.exe == "cat"
    assert tester.env == {"FOO": "1"}
    assert len(tester) == 2
    assert tester[0] == tester[-2]
    assert tester[0] != tester[-1]

    tests = [
        Test(input="one", stdout="one", out_cmds={"sort": None}),
        Test(
            input="two",
            out="tRrRo",
            env={"FOO": "2"},
            out_cmds={"sub": ["w", "RrR"]},
        ),
    ]
    assert tester == Tester("cat", env={"FOO": 1}, tests=tests)
    assert tester.run({}) == 0


def test_tester_from_yaml_with_update():
    import pytest

    pytest.importorskip("ruamel.yaml")
    fd, fname = tempfile.mkstemp()
    with os.fdopen(fd, "w") as fp:
        fp.write(
            dedent(
                """\
        exe: cat
        env:
          FOO: '1'
        tests:
        - in: |
            this is the new expected result
            over two lines
          stdout: |-
            replace me!
        # some comment
        - in: a b c
          out: |
            d e f
          exitcode: 8
        - in: here it is
          stderr: erase me
        - in: "tab\\tline"
          out: tabline
        - in: !!binary |
            AAAAAAAA4D8AAAAAAADQPw==
          # base64.b64encode(np.array([0.5, 0.25]).tobytes())
          stdout: !!binary ""
        """
            )
        )
    tester = Tester.from_yaml(fname, preferred="ruamel.yaml", update=True)
    tester.run({})
    with open(fname, "r") as fp:
        new = fp.read()
    assert new == dedent(
        """\
        exe: cat
        env:
          FOO: '1'
        tests:
        - in: |
            this is the new expected result
            over two lines
          stdout: |
            this is the new expected result
            over two lines
        # some comment
        - in: a b c
          out: |
            a b c
          exitcode: 0
        - in: here it is
          stderr: ''
        - in: "tab\\tline"
          out: "tab\\tline"
        - in: !!binary |
            AAAAAAAA4D8AAAAAAADQPw==
          # base64.b64encode(np.array([0.5, 0.25]).tobytes())
          stdout: !!binary |
            AAAAAAAA4D8AAAAAAADQPw==
        """
    )
    os.unlink(fname)


def test_test():
    import pytest

    # simple text pass
    test = Test(input="text", stdout="text", env={"FOO": 1})
    test.prepare("cat", {})
    assert not test.skip
    assert test.env == {"FOO": "1"}
    assert test.get_cmd() == "echo text | FOO=1 cat"
    result = test.run()
    assert result, result.describe_fail()
    assert result.failed == set()
    assert result == TestResult(["text\n"], [], 0, test)

    # simple text fail
    test = Test(input="\text", out="text")
    test.prepare("cat", {})
    assert not test.skip
    assert test.get_cmd() == r"echo -e '\text' | cat"
    result = test.run()
    assert not result
    assert result.failed == {"out"}
    assert result == TestResult(["\text\n"], [], 0, test)

    # multiple fails
    test = Test("notafile", stdout="should be nothing", stderr="good file")
    test.prepare("ls", {})
    assert test.get_cmd() == "ls notafile"
    result = test.run()
    assert not result
    assert result.failed == {"exitcode", "stdout", "stderr"}

    # multi-lines
    test = Test(input="one\ntwo", stdout="text")
    test.prepare("cat", {})
    assert test.get_cmd() == dedent(
        """\
            cat <<EOF
            one
            two
            EOF"""
    )
    result = test.run()
    assert not result
    assert result.failed == {"stdout"}
    assert result == TestResult(["one\n", "two\n"], [], 0, test)

    # Unicode text pass
    test = Test(comment="with BOM", input="\uFEFF02 1", out="\uFEFF02 1")
    test.prepare("cat", {})
    assert test.get_cmd().lower() == r"echo -e '\ufeff02 1' | cat"
    result = test.run()
    assert result, result.describe_fail()
    assert result == TestResult(["\uFEFF02 1\n"], [], 0, test)

    # binary I/O pass
    test = Test(input=b"bin\tdata", stdout=b"bin\tdata")
    test.prepare("cat", {})
    assert test.get_cmd() == r"printf 'bin\tdata' | cat | base64 -"
    result = test.run()
    assert result, result.describe_fail()
    assert result == TestResult(b"bin\tdata", [], 0, test)

    # binary I/O fail
    test = Test(input=b"\x02\x08", stdout=b"\x02\x08", exitcode=3)
    test.prepare("cat", {})
    assert test.get_cmd() == r"printf '\x02\x08' | cat | base64 -"
    result = test.run()
    assert result.failed == {"exitcode"}
    assert result == TestResult(b"\x02\x08", [], 0, test)

    # simple skip
    test = Test(input="\text", out="text", skipif="True")
    test.prepare("cat", {})
    with pytest.raises(ValueError, match="test is skipped"):
        test.get_cmd()
    with pytest.raises(ValueError, match="test is skipped"):
        test.run()
    assert test.skip

    # bad tests
    with pytest.raises(ValueError, match="out cannot be used with stdout"):
        Test(stdout="", out="")

    test = Test(input="hi", out="hi")
    with pytest.raises(ValueError, match="test not prepared"):
        test.get_cmd()
    with pytest.raises(ValueError, match="test not prepared"):
        test.run()
    test.prepare("cat", {})
    test.prepare("cat", {})  # twice prepared
    assert test.get_cmd() == r"echo hi | cat"
    result = test.run()
    assert result, result.describe_fail()
    assert result == TestResult(["hi\n"], [], 0, test)


def test_test_tmpdir():
    fd, fname = tempfile.mkstemp(suffix=".txt")
    with os.fdopen(fd, "w") as fp:
        fp.write("stuff\n")

    test = Test("$tmpdir", out=os.path.basename(fname), tmpdir={"copy": [fname]})
    env = {}
    test.prepare("ls", env)
    assert list(env.keys()) == []
    assert test.get_cmd() == dedent(
        f"""\
        mkdir {test.tmpdir.strpth}
        cp {fname} {test.tmpdir.strpth}
        ls {test.tmpdir.strpth}
        rm -rf {test.tmpdir.strpth}"""
    )
    result = test.run()
    assert result, result.describe_fail()
    test.cleanup()
    os.unlink(fname)


def test_test_file():
    prevdir = Path().cwd()
    with tempfile.TemporaryDirectory() as otd:
        os.chdir(otd)

        test = Test(
            out="file.txt",
            file={"name": "file.txt", "content": ""},
        )
        env = {}
        test.prepare("ls", env)
        assert list(env.keys()) == []
        assert test.get_cmd() == dedent(
            """\
            touch file.txt
            ls
            rm file.txt"""
        )
        result = test.run()
        assert result, result.describe_fail()
        test.cleanup()

        test = Test(
            "-1",
            out="file1.txt\nfile2.txt\nfile3_with_bom.txt\nlast file.txt",
            file=[
                {"name": "file1.txt", "content": "one"},
                {"name": "file2.txt", "content": b"two"},
                {"name": "file3_with_bom.txt", "content": "\uFEFF02 1"},
                {"name": "last file.txt", "content": "3 4\n5 6\n"},
            ],
        )
        env = {}
        test.prepare("ls", env)
        assert list(env.keys()) == []
        assert test.get_cmd() == dedent(
            """\
            echo one > file1.txt
            printf 'two' > file2.txt
            echo -e '\\ufeff02 1' > file3_with_bom.txt
            cat << EOF > 'last file.txt'
            3 4
            5 6
            EOF
            ls -1
            rm file1.txt file2.txt file3_with_bom.txt 'last file.txt'"""
        )
        result = test.run()
        assert result, result.describe_fail()
        test.cleanup()
    os.chdir(prevdir)


def test_out_cmds():
    import pytest

    fd, fname = tempfile.mkstemp(suffix=".txt")
    with os.fdopen(fd, "w") as fp:
        fp.write("one\ntwo\nthree\nfour\n")

    test = Test(fname, out_cmds={"sort": None}, out="four\none\nthree\ntwo\n")
    test.prepare("cat", {})
    assert test.get_cmd() == f"cat {fname} 2>&1 | sort"
    result = test.run()
    assert result, result.describe_fail()

    test = Test(fname, out_cmds={"grep": "o"}, stdout="one\ntwo\nfour\n")
    test.prepare("cat", {})
    assert test.get_cmd() == f"cat {fname} | grep o"
    result = test.run()
    assert result, result.describe_fail()

    test = Test(fname, out_cmds={"grep": "none"}, stdout="")
    test.prepare("cat", {})
    assert test.get_cmd() == f"cat {fname} | grep none"
    result = test.run()
    assert result, result.describe_fail()

    test = Test(fname, out_cmds={"grep-v": "o"}, stdout="three")
    test.prepare("cat", {})
    assert test.get_cmd() == f"cat {fname} | grep -v o"
    result = test.run()
    assert result, result.describe_fail()

    test = Test(
        fname, out_cmds={"sub": ["o", "OO"]}, stdout="OOne\ntwOO\nthree\nfOOur\n"
    )
    test.prepare("cat", {})
    assert test.get_cmd() == f"cat {fname} | sed s/o/OO/"
    result = test.run()
    assert result, result.describe_fail()

    test = Test(fname, out_cmds={"head": 3, "sort": None}, stdout="one\nthree\ntwo\n")
    test.prepare("cat", {})
    assert test.get_cmd() == f"cat {fname} | head -n 3 | sort"
    result = test.run()
    assert result, result.describe_fail()

    test = Test(fname, out_cmds={"tail": 1}, stdout="four")
    test.prepare("cat", {})
    assert test.get_cmd() == f"cat {fname} | tail -n 1"
    result = test.run()
    assert result, result.describe_fail()

    # errors
    with pytest.raises(ValueError, match="unsupported grep type"):
        Test(fname, out_cmds={"grep": 1})
    with pytest.raises(ValueError, match="sub should be a list of 2 str"):
        Test(fname, out_cmds={"sub": ["o"]})
    with pytest.raises(ValueError, match="sub should be a list of 2 str"):
        Test(fname, out_cmds={"sub": [1, 2]})
    test = Test(fname, out_cmds={"silly": None})
    test.prepare("cat", {})
    with pytest.raises(NotImplementedError, match="unsupported out cmd: silly=None"):
        test.get_cmd()
    with pytest.raises(NotImplementedError, match="unsupported out cmd: silly=None"):
        test.run()
    os.unlink(fname)
