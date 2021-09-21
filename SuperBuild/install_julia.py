# without this I get SSL error when `wget` [SSL: CERTIFICATE_VERIFY_FAILED] on Ubuntu 20.04
import ssl
ssl._create_default_https_context = ssl._create_unverified_context

import sys
import getopt

argv = sys.argv[1:]

try:
    opts, args = getopt.getopt(argv, "v:p:", ["version =", "prefix ="])
  
except:
    print("Error")

julia_version = None
julia_install_dir = None

for opt, arg in opts:
    if opt in ['-v', '--version']:
        julia_version = arg
    elif opt in ['-p', '--prefix']:
        julia_install_dir = arg

if julia_version is None:
    raise ValueError('julia_version is not specified!')

if julia_install_dir is None:
    raise ValueError('julia_install_dir is not specified!')

import subprocess
subprocess.run(['jill', 'install', julia_version, '--confirm', '--install_dir', julia_install_dir])