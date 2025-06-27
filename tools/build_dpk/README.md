# build_dpk.sh 1.1

# Description
build_dpk.sh is an application packaging tool. It requires an input application configuration json file(app.json), and generates a .dpk application installation package.

the sample app.json: samples/sample_c/platform/linux/manifold3/app_json/app.json
app.json filling specification:
please refer to samples/sample_c/platform/linux/manifold3/app_json/README.md

# Environment Dependencies
Python 3 environment is required;
dpkg tool is required.

# Usage
    build_dpk.sh -i input app.json file
        [-o output dpk file path]
        [-h help]"
    Options:
    "  -h        Show this help message and exit"
    "  -i        Specify input app.json file"
    "  -o        Specify build dpk file path"

    Examples:
      build_dpk.sh -i ../../samples/sample_c/platform/linux/manifold3/app_json/app.json           Generate .dpk application installation package under the current path
      build_dpk.sh -i ../../samples/sample_c/platform/linux/manifold3/app_json/app.json -o dpk    Generate .dpk application installation package under the specified path(dpk)
