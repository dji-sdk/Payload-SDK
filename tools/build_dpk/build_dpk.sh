#!/bin/bash

is_ai_rendering="false"
input_json_file=""
output_dpk_path="./"

# help
show_help()
{
    echo "Usage: build_dpk.sh -i input app.json file
        [-o output dpk file path]
        [-h help]"
    echo ""
    echo "Options:"
    echo "  -h        Show this help message and exit"
    echo "  -i        Specify input app.json file"
    echo "  -o        Specify build dpk file path"
    echo ""
}

# prase parameters
while getopts ":i:o:h" opt; do
    case ${opt} in
        h)
            show_help
            exit 0
            ;;
        i)
            input_json_file=$OPTARG
            ;;
        o)
            output_dpk_path=$OPTARG
            ;;
        \?)
            echo "Invalid option: -$OPTARG" >&1
            show_help
            exit 1
            ;;
        :)
            echo "Option -$OPTARG requires an argument." >&1
            show_help
            exit 1
            ;;
    esac
done

# check input json file
if [ -n "$input_json_file" ]; then
    echo "-i option is set with value: $input_json_file"
else
    show_help
    exit 1
fi

if [ ! -f "$input_json_file" ]; then
    echo "Input file $input_json_file does not exist."
    exit 1
fi

if [[ "$input_json_file" != *.json ]]; then
    echo "Input file $input_json_file is not a .json file."
    exit 1
fi

echo "Input json file: $input_json_file"

input_json_dir=$(dirname "$input_json_file")

# check output path
if [ -n "$output_dpk_path" ]; then
    echo "Output dpk file path: $output_dpk_path"
else
    echo "Use Default Output dpk path"
fi

# prase and check id field
id=$(python3 -c "import json; print(json.load(open('$input_json_file'))['user_app_id'])")
if [ "$id" == "" ]; then
    echo "Error, user_app_id field not exist or null"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
fi

if [ $(printf "%s" "$id" | wc -c) -gt 16 ]; then
    echo "Error, user_app_id field byte count:$(printf "%s" "$id" | wc -c) over 16"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
fi

# prase and check version field
version_regex='^[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+$'
version=$(python3 -c "import json; print(json.load(open('$input_json_file'))['firmware_version'])")
if [ "$version" == "" ]; then
    echo "Error, firmware_version field not exist or null"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
elif [[ ! $version =~ $version_regex ]]; then
    echo "Error, firmware_version field not comply with the xx.xx.xx.xx specification"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
fi

# prase and check platform field
platform=$(python3 -c "import json; print(json.load(open('$input_json_file'))['platform'])")
if [ "$platform" == "" ]; then
    echo "Error, platform field not exist or null"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
elif [[ "$platform" != "manifold3" ]]; then
    echo "Error, platform field is not manifold3"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
fi

# prase and check name_cn field
name_cn=$(python3 -c "import json; print(json.load(open('$input_json_file'))['name']['name_cn'])")
if [ "$name_cn" == "" ]; then
    echo "Error, name_cn field not exist or null"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
elif [ $(printf "%s" "$name_cn" | wc -c) -gt 47 ]; then
    echo "Error, name_cn field byte count:$(printf "%s" "$name_cn" | wc -c) over 47"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
fi

# prase and check name_en field
regex='^[a-zA-Z0-9.-]+$'
name_en=$(python3 -c "import json; print(json.load(open('$input_json_file'))['name']['name_en'])")
if [ "$name_en" == "" ]; then
    echo "Error, name_en field not exist or null"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
elif [ $(printf "%s" "$name_en" | wc -c) -gt 32 ]; then
    echo "Error, name_en field byte count:$(printf "%s" "$name_en" | wc -c) over 32"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
elif [[ ! $name_en =~ $regex ]]; then
    echo "Error, name_en field not comply with the specification"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
fi

# prase and check name_jp field
name_jp=$(python3 -c "import json; print(json.load(open('$input_json_file'))['name']['name_jp'])")
if [ "$name_jp" == "" ]; then
    echo "Error, name_jp field not exist or null"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
elif [ $(printf "%s" "$name_jp" | wc -c) -gt 47 ]; then
    echo "Error, name_jp field byte count:$(printf "%s" "$name_jp" | wc -c) over 47"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
fi

# prase and check name_fr field
name_fr=$(python3 -c "import json; print(json.load(open('$input_json_file'))['name']['name_fr'])")
if [ "$name_fr" == "" ]; then
    echo "Error, name_fr field not exist or null"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
elif [ $(printf "%s" "$name_fr" | wc -c) -gt 47 ]; then
    echo "Error, name_fr field byte count:$(printf "%s" "$name_fr" | wc -c) over 47"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
fi

# prase and check description_cn field
desc_cn=$(python3 -c "
import json
import sys

with open('$input_json_file', 'r') as file:
    data = json.load(file)
print('\n '.join(data['description']['description_cn']))
")
if [ "$desc_cn" == "" ]; then
    echo "Error, description_cn field not exist or null"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
elif [ $(printf "%s" "$desc_cn" | wc -c) -gt 367 ]; then
    echo "Error, desc_cn field byte count:$(printf "%s" "$desc_cn" | wc -c) over 367"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
fi

# prase and check description_en field
desc_en=$(python3 -c "
import json
import sys

with open('$input_json_file', 'r') as file:
    data = json.load(file)
print('\n '.join(data['description']['description_en']))
")
if [ "$desc_en" == "" ]; then
    echo "Error, description_en field not exist or null"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
elif [ $(printf "%s" "$desc_en" | wc -c) -gt 367 ]; then
    echo "Error, desc_en field byte count:$(printf "%s" "$desc_en" | wc -c) over 367"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
fi

# prase and check description_jp field
desc_jp=$(python3 -c "
import json
import sys

with open('$input_json_file', 'r') as file:
    data = json.load(file)
print('\n '.join(data['description']['description_jp']))
")
if [ "$desc_jp" == "" ]; then
    echo "Error, description_jp field not exist or null"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
elif [ $(printf "%s" "$desc_jp" | wc -c) -gt 367 ]; then
    echo "Error, desc_jp field byte count:$(printf "%s" "$desc_jp" | wc -c) over 367"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
fi

# prase and check description_fr field
desc_fr=$(python3 -c "
import json
import sys

with open('$input_json_file', 'r') as file:
    data = json.load(file)
print('\n '.join(data['description']['description_fr']))
")
if [ "$desc_fr" == "" ]; then
    echo "Error, description_fr field not exist or null"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
elif [ $(printf "%s" "$desc_fr" | wc -c) -gt 367 ]; then
    echo "Error, desc_fr field byte count:$(printf "%s" "$desc_fr" | wc -c) over 367"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
fi

# prase and check maintainer_cn field
maintainer_cn=$(python3 -c "import json; print(json.load(open('$input_json_file'))['maintainer']['maintainer_cn'])")
if [ "$maintainer_cn" == "" ]; then
    echo "Error, maintainer_cn field not exist or null"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
elif [ $(printf "%s" "$maintainer_cn" | wc -c) -gt 127 ]; then
    echo "Error, maintainer_cn field byte count:$(printf "%s" "$maintainer_cn" | wc -c) over 127"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
fi

# prase and check maintainer_en field
maintainer_en=$(python3 -c "import json; print(json.load(open('$input_json_file'))['maintainer']['maintainer_en'])")
if [ "$maintainer_en" == "" ]; then
    echo "Error, maintainer_en field not exist or null"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
elif [ $(printf "%s" "$maintainer_en" | wc -c) -gt 127 ]; then
    echo "Error, maintainer_en field byte count:$(printf "%s" "$maintainer_en" | wc -c) over 127"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
fi

# prase and check maintainer_jp field
maintainer_jp=$(python3 -c "import json; print(json.load(open('$input_json_file'))['maintainer']['maintainer_jp'])")
if [ "$maintainer_jp" == "" ]; then
    echo "Error, maintainer_jp field not exist or null"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
elif [ $(printf "%s" "$maintainer_jp" | wc -c) -gt 127 ]; then
    echo "Error, maintainer_jp field byte count:$(printf "%s" "$maintainer_jp" | wc -c) over 127"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
fi

# prase and check maintainer_fr field
maintainer_fr=$(python3 -c "import json; print(json.load(open('$input_json_file'))['maintainer']['maintainer_fr'])")
if [ "$maintainer_fr" == "" ]; then
    echo "Error, maintainer_fr field not exist or null"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
elif [ $(printf "%s" "$maintainer_fr" | wc -c) -gt 127 ]; then
    echo "Error, maintainer_fr field byte count:$(printf "%s" "$maintainer_fr" | wc -c) over 127"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
fi

# prase and check ver_min field
ver_min=$(python3 -c "import json; print(json.load(open('$input_json_file'))['ver_min'])")
if [ "$ver_min" == "" ]; then
    echo "Error, ver_min field not exist or null"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
elif [[ ! $ver_min =~ $version_regex ]]; then
    echo "Error, ver_min field not comply with the xx.xx.xx.xx specification"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
fi

# prase and check ver_maxfield
ver_max=$(python3 -c "import json; print(json.load(open('$input_json_file'))['ver_max'])")
if [ "$ver_max" == "" ]; then
    echo "Error, ver_max field not exist or null"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
elif [[ ! $ver_max =~ $version_regex ]]; then
    echo "Error, ver_max field not comply with the xx.xx.xx.xx specification"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
fi

# prase and check bin field
bin_file=$(python3 -c "import json; print(json.load(open('$input_json_file'))['bin'])")
if [ "$bin_file" == "" ]; then
    echo "Error, bin field not exist or null"
    echo "Please check app.json"
    echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
    exit 1
else
    bin_source_file="$input_json_dir/$bin_file"
    if [ ! -f "$bin_source_file" ]; then
        echo "Error, bin field $bin_file not exist"
        echo "Please check app.json"
        echo "Document Specification: samples/sample_c/platform/linux/manifold3/app_json/README.md"
        exit 1
    fi
fi

# prase and check userconfig field
mapfile -t userconfig_files < <(python3 - <<END
import json
with open('$input_json_file') as f:
    data = json.load(f)
for value in data['userconfig']:
    print(value)
END
)

# prase and check is_ai_rendering field
is_ai_rendering=$(python3 -c "import json; print(json.load(open('$input_json_file'))['is_ai_rendering'])")
if [ "$is_ai_rendering" == "" ]; then
    echo "Warning, is_ai_rendering field not exist or null, use Default value: true"
    is_ai_rendering="true"
elif [[ "$is_ai_rendering" != "false" && "$is_ai_rendering" != "true" ]]; then
    echo "Warning, is_ai_rendering field not true and not false, use Default value: true"
    is_ai_rendering="true"
fi

# generate package name
package_name="${name_en}_v${version}"

# generate package path
package_path=$output_dpk_path/$package_name
if ! rm -rf "$package_path"; then
    echo "Failed to delete $package_path"
    exit 1
fi
if ! rm -rf "$package_path".dpk; then
    echo "Failed to delete $package_path".dpk
    exit 1
fi

if ! mkdir -p "$package_path"; then
    echo "Failed to create $package_path"
    rm -rf "$package_path"
    exit 1
fi

if ! mkdir "$package_path"/DEBIAN; then
    echo "Failed to create $package_path/DEBIAN"
    rm -rf "$package_path"
    exit 1
fi
if ! touch "$package_path"/DEBIAN/control; then
    echo "Failed to create $package_path/DEBIAN/control"
    rm -rf "$package_path"
    exit 1
fi

bin_name=$(basename "$bin_file")

cat << EOF > "$package_path"/DEBIAN/control
Id: $id
Package: $name_en
Format_Version: 3
Is_Ai_Rendering: $is_ai_rendering
Name_CN: $name_cn
Name_EN: $name_en
Name_JP: $name_jp
Name_FR: $name_fr
Description: $desc_en
Description_CN: $desc_cn
Description_EN: $desc_en
Description_JP: $desc_jp
Description_FR: $desc_fr
Version: $version
Platform: $platform
Maintainer: $maintainer_en
Maintainer_CN: $maintainer_cn
Maintainer_EN: $maintainer_en
Maintainer_JP: $maintainer_jp
Maintainer_FR: $maintainer_fr
Architecture: all
Binary: bin/$bin_name
Ver_Min: $ver_min
Ver_Max: $ver_max
EOF

bin_dest_path="$package_path/bin"
userconfig_dest_path="$package_path"
if ! mkdir "$bin_dest_path"; then
    echo "Failed to create $bin_dest_path dir"
    rm -rf "$package_path"
    exit 1
fi

if ! cp "$bin_source_file" "$bin_dest_path"; then
    echo "Failed to copy $bin_source_file to $bin_dest_path"
    rm -rf "$package_path"
    exit 1
fi

chmod a+x $bin_dest_path/$bin_name
if [[ $? -ne 0 ]]; then
    echo "Failed to add $bin_dest_path/$bin_name excutable permission"
    rm -rf "$package_path"
    exit 1
fi

for userconfig_file in "${userconfig_files[@]}"; do
    userconfig_source_file="$input_json_dir/$userconfig_file"
    cp -r "$userconfig_source_file" "$userconfig_dest_path"
    if [[ $? -ne 0 ]]; then
        echo "Failed to copy $userconfig_file to $userconfig_dest_path"
        rm -rf "$package_path"
        exit 1
    fi
done

data_path="$package_path/data"
log_path="$package_path/data/logs"
media_path="$package_path/data/media"
if ! mkdir "$data_path"; then
    echo "Failed to create $data_path dir"
    rm -rf "$package_path"
    exit 1
fi
if ! mkdir "$log_path"; then
    echo "Failed to create $log_path dir"
    rm -rf "$package_path"
    exit 1
fi
if ! mkdir "$media_path"; then
    echo "Failed to create $media_path dir"
    rm -rf "$package_path"
    exit 1
fi

# generate .dpk
old_deb_file="$package_path.deb"
new_dpk_file="$package_path.dpk"
dpkg-deb --build "$package_path" > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "Failed to create $old_deb_file"
    rm -rf "$package_path"
    rm "$old_deb_file"
    exit 1
fi
if ! mv "$old_deb_file" "$new_dpk_file"; then
    echo "Failed to create $new_dpk_file"
    rm -rf "$package_path"
    rm "$old_deb_file"
    rm "$new_dpk_file"
    exit 1
fi

if ! rm -rf "$package_path"; then
    echo "Failed to delete $package_path"
    exit 1
fi

echo "----------------------------"
echo "Successed to create $new_dpk_file!!!"

exit 0
