"""Render the repository's host proxy config for an isolated Docker HTTP test."""

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
source = (ROOT / "proxy.conf").read_text()
# Only environment-specific addresses and paths change; routing/gzip/limits stay.
for old, new in (
    ("127.0.0.1:10001", "backend:8000"),
    ("listen 80;", "listen 8080;"),
    ("/home/ilya/meebible/logs/proxy-access.log", "/dev/stdout"),
    ("/home/ilya/meebible/logs/proxy-error.log", "/dev/stderr"),
):
    if source.count(old) != 1:
        raise ValueError(f"Expected exactly one {old!r}; review proxy test mapping")
    source = source.replace(old, new)
wrapper = """worker_processes 1;
pid /tmp/nginx.pid;
error_log /dev/stderr;
events { worker_connections 1024; }
http {
 client_body_temp_path /tmp/client_body;
 proxy_temp_path /tmp/proxy;
 fastcgi_temp_path /tmp/fastcgi;
 uwsgi_temp_path /tmp/uwsgi;
 scgi_temp_path /tmp/scgi;
"""
output = ROOT / "tests/backend/.local/outer-proxy.conf"
output.parent.mkdir(exist_ok=True)
output.write_text(wrapper + source + "\n}\n")
print(output)
