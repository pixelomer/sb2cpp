#!/usr/bin/env bash

bin2c() {
	file_size="$(wc -c < "$2" | awk '{print $1}')"

	echo -en "uint8_t $1[] =\n"
	hexdump -v -e '16/1 "_x%02X" "\n"' < "$2" | sed 's/_/\\/g; s/\\x  //g; s/.*/    "&"/'
	echo -en '    ;\n'
}

if [ -z "$1" ]; then
	echo "Usage: $0 <DefaultFontName> [files...]"
	exit 1
fi

cat <<'EOF'
#ifndef SMALLBASIC_PACKED_ASSETS
#define SMALLBASIC_PACKED_ASSETS

#include <stdint.h>
#include <std/platform/Renderer.hpp>

EOF

out=""
font_name=""
idx=0
for arg in "$@"; do
	if [ -z "${font_name}" ]; then
		font_name="${arg}"
		continue
	fi
	idx=$((idx+1))
	echo "// ${arg}"
	bin2c "_fileres$((idx-1))" "${arg}"
done

cat <<'EOF'

void SmallBasic_RegisterPackedAssets() {
EOF

total="${idx}"
idx=1
while [ "${idx}" -le "${total}" ]; do
	echo "    SmallBasic::Platform::Renderer::RegisterResource(L\"${!idx}\","
	echo "        _fileres$((idx-1)), sizeof(_fileres$((idx-1))));"
	idx=$((idx+1))
done

echo "    SmallBasic::Platform::Renderer::SetDefaultFontName(L\"${font_name}\");"

cat <<'EOF'
}

#endif
EOF

