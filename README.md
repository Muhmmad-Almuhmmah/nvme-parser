# NVME parser
![License](https://img.shields.io/github/license/R0rt1z2/nvme-parser)
![GitHub release (latest by date including pre-releases)](https://img.shields.io/github/v/release/R0rt1z2/nvme-parser?include_prereleases)
![GitHub Issues](https://img.shields.io/bitbucket/issues-raw/R0rt1z2/AutomatedRoot?color=red)

Read, write, parse and dump your nvme block through adb!

## ⚠️ Warning
* Use this tool at your own risk. I am not responsible for bricked devices. Please backup NVME before modifying any value.
* Avoid modifying encrypted values ​​(FBLOCK, WVLOCK, FRPLOCK, etc). These values can be only rewritten with their SHA256 hash.
* When reporting an issue, make sure to hide sensitive information (such as S/N, MACs, etc).

## 📜 Requirements
* Huawei phone.
* Root shell (terminal/adb).
* The binary itself.

## ✔️ Features
* Read NVME values => `./nvme -r VALUE_NAME`
* Write NVME values => `./nvme -w VALUE_NAME NEW_VALUE`
* Dump the NVME block (dd) => `./nvme -b [optional] dump_path`
* Restore NVME dump (dd) => `./nvme -e [optional] backup_path`

## 💻 Building the tool
* If you want to compile this tool, you will need the android NDK kit.
* Once you installed NDK and you set up the exports, run `make` to build the tool.

## ℹ️ License
* This tool is licensed under the GNU (v3) General Public License. See `LICENSE` for more details.
