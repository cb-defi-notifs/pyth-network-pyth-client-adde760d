#!/usr/bin/env bash
#
# Build given bpf makefile dir (./program by default):
#   ~/pyth-client$ ./scripts/build-bpf.sh
#   ~/pyth-client/program$ ../scripts/build-bpf.sh .
#   ~/$ ./pyth-client/scripts/build-bpf.sh ./serum-pyth/program
#

set -eu


#find the makefile in pyth-client
#ASSUMES THAT there is only one makefile there 
C_DIR="$( cd "${1:-.}" && find $(pwd) | grep makefile)"
C_DIR=$(dirname $C_DIR)

#finds Cargo.toml in pyth-client
#ASSUMES THAT there is only one Cargo.toml there 
RUST_DIR="$( cd "${1:-.}" && find $(pwd) | grep Cargo.toml )"
RUST_DIR=$(dirname $RUST_DIR)

if ! which cargo 2> /dev/null
then
  # shellcheck disable=SC1090
  source "${CARGO_HOME:-$HOME/.cargo}/env"
fi


set -x


cd "${C_DIR}"
export V="${V:-1}"
make clean
make  "${@:2}"
make cpyth
rm ./target/*-keypair.json



cd "${RUST_DIR}"
cargo clean
cargo build-bpf
sha256sum ./target/**/*.so
rm ./target/**/*-keypair.json
rm -r ../target || true
mv ./target ..




