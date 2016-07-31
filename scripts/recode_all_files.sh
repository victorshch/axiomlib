#!/bin/sh

find . -path "./.git" -prune -o -type f -regex "\(.*\.c\)\|\(.*\.cpp\)\|\(.*\.h\)\|\(.*\.cxx\)\|\(.*\.hpp\)\|\(.*\.config\)\|\(.*\.pro\)\|\(.*\.pri\)" -print | while read f; do
        echo "Recoding file $f"
        mv -i "$f" "$f.recode.$$"
        iconv -f koi8-r -t utf-8 < "$f.recode.$$" > "$f"
        rm -f "$f.recode.$$"
done