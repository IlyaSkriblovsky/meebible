#!/usr/bin/env python3
"""Prepare and smoke-check the local PHP reference. Python standard library only."""

import argparse
import gzip
import hashlib
import http.client
import json
import shutil
import sqlite3
import subprocess
import sys
import tarfile
import time
import xml.etree.ElementTree as ET
from datetime import datetime, timezone
from pathlib import Path, PurePosixPath


HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
LOCAL = HERE / ".local"
SNAPSHOT = LOCAL / "snapshot"
SOURCE_REVISION = "ebfbcd85cd01cf8c62f0fb59cf7533f5befc85c3"
COMPOSE = ["docker", "compose", "-f", str(HERE / "compose.yml")]


def run(*args):
    return subprocess.check_output(args, cwd=ROOT, text=True).strip()


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def digest(path):
    with path.open("rb") as stream:
        return hashlib.file_digest(stream, "sha256").hexdigest()


def inventory():
    return {
        path.relative_to(SNAPSHOT).as_posix(): digest(path)
        for path in sorted((SNAPSHOT / "www").rglob("*"))
        if path.is_file() and not path.is_symlink()
    }


def content_manifest(files):
    return "".join(
        f"{sha}  {path}\n" for path, sha in files.items()
        if path.startswith("www/content/") and path.endswith(".sqlite")
    )


def prepare():
    require(not SNAPSHOT.exists(), "Snapshot already exists; use check to verify it.")
    SNAPSHOT.mkdir(parents=True)
    # Export only committed www files, never local user databases or compile files.
    with subprocess.Popen(
        ["git", "archive", SOURCE_REVISION, "www"], cwd=ROOT, stdout=subprocess.PIPE
    ) as process:
        with tarfile.open(fileobj=process.stdout, mode="r|") as archive:
            for entry in archive:
                parts = PurePosixPath(entry.name).parts
                require(parts[0] == "www" and ".." not in parts, "Unsafe archive path")
                if len(parts) >= 2 and parts[1] in {
                    "startup.sqlite", "feedback.sqlite", "compile", "cache"
                }:
                    continue
                target = SNAPSHOT.joinpath(*parts)
                if entry.isdir():
                    target.mkdir(parents=True, exist_ok=True)
                else:
                    require(entry.isfile(), "Only regular source files are allowed")
                    target.parent.mkdir(parents=True, exist_ok=True)
                    with archive.extractfile(entry) as source, target.open("wb") as dest:
                        shutil.copyfileobj(source, dest)
                    target.chmod(0o444)
        require(process.wait() == 0, "git archive failed")

    for name in ("startup", "feedback"):
        (SNAPSHOT / "www" / f"{name}.sqlite").symlink_to(f"/data/{name}.sqlite")
    for name in ("compile", "cache"):
        (SNAPSHOT / "www" / name).mkdir()
    files = inventory()
    expected = HERE / "content.sha256"
    require(expected.read_text() == content_manifest(files), "Content checksum mismatch")
    for path in sorted(SNAPSHOT.rglob("*"), reverse=True):
        if path.is_dir():
            path.chmod(0o555)
    manifest = {"source_revision": SOURCE_REVISION, "files": files}
    (LOCAL / "snapshot.json").write_text(json.dumps(manifest, indent=2) + "\n")
    print(f"Prepared {len(files)} files from {SOURCE_REVISION}")


def php(code):
    return run(*COMPOSE, "exec", "-T", "php", "php", "-r", code)


def user_counts():
    return json.loads(php('''
        $out = array();
        foreach (array("startup", "feedback") as $name) {
            $out[$name] = 0;
            if (file_exists("/data/$name.sqlite")) {
                $db = new SQLite3("/data/$name.sqlite", SQLITE3_OPEN_READONLY);
                $out[$name] = $db->querySingle("SELECT count(*) FROM $name");
            }
        }
        echo json_encode($out);
    '''))


def request(path, method="GET", body=None, encoding="identity", timeout=10):
    # A fixed loopback target prevents running writes against production by mistake.
    connection = http.client.HTTPConnection("127.0.0.1", 10056, timeout=timeout)
    headers = {"Host": "meebible.org", "Accept-Encoding": encoding}
    if body is not None:
        headers["Content-Type"] = "application/x-www-form-urlencoded"
    try:
        connection.request(method, path, body=body, headers=headers)
        response = connection.getresponse()
        data = response.read()
        wire_headers = response.getheaders()
        values = {key.lower(): value for key, value in wire_headers}
        if "content-length" in values and method != "HEAD":
            require(int(values["content-length"]) == len(data), f"Wrong length: {path}")
        decoded = gzip.decompress(data) if values.get("content-encoding") == "gzip" else data
        return response.status, values, decoded, {
            "method": method, "path": path, "accept_encoding": encoding,
            "status": response.status, "headers": wire_headers,
            "wire_bytes": len(data), "decoded_bytes": len(decoded),
            "body_sha256": hashlib.sha256(decoded).hexdigest(),
        }
    finally:
        connection.close()


def check():
    manifest = json.loads((LOCAL / "snapshot.json").read_text())
    require(manifest["source_revision"] == SOURCE_REVISION, "Unexpected source revision")
    require(inventory() == manifest["files"], "Snapshot changed before check")
    require(content_manifest(manifest["files"]) == (HERE / "content.sha256").read_text(),
            "Content differs from the pinned snapshot")
    for name in ("startup", "feedback"):
        path = SNAPSHOT / "www" / f"{name}.sqlite"
        require(path.is_symlink() and str(path.readlink()) == f"/data/{name}.sqlite",
                "User database must link to temporary data")
    report = {
        "checked_at_utc": datetime.now(timezone.utc).isoformat(),
        "source_revision": manifest["source_revision"],
        "snapshot_files": len(manifest["files"]),
        "content_files": len(content_manifest(manifest["files"]).splitlines()),
        "content_manifest_sha256": digest(HERE / "content.sha256"),
        "docker_version": run("docker", "version", "--format", "{{.Server.Version}}"),
        "compose_version": run("docker", "compose", "version", "--short"),
        "python_version": sys.version.split()[0],
        "config_sha256": {name: digest(HERE / name) for name in
                          ("compose.yml", "fpm.conf", "php.ini", "nginx.conf", "reference.py")},
        "responses": [],
    }
    containers = {}
    for service, uid in (("php", "33:33"), ("nginx", "101:101")):
        container_id = run(*COMPOSE, "ps", "-q", service)
        require(bool(container_id), f"{service} is not running")
        info = json.loads(run("docker", "inspect", container_id))[0]
        host = info["HostConfig"]
        require(host["ReadonlyRootfs"] and info["Config"]["User"] == uid,
                f"{service}: expected read-only root and non-root user")
        require(host["CapDrop"] == ["ALL"] and
                "no-new-privileges:true" in host["SecurityOpt"], "Missing process isolation")
        require(host["PidsLimit"] == 64 and host["Memory"] == 256 * 1024 * 1024,
                "Missing resource limits")
        binds = [mount for mount in info["Mounts"] if mount["Type"] == "bind"]
        require(all(not mount["RW"] for mount in binds), "Writable host bind mount")
        require({mount["Destination"] for mount in binds} ==
                ({"/www", "/reference/fpm.conf", "/usr/local/etc/php/conf.d/zz-reference.ini"}
                 if service == "php" else
                 {"/www", "/etc/nginx/nginx.conf"}), "Unexpected host mount")
        networks = info["NetworkSettings"]["Networks"]
        network_settings = {}
        for name in networks:
            network = json.loads(run("docker", "network", "inspect", name))[0]
            network_settings[network["Labels"]["com.docker.compose.network"]] = network["Internal"]
        require(network_settings == ({"reference": True} if service == "php" else
                {"reference": True, "loopback": False}), "Unexpected network access")
        bindings = host["PortBindings"] or {}
        require(bindings == ({} if service == "php" else
                {"8080/tcp": [{"HostIp": "127.0.0.1", "HostPort": "10056"}]}),
                "Unexpected published ports")
        actual_ports = info["NetworkSettings"]["Ports"]
        require(all(not value for value in actual_ports.values()) if service == "php" else
                actual_ports.get("8080/tcp") == bindings["8080/tcp"],
                "Actual published ports differ from requested bindings")
        image = json.loads(run("docker", "image", "inspect", "--platform", "linux/amd64",
                               info["Config"]["Image"]))[0]
        require(image["Os"] == "linux" and image["Architecture"] == "amd64",
                "Unexpected image platform")
        containers[service] = {
            "image": info["Config"]["Image"], "image_id": image["Id"],
            "platform": "linux/amd64", "user": uid, "read_only_root": True,
            "bind_mounts_read_only": True, "networks": network_settings,
            "port_bindings": bindings,
        }
    report["containers"] = containers
    settings = json.loads(php('''
        echo json_encode(array("version" => PHP_VERSION, "cwd" => getcwd(),
            "extensions" => get_loaded_extensions(), "ini" => php_ini_loaded_file(),
            "scanned_ini" => php_ini_scanned_files(),
            "sqlite" => SQLite3::version(), "libxml" => LIBXML_DOTTED_VERSION));
    '''))
    require(settings["version"] == "5.6.40" and settings["cwd"] == "/www", "Wrong PHP runtime")
    require({"sqlite3", "dom", "mbstring"} <= set(settings["extensions"]), "Missing extensions")
    report["php"] = settings
    # FPM phpinfo reports SAPI-specific defaults; CLI phpinfo alone is insufficient.
    fpm_info = run(*COMPOSE, "exec", "-T", "php", "php-fpm", "-i")
    (LOCAL / "fpm-info.txt").write_text(fpm_info + "\n")
    keys = {"Server API", "Loaded Configuration File", "Scan this dir for additional .ini files",
            "Additional .ini files parsed", "default_charset", "display_errors", "html_errors",
            "error_reporting", "log_errors", "request_order", "variables_order", "output_buffering",
            "zlib.output_compression", "post_max_size", "max_input_vars", "date.timezone"}
    report["fpm_defaults"] = dict(
        line.split(" => ", 1) for line in fpm_info.splitlines()
        if line.split(" => ", 1)[0] in keys
    )
    require(report["fpm_defaults"].get("date.timezone") == "UTC => UTC", "UTC setting missing")
    isolation = json.loads(php('''
        echo json_encode(array(
            "code_write_blocked" => @file_put_contents("/www/reference-probe", "x") === false,
            "content_write_blocked" => @file_put_contents("/www/content/reference-probe", "x") === false,
            "new_content_db_blocked" => !is_writable("/www/content"),
            "compile_writable" => is_writable("/www/compile"),
            "data_writable" => is_writable("/data"),
            "routes" => file_get_contents("/proc/net/route")));
    '''))
    require(all(isolation[key] for key in isolation if key != "routes"), "Write isolation failed")
    require(not any(line.split()[1] == "00000000"
                    for line in isolation["routes"].splitlines()[1:] if line.split()),
            "Container has a default IPv4 route")
    report["filesystem_and_routing"] = isolation
    before = user_counts()

    def get(path, status=200, method="GET", body=None, encoding="identity"):
        actual, headers, data, evidence = request(path, method, body, encoding)
        require(actual == status, f"{method} {path}: expected {status}, got {actual}")
        report["responses"].append(evidence)
        return headers, data

    # Readiness is a safe GET and is bounded even if FPM fails to start.
    deadline = time.monotonic() + 30
    while time.monotonic() < deadline:
        try:
            status, _, _, _ = request("/meta", timeout=2)
            if status == 200:
                break
        except (OSError, http.client.HTTPException):
            pass
        time.sleep(1)
    else:
        raise RuntimeError("Reference did not become ready within 30 seconds")

    _, meta = get("/meta")
    require(ET.fromstring(meta).tag == "metainfo", "Invalid catalog")
    headers, compressed_meta = get("/meta", encoding="gzip")
    require(headers.get("content-encoding") == "gzip" and compressed_meta == meta,
            "gzip does not preserve metadata")
    _, translation = get("/translation?trans=kjv&lang=e")
    require(ET.fromstring(translation).tag == "translation", "Invalid translation")
    for trans, lang in (("kjv", "e"), ("rus", "u"), ("ara", "a"), ("nwt", "e")):
        _, chapter = get(f"/chapter?trans={trans}&lang={lang}&book=ge&chapter=1")
        path = SNAPSHOT / "www/content" / (
            f"nwt/nwt_{lang}.sqlite" if trans == "nwt" else f"{trans}.sqlite")
        with sqlite3.connect(f"{path.as_uri()}?mode=ro", uri=True) as db:
            html = db.execute("SELECT html FROM html WHERE langCode=? AND bookCode='ge' "
                              "AND chapterNo=1", (lang,)).fetchone()[0]
        require(chapter == html.encode("utf-8"), f"Chapter bytes changed for {trans}/{lang}")
    for path in ("/", "/privacy", "/donate", "/donate-mobile", "/t/kjv/e/ge/1/1-3,5"):
        _, page = get(path)
        require(page.lstrip().startswith(b"<!DOCTYPE html>"), f"Invalid HTML page: {path}")
    for path in ("/static/css/style.css", "/favicon.ico"):
        get(path)
    _, missing = get("/reference-missing", status=404)
    require(missing.lstrip().startswith(b"<!DOCTYPE html>"), "Invalid 404 page")
    _, error = get("/chapter?trans=kjv", status=400)
    require(error == b"Not all parameters specified", "Wrong chapter error")
    headers, _ = get("/t/kjv/e/ge/1/3,1,2", status=301)
    require(headers.get("location") == "/t/kjv/e/ge/1/1-3", "Wrong passage redirect")
    _, head = get("/meta", method="HEAD")
    require(not head, "HEAD has a body")
    for path, body in (
        ("/startup", b"os=symbian&device_id=reference-only&lang=e&trans=kjv"),
        ("/feedback", b"name=Reference%20check&email=user+tag@example.org&message=Local%20only"),
    ):
        _, data = get(path, method="POST", body=body)
        require(data == b"ok", f"Unexpected POST body: {path}")
    after = user_counts()
    require(all(after[name] == before[name] + 1 for name in before), "POST did not commit one row")
    stored = json.loads(php('''
        $out = array();
        foreach (array("startup", "feedback") as $name) {
            $db = new SQLite3("/data/$name.sqlite", SQLITE3_OPEN_READONLY);
            $out[$name] = $db->querySingle("SELECT * FROM $name ORDER BY id DESC LIMIT 1", true);
        }
        echo json_encode($out);
    '''))
    require(stored["startup"]["device_id"] == "reference-only" and
            stored["feedback"]["email"] == "user tag@example.org", "Unexpected stored form values")
    report["temporary_writes"] = {"before": before, "after": after, "stored": stored}
    require(inventory() == manifest["files"], "Snapshot changed during check")
    report["snapshot_unchanged"] = True
    report["result"] = "pass"
    output = LOCAL / "verification.json"
    output.write_text(json.dumps(report, indent=2) + "\n")
    print(f"PASS: {len(report['responses'])} HTTP checks, committed POST rows, isolation and checksums")
    print(f"Report: {output.relative_to(ROOT)}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("command", choices=("prepare", "check"))
    args = parser.parse_args()
    try:
        {"prepare": prepare, "check": check}[args.command]()
    except (RuntimeError, OSError, subprocess.CalledProcessError) as error:
        parser.exit(1, f"Error: {error}\n")
