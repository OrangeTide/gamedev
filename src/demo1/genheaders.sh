for SHADER in *.glsl ; do
  sokol-shdc --input "$SHADER" --output "$SHADER".h --slang glsl330 --genver 1 --errfmt gcc --format sokol --bytecode --module demo1 --reflection
  # Enscripten WebGL2 - EXPERIMENTAL
  # sokol-shdc --input "$SHADER" --output "$SHADER".h --slang glsl300es --genver 1 --errfmt gcc --format sokol --bytecode --module demo1 --reflection
done
