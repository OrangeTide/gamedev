#pragma once
/*
    #version:1# (machine generated, don't edit!)

    Generated by sokol-shdc (https://github.com/floooh/sokol-tools)

    Cmdline: sokol-shdc --input basic.glsl --output basic.glsl.android.h --slang glsl300es --genver 1 --errfmt gcc --format sokol --bytecode --module hello --reflection

    Overview:

        Shader program 'basic':
            Get shader desc: hello_basic_shader_desc(sg_query_backend());
            Vertex shader: vs
                Attribute slots:
                    ATTR_hello_vs_position = 0
                    ATTR_hello_vs_texcoord = 1
                Uniform block 'mvp_matrix':
                    C struct: hello_mvp_matrix_t
                    Bind slot: SLOT_hello_mvp_matrix = 0
            Fragment shader: fs
                Image 'u_tex':
                    Type: SG_IMAGETYPE_2D
                    Component Type: SG_SAMPLERTYPE_FLOAT
                    Bind slot: SLOT_hello_u_tex = 0


    Shader descriptor structs:

        sg_shader basic = sg_make_shader(hello_basic_shader_desc(sg_query_backend()));

    Vertex attribute locations for vertex shader 'vs':

        sg_pipeline pip = sg_make_pipeline(&(sg_pipeline_desc){
            .layout = {
                .attrs = {
                    [ATTR_hello_vs_position] = { ... },
                    [ATTR_hello_vs_texcoord] = { ... },
                },
            },
            ...});

    Image bind slots, use as index in sg_bindings.vs_images[] or .fs_images[]

        SLOT_hello_u_tex = 0;

    Bind slot and C-struct for uniform block 'mvp_matrix':

        hello_mvp_matrix_t mvp_matrix = {
            .mvp = ...;
        };
        sg_apply_uniforms(SG_SHADERSTAGE_[VS|FS], SLOT_hello_mvp_matrix, &SG_RANGE(mvp_matrix));

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
#define ATTR_hello_vs_position (0)
#define ATTR_hello_vs_texcoord (1)
#define SLOT_hello_u_tex (0)
#define SLOT_hello_mvp_matrix (0)
#pragma pack(push,1)
SOKOL_SHDC_ALIGN(16) typedef struct hello_mvp_matrix_t {
    hmm_mat4 mvp;
} hello_mvp_matrix_t;
#pragma pack(pop)
/*
    #version 300 es
    
    uniform vec4 mvp_matrix[4];
    layout(location = 0) in vec3 position;
    out vec2 uv0;
    layout(location = 1) in vec2 texcoord;
    
    void main()
    {
        gl_Position = mat4(mvp_matrix[0], mvp_matrix[1], mvp_matrix[2], mvp_matrix[3]) * vec4(position, 1.0);
        uv0 = texcoord;
    }
    
*/
static const char hello_vs_source_glsl300es[282] = {
    0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x33,0x30,0x30,0x20,0x65,0x73,0x0a,
    0x0a,0x75,0x6e,0x69,0x66,0x6f,0x72,0x6d,0x20,0x76,0x65,0x63,0x34,0x20,0x6d,0x76,
    0x70,0x5f,0x6d,0x61,0x74,0x72,0x69,0x78,0x5b,0x34,0x5d,0x3b,0x0a,0x6c,0x61,0x79,
    0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x30,
    0x29,0x20,0x69,0x6e,0x20,0x76,0x65,0x63,0x33,0x20,0x70,0x6f,0x73,0x69,0x74,0x69,
    0x6f,0x6e,0x3b,0x0a,0x6f,0x75,0x74,0x20,0x76,0x65,0x63,0x32,0x20,0x75,0x76,0x30,
    0x3b,0x0a,0x6c,0x61,0x79,0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,
    0x6e,0x20,0x3d,0x20,0x31,0x29,0x20,0x69,0x6e,0x20,0x76,0x65,0x63,0x32,0x20,0x74,
    0x65,0x78,0x63,0x6f,0x6f,0x72,0x64,0x3b,0x0a,0x0a,0x76,0x6f,0x69,0x64,0x20,0x6d,
    0x61,0x69,0x6e,0x28,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x67,0x6c,0x5f,0x50,
    0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x6d,0x61,0x74,0x34,0x28,0x6d,
    0x76,0x70,0x5f,0x6d,0x61,0x74,0x72,0x69,0x78,0x5b,0x30,0x5d,0x2c,0x20,0x6d,0x76,
    0x70,0x5f,0x6d,0x61,0x74,0x72,0x69,0x78,0x5b,0x31,0x5d,0x2c,0x20,0x6d,0x76,0x70,
    0x5f,0x6d,0x61,0x74,0x72,0x69,0x78,0x5b,0x32,0x5d,0x2c,0x20,0x6d,0x76,0x70,0x5f,
    0x6d,0x61,0x74,0x72,0x69,0x78,0x5b,0x33,0x5d,0x29,0x20,0x2a,0x20,0x76,0x65,0x63,
    0x34,0x28,0x70,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x2c,0x20,0x31,0x2e,0x30,0x29,
    0x3b,0x0a,0x20,0x20,0x20,0x20,0x75,0x76,0x30,0x20,0x3d,0x20,0x74,0x65,0x78,0x63,
    0x6f,0x6f,0x72,0x64,0x3b,0x0a,0x7d,0x0a,0x0a,0x00,
};
/*
    #version 300 es
    precision mediump float;
    precision highp int;
    
    uniform highp sampler2D u_tex;
    
    layout(location = 0) out highp vec4 frag_color;
    in highp vec2 uv0;
    
    void main()
    {
        frag_color = vec4(texture(u_tex, uv0).xyz, 1.0);
    }
    
*/
static const char hello_fs_source_glsl300es[234] = {
    0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x33,0x30,0x30,0x20,0x65,0x73,0x0a,
    0x70,0x72,0x65,0x63,0x69,0x73,0x69,0x6f,0x6e,0x20,0x6d,0x65,0x64,0x69,0x75,0x6d,
    0x70,0x20,0x66,0x6c,0x6f,0x61,0x74,0x3b,0x0a,0x70,0x72,0x65,0x63,0x69,0x73,0x69,
    0x6f,0x6e,0x20,0x68,0x69,0x67,0x68,0x70,0x20,0x69,0x6e,0x74,0x3b,0x0a,0x0a,0x75,
    0x6e,0x69,0x66,0x6f,0x72,0x6d,0x20,0x68,0x69,0x67,0x68,0x70,0x20,0x73,0x61,0x6d,
    0x70,0x6c,0x65,0x72,0x32,0x44,0x20,0x75,0x5f,0x74,0x65,0x78,0x3b,0x0a,0x0a,0x6c,
    0x61,0x79,0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,
    0x20,0x30,0x29,0x20,0x6f,0x75,0x74,0x20,0x68,0x69,0x67,0x68,0x70,0x20,0x76,0x65,
    0x63,0x34,0x20,0x66,0x72,0x61,0x67,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x69,
    0x6e,0x20,0x68,0x69,0x67,0x68,0x70,0x20,0x76,0x65,0x63,0x32,0x20,0x75,0x76,0x30,
    0x3b,0x0a,0x0a,0x76,0x6f,0x69,0x64,0x20,0x6d,0x61,0x69,0x6e,0x28,0x29,0x0a,0x7b,
    0x0a,0x20,0x20,0x20,0x20,0x66,0x72,0x61,0x67,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x20,
    0x3d,0x20,0x76,0x65,0x63,0x34,0x28,0x74,0x65,0x78,0x74,0x75,0x72,0x65,0x28,0x75,
    0x5f,0x74,0x65,0x78,0x2c,0x20,0x75,0x76,0x30,0x29,0x2e,0x78,0x79,0x7a,0x2c,0x20,
    0x31,0x2e,0x30,0x29,0x3b,0x0a,0x7d,0x0a,0x0a,0x00,
};
#if !defined(SOKOL_GFX_INCLUDED)
  #error "Please include sokol_gfx.h before basic.glsl.android.h"
#endif
static inline const sg_shader_desc* hello_basic_shader_desc(sg_backend backend) {
  if (backend == SG_BACKEND_GLES3) {
    static sg_shader_desc desc;
    static bool valid;
    if (!valid) {
      valid = true;
      desc.attrs[0].name = "position";
      desc.attrs[1].name = "texcoord";
      desc.vs.source = hello_vs_source_glsl300es;
      desc.vs.entry = "main";
      desc.vs.uniform_blocks[0].size = 64;
      desc.vs.uniform_blocks[0].layout = SG_UNIFORMLAYOUT_STD140;
      desc.vs.uniform_blocks[0].uniforms[0].name = "mvp_matrix";
      desc.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
      desc.vs.uniform_blocks[0].uniforms[0].array_count = 4;
      desc.fs.source = hello_fs_source_glsl300es;
      desc.fs.entry = "main";
      desc.fs.images[0].name = "u_tex";
      desc.fs.images[0].image_type = SG_IMAGETYPE_2D;
      desc.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
      desc.label = "hello_basic_shader";
    }
    return &desc;
  }
  return 0;
}
static inline int hello_basic_attr_slot(const char* attr_name) {
  (void)attr_name;
  if (0 == strcmp(attr_name, "position")) {
    return 0;
  }
  if (0 == strcmp(attr_name, "texcoord")) {
    return 1;
  }
  return -1;
}
static inline int hello_basic_image_slot(sg_shader_stage stage, const char* img_name) {
  (void)stage; (void)img_name;
  if (SG_SHADERSTAGE_FS == stage) {
    if (0 == strcmp(img_name, "u_tex")) {
      return 0;
    }
  }
  return -1;
}
static inline int hello_basic_uniformblock_slot(sg_shader_stage stage, const char* ub_name) {
  (void)stage; (void)ub_name;
  if (SG_SHADERSTAGE_VS == stage) {
    if (0 == strcmp(ub_name, "mvp_matrix")) {
      return 0;
    }
  }
  return -1;
}
static inline size_t hello_basic_uniformblock_size(sg_shader_stage stage, const char* ub_name) {
  (void)stage; (void)ub_name;
  if (SG_SHADERSTAGE_VS == stage) {
    if (0 == strcmp(ub_name, "mvp_matrix")) {
      return sizeof(hello_mvp_matrix_t);
    }
  }
  return 0;
}
static inline int hello_basic_uniform_offset(sg_shader_stage stage, const char* ub_name, const char* u_name) {
  (void)stage; (void)ub_name; (void)u_name;
  if (SG_SHADERSTAGE_VS == stage) {
    if (0 == strcmp(ub_name, "mvp_matrix")) {
      if (0 == strcmp(u_name, "mvp")) {
        return 0;
      }
    }
  }
  return -1;
}
static inline sg_shader_uniform_desc hello_basic_uniform_desc(sg_shader_stage stage, const char* ub_name, const char* u_name) {
  (void)stage; (void)ub_name; (void)u_name;
  #if defined(__cplusplus)
  sg_shader_uniform_desc desc = {};
  #else
  sg_shader_uniform_desc desc = {0};
  #endif
  if (SG_SHADERSTAGE_VS == stage) {
    if (0 == strcmp(ub_name, "mvp_matrix")) {
      if (0 == strcmp(u_name, "mvp")) {
        desc.name = "mvp";
        desc.type = SG_UNIFORMTYPE_MAT4;
        desc.array_count = 1;
        return desc;
      }
    }
  }
  return desc;
}
