import subprocess

revision = subprocess.check_output(["git", "rev-parse", "HEAD"]).strip()
print("-DPIO_GIT_REV=\\\"%s\\\"" % revision)