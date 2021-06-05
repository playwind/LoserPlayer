#include "Shader.h"
#include <AndroidLogger.h>
#include <GLES2/gl2.h>

#define GET_STR(x) #x

// 顶点着色器
static const char *vertexShaderSource = GET_STR(
        attribute vec4 aPosition; // 顶点坐标
        attribute vec2 aTexCoord; //材质顶点坐标
        varying vec2 vTexCoord; // 输出的材质坐标
        void main() {
            vTexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
            gl_Position = aPosition;
        }
);

static const char *fragmentYUV420P = GET_STR(
        precision mediump float; // 精度
        varying vec2 vTexCoord; // 顶点着色器传递的坐标
        uniform sampler2D yTexture;
        uniform sampler2D uTexture;
        uniform sampler2D vTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            yuv.r = texture2D(yTexture, vTexCoord).r;
            yuv.g = texture2D(uTexture, vTexCoord).r - 0.5;
            yuv.b = texture2D(vTexture, vTexCoord).r - 0.5;
            rgb = mat3(1.0, 1.0, 1.0,
                       0.0, -0.39465, 2.03211,
                       1.13983, -0.58060, 0.0) * yuv;
            //输出像素颜色
            gl_FragColor = vec4(rgb, 1.0);
        }
);

//片元着色器,软解码和部分x86硬解码
static const char *fragNV12 = GET_STR(
        precision mediump float;    //精度
        varying vec2 vTexCoord;     //顶点着色器传递的坐标
        uniform sampler2D yTexture; //输入的材质（不透明灰度，单像素）
        uniform sampler2D uvTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            yuv.r = texture2D(yTexture, vTexCoord).r;
            yuv.g = texture2D(uvTexture, vTexCoord).r - 0.5;
            yuv.b = texture2D(uvTexture, vTexCoord).a - 0.5;
            rgb = mat3(1.0, 1.0, 1.0,
                       0.0, -0.39465, 2.03211,
                       1.13983, -0.58060, 0.0) * yuv;
            //输出像素颜色
            gl_FragColor = vec4(rgb, 1.0);
        }
);

//片元着色器,软解码和部分x86硬解码
static const char *fragNV21 = GET_STR(
        precision mediump float;    //精度
        varying vec2 vTexCoord;     //顶点着色器传递的坐标
        uniform sampler2D yTexture; //输入的材质（不透明灰度，单像素）
        uniform sampler2D uvTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            yuv.r = texture2D(yTexture, vTexCoord).r;
            yuv.g = texture2D(uvTexture, vTexCoord).a - 0.5;
            yuv.b = texture2D(uvTexture, vTexCoord).r - 0.5;
            rgb = mat3(1.0, 1.0, 1.0,
                       0.0, -0.39465, 2.03211,
                       1.13983, -0.58060, 0.0) * yuv;
            //输出像素颜色
            gl_FragColor = vec4(rgb, 1.0);
        }
);

static GLuint InitShader(const char *code, GLint type) {
    // 创建shader
    GLuint shader = glCreateShader(type);
    if (shader == 0) {
        LOGE("glCreateShader %d failed!!!", type);
        return 0;
    }

    // 加载shader
    glShaderSource(shader, 1, &code, nullptr);

    // 编译shader
    glCompileShader(shader);

    // 检查编译结果
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        LOGE("glCompileShader %d failed!!!", shader);
        return 0;
    }

    LOGI("glCompileShader %d success.", shader);

    return shader;
}

bool Shader::Init(ShaderType type) {
    Close();

    mutex.lock();

    // 顶点shader初始化
    vertexShader = InitShader(vertexShaderSource, GL_VERTEX_SHADER);
    if (vertexShader == 0) {
        mutex.unlock();
        LOGE("Init Vertex Shader failed!!!");
        return false;
    }

    switch (type) {
        case SHADER_NV12:
            fragmentShader = InitShader(fragNV12, GL_FRAGMENT_SHADER);
            break;
        case SHADER_NV21:
            fragmentShader = InitShader(fragNV21, GL_FRAGMENT_SHADER);
            break;
        case SHADER_YUV420P:
        default:
            fragmentShader = InitShader(fragmentYUV420P, GL_FRAGMENT_SHADER);
            break;
    }

    if (fragmentShader == 0) {
        mutex.unlock();
        LOGE("Init Fragment Shader failed!!!");
        return false;
    }

    // 创建渲染程序
    program = glCreateProgram();
    if (program == 0) {
        mutex.unlock();
        LOGE("glCreateProgram failed!!!");
        return false;
    }

    // 加入着色器到程序
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    // 链接程序
    glLinkProgram(program);

    // 检查链接状态
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        mutex.unlock();
        LOGE("glLinkProgram failed!!!");
        return false;
    }

    glUseProgram(program);

    // 顶点坐标
    static float vertexs[] = {
            1.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0,
            -1.0f, 1.0f, 0.0f
    };

    // 启用顶点
    GLuint aPosition = (GLuint) glGetAttribLocation(program, "aPosition");
    glEnableVertexAttribArray(aPosition);
    // 传递顶点
    glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, 12, vertexs);

    // 材质坐标
    static float txts[] = {
            1.0f, 0.0f,
            0.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
    };

    GLuint aTexCoord = glGetAttribLocation(program, "aTexCoord");
    glEnableVertexAttribArray(aTexCoord);
    glVertexAttribPointer(aTexCoord, 2, GL_FLOAT, GL_FALSE, 8, txts);

    glUniform1i(glGetUniformLocation(program, "yTexture"), 0);
    switch (type) {
        case SHADER_NV21:
        case SHADER_NV12:
            glUniform1i(glGetUniformLocation(program, "uvTexture"), 1);
            break;
        case SHADER_YUV420P:
        default:
            glUniform1i(glGetUniformLocation(program, "uTexture"), 1);
            glUniform1i(glGetUniformLocation(program, "vTexture"), 2);
            break;
    }

    mutex.unlock();
    return true;
}

void Shader::Close() {
    mutex.lock();

    if (program) {
        glDeleteProgram(program);
    }

    if (fragmentShader) {
        glDeleteShader(fragmentShader);
    }

    if (vertexShader) {
        glDeleteShader(vertexShader);
    }

    // 释放材质
    for (unsigned int texture : textures) {
        if (texture) {
            glDeleteTextures(1, &texture);
        }
        texture = 0;
    }

    mutex.unlock();
}

void Shader::GetTexture(unsigned int index, int width, int height, unsigned char *buf, bool isa) {
    unsigned int format = GL_LUMINANCE;
    if (isa) format = GL_LUMINANCE_ALPHA;

    mutex.lock();
    if (textures[index] == 0) {
        // 材质初始化
        glGenTextures(1, &textures[index]);
        glBindTexture(GL_TEXTURE_2D, textures[index]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // 设置纹理格式和大小
        glTexImage2D(GL_TEXTURE_2D,
                     0, //指定详细程度编号。级别0是基本图像级别。级别n是第n个缩略图缩小图像。
                     format, //指定纹理的内部格式。必须是下列符号常量之一：GL_ALPHA，GL_LUMINANCE，GL_LUMINANCE_ALPHA，GL_RGB，
                     width, //指定纹理图像的宽度。
                     height, //指定纹理图像的高度
                     0, //指定边框的宽度。必须为0。
                     format,
                     GL_UNSIGNED_BYTE, // 指定纹理数据的数据类型。
                     NULL);
    }

    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, textures[index]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, buf);
    mutex.unlock();
}

void Shader::Draw() {
    mutex.lock();
    if (!program) {
        mutex.unlock();
        return;
    }

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    mutex.unlock();
}
