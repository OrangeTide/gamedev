#pragma once
/*
    #version:1# (machine generated, don't edit!)

    Generated by sokol-shdc (https://github.com/floooh/sokol-tools)

    Cmdline: sokol-shdc --input palette.glsl --output palette.glsl.webgl.h --slang glsl300es:glsl100 --genver 1 --errfmt gcc --format sokol --bytecode --module demo3 --reflection

    Overview:

        Shader program 'palette':
            Get shader desc: demo3_palette_shader_desc(sg_query_backend());
            Vertex shader: vs
                Attribute slots:
                    ATTR_demo3_vs_position = 0
            Fragment shader: fs
                Image 'palette':
                    Type: SG_IMAGETYPE_2D
                    Component Type: SG_SAMPLERTYPE_FLOAT
                    Bind slot: SLOT_demo3_palette = 0
                Image 'screentexture':
                    Type: SG_IMAGETYPE_2D
                    Component Type: SG_SAMPLERTYPE_FLOAT
                    Bind slot: SLOT_demo3_screentexture = 1


    Shader descriptor structs:

        sg_shader palette = sg_make_shader(demo3_palette_shader_desc(sg_query_backend()));

    Vertex attribute locations for vertex shader 'vs':

        sg_pipeline pip = sg_make_pipeline(&(sg_pipeline_desc){
            .layout = {
                .attrs = {
                    [ATTR_demo3_vs_position] = { ... },
                },
            },
            ...});

    Image bind slots, use as index in sg_bindings.vs_images[] or .fs_images[]

        SLOT_demo3_palette = 0;
        SLOT_demo3_screentexture = 1;

*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#if !defined(SOKOL_SHDC_ALIGN)
  #if defined(_MSC_VER)
    #define SOKOL_SHDC_ALIGN(a) __declspec(align(a))
  #else
    #define SOKOL_SHDC_ALIGN(a) __attribute__((aligned(a)))
  #endif
#endif
#define ATTR_demo3_vs_position (0)
#define SLOT_demo3_palette (0)
#define SLOT_demo3_screentexture (1)
/*
    #version 100
    
    attribute vec4 position;
    varying vec2 uv0;
    
    void main()
    {
        gl_Position = position;
        uv0 = (position.xy * 0.5) + vec2(0.5);
    }
    
*/
static const char demo3_vs_source_glsl100[147] = {
    0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x31,0x30,0x30,0x0a,0x0a,0x61,0x74,
    0x74,0x72,0x69,0x62,0x75,0x74,0x65,0x20,0x76,0x65,0x63,0x34,0x20,0x70,0x6f,0x73,
    0x69,0x74,0x69,0x6f,0x6e,0x3b,0x0a,0x76,0x61,0x72,0x79,0x69,0x6e,0x67,0x20,0x76,
    0x65,0x63,0x32,0x20,0x75,0x76,0x30,0x3b,0x0a,0x0a,0x76,0x6f,0x69,0x64,0x20,0x6d,
    0x61,0x69,0x6e,0x28,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x67,0x6c,0x5f,0x50,
    0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x70,0x6f,0x73,0x69,0x74,0x69,
    0x6f,0x6e,0x3b,0x0a,0x20,0x20,0x20,0x20,0x75,0x76,0x30,0x20,0x3d,0x20,0x28,0x70,
    0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x2e,0x78,0x79,0x20,0x2a,0x20,0x30,0x2e,0x35,
    0x29,0x20,0x2b,0x20,0x76,0x65,0x63,0x32,0x28,0x30,0x2e,0x35,0x29,0x3b,0x0a,0x7d,
    0x0a,0x0a,0x00,
};
/*
    #version 100
    precision mediump float;
    precision highp int;
    
    uniform highp sampler2D palette;
    uniform highp sampler2D screentexture;
    
    varying highp vec2 uv0;
    
    void main()
    {
        gl_FragData[0] = texture2D(palette, vec2(texture2D(screentexture, uv0).x, 0.0));
    }
    
*/
static const char demo3_fs_source_glsl100[261] = {
    0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x31,0x30,0x30,0x0a,0x70,0x72,0x65,
    0x63,0x69,0x73,0x69,0x6f,0x6e,0x20,0x6d,0x65,0x64,0x69,0x75,0x6d,0x70,0x20,0x66,
    0x6c,0x6f,0x61,0x74,0x3b,0x0a,0x70,0x72,0x65,0x63,0x69,0x73,0x69,0x6f,0x6e,0x20,
    0x68,0x69,0x67,0x68,0x70,0x20,0x69,0x6e,0x74,0x3b,0x0a,0x0a,0x75,0x6e,0x69,0x66,
    0x6f,0x72,0x6d,0x20,0x68,0x69,0x67,0x68,0x70,0x20,0x73,0x61,0x6d,0x70,0x6c,0x65,
    0x72,0x32,0x44,0x20,0x70,0x61,0x6c,0x65,0x74,0x74,0x65,0x3b,0x0a,0x75,0x6e,0x69,
    0x66,0x6f,0x72,0x6d,0x20,0x68,0x69,0x67,0x68,0x70,0x20,0x73,0x61,0x6d,0x70,0x6c,
    0x65,0x72,0x32,0x44,0x20,0x73,0x63,0x72,0x65,0x65,0x6e,0x74,0x65,0x78,0x74,0x75,
    0x72,0x65,0x3b,0x0a,0x0a,0x76,0x61,0x72,0x79,0x69,0x6e,0x67,0x20,0x68,0x69,0x67,
    0x68,0x70,0x20,0x76,0x65,0x63,0x32,0x20,0x75,0x76,0x30,0x3b,0x0a,0x0a,0x76,0x6f,
    0x69,0x64,0x20,0x6d,0x61,0x69,0x6e,0x28,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,
    0x67,0x6c,0x5f,0x46,0x72,0x61,0x67,0x44,0x61,0x74,0x61,0x5b,0x30,0x5d,0x20,0x3d,
    0x20,0x74,0x65,0x78,0x74,0x75,0x72,0x65,0x32,0x44,0x28,0x70,0x61,0x6c,0x65,0x74,
    0x74,0x65,0x2c,0x20,0x76,0x65,0x63,0x32,0x28,0x74,0x65,0x78,0x74,0x75,0x72,0x65,
    0x32,0x44,0x28,0x73,0x63,0x72,0x65,0x65,0x6e,0x74,0x65,0x78,0x74,0x75,0x72,0x65,
    0x2c,0x20,0x75,0x76,0x30,0x29,0x2e,0x78,0x2c,0x20,0x30,0x2e,0x30,0x29,0x29,0x3b,
    0x0a,0x7d,0x0a,0x0a,0x00,
};
/*
    #version 300 es
    
    layout(location = 0) in vec4 position;
    out vec2 uv0;
    
    void main()
    {
        gl_Position = position;
        uv0 = (position.xy * 0.5) + vec2(0.5);
    }
    
*/
static const char demo3_vs_source_glsl300es[160] = {
    0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x33,0x30,0x30,0x20,0x65,0x73,0x0a,
    0x0a,0x6c,0x61,0x79,0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,
    0x20,0x3d,0x20,0x30,0x29,0x20,0x69,0x6e,0x20,0x76,0x65,0x63,0x34,0x20,0x70,0x6f,
    0x73,0x69,0x74,0x69,0x6f,0x6e,0x3b,0x0a,0x6f,0x75,0x74,0x20,0x76,0x65,0x63,0x32,
    0x20,0x75,0x76,0x30,0x3b,0x0a,0x0a,0x76,0x6f,0x69,0x64,0x20,0x6d,0x61,0x69,0x6e,
    0x28,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,
    0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x70,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x3b,
    0x0a,0x20,0x20,0x20,0x20,0x75,0x76,0x30,0x20,0x3d,0x20,0x28,0x70,0x6f,0x73,0x69,
    0x74,0x69,0x6f,0x6e,0x2e,0x78,0x79,0x20,0x2a,0x20,0x30,0x2e,0x35,0x29,0x20,0x2b,
    0x20,0x76,0x65,0x63,0x32,0x28,0x30,0x2e,0x35,0x29,0x3b,0x0a,0x7d,0x0a,0x0a,0x00,

};
/*
    #version 300 es
    precision mediump float;
    precision highp int;
    
    uniform highp sampler2D palette;
    uniform highp sampler2D screentexture;
    
    layout(location = 0) out highp vec4 frag_color;
    in highp vec2 uv0;
    
    void main()
    {
        frag_color = texture(palette, vec2(texture(screentexture, uv0).x, 0.0));
    }
    
*/
static const char demo3_fs_source_glsl300es[299] = {
    0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x33,0x30,0x30,0x20,0x65,0x73,0x0a,
    0x70,0x72,0x65,0x63,0x69,0x73,0x69,0x6f,0x6e,0x20,0x6d,0x65,0x64,0x69,0x75,0x6d,
    0x70,0x20,0x66,0x6c,0x6f,0x61,0x74,0x3b,0x0a,0x70,0x72,0x65,0x63,0x69,0x73,0x69,
    0x6f,0x6e,0x20,0x68,0x69,0x67,0x68,0x70,0x20,0x69,0x6e,0x74,0x3b,0x0a,0x0a,0x75,
    0x6e,0x69,0x66,0x6f,0x72,0x6d,0x20,0x68,0x69,0x67,0x68,0x70,0x20,0x73,0x61,0x6d,
    0x70,0x6c,0x65,0x72,0x32,0x44,0x20,0x70,0x61,0x6c,0x65,0x74,0x74,0x65,0x3b,0x0a,
    0x75,0x6e,0x69,0x66,0x6f,0x72,0x6d,0x20,0x68,0x69,0x67,0x68,0x70,0x20,0x73,0x61,
    0x6d,0x70,0x6c,0x65,0x72,0x32,0x44,0x20,0x73,0x63,0x72,0x65,0x65,0x6e,0x74,0x65,
    0x78,0x74,0x75,0x72,0x65,0x3b,0x0a,0x0a,0x6c,0x61,0x79,0x6f,0x75,0x74,0x28,0x6c,
    0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x30,0x29,0x20,0x6f,0x75,0x74,
    0x20,0x68,0x69,0x67,0x68,0x70,0x20,0x76,0x65,0x63,0x34,0x20,0x66,0x72,0x61,0x67,
    0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x69,0x6e,0x20,0x68,0x69,0x67,0x68,0x70,
    0x20,0x76,0x65,0x63,0x32,0x20,0x75,0x76,0x30,0x3b,0x0a,0x0a,0x76,0x6f,0x69,0x64,
    0x20,0x6d,0x61,0x69,0x6e,0x28,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,0x72,
    0x61,0x67,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x74,0x65,0x78,0x74,0x75,
    0x72,0x65,0x28,0x70,0x61,0x6c,0x65,0x74,0x74,0x65,0x2c,0x20,0x76,0x65,0x63,0x32,
    0x28,0x74,0x65,0x78,0x74,0x75,0x72,0x65,0x28,0x73,0x63,0x72,0x65,0x65,0x6e,0x74,
    0x65,0x78,0x74,0x75,0x72,0x65,0x2c,0x20,0x75,0x76,0x30,0x29,0x2e,0x78,0x2c,0x20,
    0x30,0x2e,0x30,0x29,0x29,0x3b,0x0a,0x7d,0x0a,0x0a,0x00,
};
#if !defined(SOKOL_GFX_INCLUDED)
  #error "Please include sokol_gfx.h before palette.glsl.webgl.h"
#endif
static inline const sg_shader_desc* demo3_palette_shader_desc(sg_backend backend) {
  if (backend == SG_BACKEND_GLES2) {
    static sg_shader_desc desc;
    static bool valid;
    if (!valid) {
      valid = true;
      desc.attrs[0].name = "position";
      desc.vs.source = demo3_vs_source_glsl100;
      desc.vs.entry = "main";
      desc.fs.source = demo3_fs_source_glsl100;
      desc.fs.entry = "main";
      desc.fs.images[0].name = "palette";
      desc.fs.images[0].image_type = SG_IMAGETYPE_2D;
      desc.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
      desc.fs.images[1].name = "screentexture";
      desc.fs.images[1].image_type = SG_IMAGETYPE_2D;
      desc.fs.images[1].sampler_type = SG_SAMPLERTYPE_FLOAT;
      desc.label = "demo3_palette_shader";
    }
    return &desc;
  }
  if (backend == SG_BACKEND_GLES3) {
    static sg_shader_desc desc;
    static bool valid;
    if (!valid) {
      valid = true;
      desc.attrs[0].name = "position";
      desc.vs.source = demo3_vs_source_glsl300es;
      desc.vs.entry = "main";
      desc.fs.source = demo3_fs_source_glsl300es;
      desc.fs.entry = "main";
      desc.fs.images[0].name = "palette";
      desc.fs.images[0].image_type = SG_IMAGETYPE_2D;
      desc.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
      desc.fs.images[1].name = "screentexture";
      desc.fs.images[1].image_type = SG_IMAGETYPE_2D;
      desc.fs.images[1].sampler_type = SG_SAMPLERTYPE_FLOAT;
      desc.label = "demo3_palette_shader";
    }
    return &desc;
  }
  return 0;
}
static inline int demo3_palette_attr_slot(const char* attr_name) {
  (void)attr_name;
  if (0 == strcmp(attr_name, "position")) {
    return 0;
  }
  return -1;
}
static inline int demo3_palette_image_slot(sg_shader_stage stage, const char* img_name) {
  (void)stage; (void)img_name;
  if (SG_SHADERSTAGE_FS == stage) {
    if (0 == strcmp(img_name, "palette")) {
      return 0;
    }
    if (0 == strcmp(img_name, "screentexture")) {
      return 1;
    }
  }
  return -1;
}
static inline int demo3_palette_uniformblock_slot(sg_shader_stage stage, const char* ub_name) {
  (void)stage; (void)ub_name;
  return -1;
}
static inline size_t demo3_palette_uniformblock_size(sg_shader_stage stage, const char* ub_name) {
  (void)stage; (void)ub_name;
  return 0;
}
static inline int demo3_palette_uniform_offset(sg_shader_stage stage, const char* ub_name, const char* u_name) {
  (void)stage; (void)ub_name; (void)u_name;
  return -1;
}
static inline sg_shader_uniform_desc demo3_palette_uniform_desc(sg_shader_stage stage, const char* ub_name, const char* u_name) {
  (void)stage; (void)ub_name; (void)u_name;
  #if defined(__cplusplus)
  sg_shader_uniform_desc desc = {};
  #else
  sg_shader_uniform_desc desc = {0};
  #endif
  return desc;
}
