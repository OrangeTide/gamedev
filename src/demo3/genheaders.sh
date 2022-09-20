for SHADER in *.glsl ; do
  sokol-shdc --input "$SHADER" --output "$SHADER".h --slang glsl330 --genver 1 --errfmt gcc --format sokol --bytecode --module demo3 --reflection
done
