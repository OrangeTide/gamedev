for SHADER in *.glsl ; do
  sokol-shdc --input "$SHADER" --output "$SHADER".h --slang glsl330 --genver 5 --errfmt gcc --format sokol --bytecode --module demo1 --reflection
done
