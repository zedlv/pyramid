#include "GLTools.h"
#include "GLShaderManager.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLFrame.h"
#include "GLMatrixStack.h"
#include "GLGeometryTransform.h"

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLShaderManager		shaderManager;
GLMatrixStack		modelViewMatrix;
GLMatrixStack		projectionMatrix;
GLFrame				cameraFrame;
GLFrame             objectFrame;
GLFrustum			viewFrustum;

GLBatch             pyramidBatch;

//纹理变量，一般使用无符号整型
GLuint              textureID;

GLGeometryTransform	transformPipeline;
M3DMatrix44f		shadowMatrix;

//绘制金字塔
void MakePyramid(GLBatch& pyramidBatch)
{
    
     /***前情导入
        
        2)设置纹理坐标
        void MultiTexCoord2f(GLuint texture, GLclampf s, GLclampf t);
        参数1：texture，纹理层次，对于使用存储着色器来进行渲染，设置为0
        参数2：s：对应顶点坐标中的x坐标
        参数3：t:对应顶点坐标中的y
        (s,t,r,q对应顶点坐标的x,y,z,w)
        
        pyramidBatch.MultiTexCoord2f(0,s,t);
        
        3)void Vertex3f(GLfloat x, GLfloat y, GLfloat z);
         void Vertex3fv(M3DVector3f vVertex);
        向三角形批次类添加顶点数据(x,y,z);
         pyramidBatch.Vertex3f(-1.0f, -1.0f, -1.0f);
       */
      pyramidBatch.Begin(GL_TRIANGLES, 18, 1);
    
   
    
      // 点位置
      M3DVector3f vApex = { 0.0f, 1.0f, 0.0f };
      M3DVector3f vFrontLeft = { -1.0f, -1.0f, 1.0f };
      M3DVector3f vFrontRight = { 1.0f, -1.0f, 1.0f };
      M3DVector3f vBackLeft = { -1.0f,  -1.0f, -1.0f };
      M3DVector3f vBackRight = { 1.0f,  -1.0f, -1.0f };
    
    
    
      //vBackLeft
      pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
      pyramidBatch.Vertex3fv(vBackLeft);
      
      //vBackRight
      pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
      pyramidBatch.Vertex3fv(vBackRight);
      
      //vFrontRight
      pyramidBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
      pyramidBatch.Vertex3fv(vFrontRight);
      
      
      //三角形Y =(vFrontLeft,vBackLeft,vFrontRight)
      //vFrontLeft
      pyramidBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
      pyramidBatch.Vertex3fv(vFrontLeft);
      
      //vBackLeft
      pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
      pyramidBatch.Vertex3fv(vBackLeft);
      
      //vFrontRight
      pyramidBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
      pyramidBatch.Vertex3fv(vFrontRight);

      
      // 金字塔前面
      //三角形：（Apex，vFrontLeft，vFrontRight）
      pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
      pyramidBatch.Vertex3fv(vApex);

      pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
      pyramidBatch.Vertex3fv(vFrontLeft);

      pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
      pyramidBatch.Vertex3fv(vFrontRight);
      
      //金字塔左边
      //三角形：（vApex, vBackLeft, vFrontLeft）
      pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
      pyramidBatch.Vertex3fv(vApex);
      
      pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
      pyramidBatch.Vertex3fv(vBackLeft);
      
      pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
      pyramidBatch.Vertex3fv(vFrontLeft);
      
      //金字塔右边
      //三角形：（vApex, vFrontRight, vBackRight）
      pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
      pyramidBatch.Vertex3fv(vApex);
      
      pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
      pyramidBatch.Vertex3fv(vFrontRight);

      pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
      pyramidBatch.Vertex3fv(vBackRight);
      
      //金字塔后边
      //三角形：（vApex, vBackRight, vBackLeft）
      pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
      pyramidBatch.Vertex3fv(vApex);
      
      pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
      pyramidBatch.Vertex3fv(vBackRight);
      
      pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
      pyramidBatch.Vertex3fv(vBackLeft);
      
      //结束批次设置
      pyramidBatch.End();
}

// 将TGA文件加载为2D纹理。
bool LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode)
{
    GLbyte *pBits;
    int nWidth,nHeight,nComponents;
    GLenum eFormat;
    
    //读取像素
    pBits = gltReadTGABits(szFileName, &nWidth, &nHeight, &nComponents, &eFormat);
    
    if(!pBits){
        return false;
    }
    
    //s设置纹理参数
    //1 纹理环绕
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    
    //2 过滤方式
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    
    // 3.载入纹理
    
    glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBits);
    free(pBits);
    
    if(minFilter == GL_LINEAR_MIPMAP_LINEAR ||
          minFilter == GL_LINEAR_MIPMAP_NEAREST ||
          minFilter == GL_NEAREST_MIPMAP_LINEAR ||
          minFilter == GL_NEAREST_MIPMAP_NEAREST)
    
      glGenerateMipmap(GL_TEXTURE_2D);
        
    return true;
}




void SetupRC()
{
    //清屏
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    //初始化
    shaderManager.InitializeStockShaders();
    //深度测试
    glEnable(GL_DEPTH_TEST);
    //1.分配纹理对象
    glGenTextures(1, &textureID);
    //2.绑定纹理状态
    glBindTexture(GL_TEXTURE_2D, textureID);
    //3.将TGA文件加载到2D纹理
    LoadTGATexture("stone.tga", GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR, GL_CLAMP_TO_EDGE);
    
    
    //4 创建金字塔
    MakePyramid(pyramidBatch);
    
    //5 调整相机
    cameraFrame.MoveForward(-10);
}



// 清理…例如删除纹理对象
void ShutdownRC(void)
{
    glDeleteTextures(1, &textureID);
}

void RenderScene(void)
{
   //1.灯泡 颜色值
    
    static GLfloat vLightPos [] = {1.0f, 1.0f, 0.0f};
    static GLfloat vWhite [] = {1.0f, 1.0f, 1.0f, 1.0f};
    
  //2.清除缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    //压榨出栈
    modelViewMatrix.PushMatrix();
    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);
    modelViewMatrix.MultMatrix(mCamera);
    
    M3DMatrix44f mObjectFrame;
    objectFrame.GetMatrix(mObjectFrame);
    modelViewMatrix.MultMatrix(mObjectFrame);
    
    //绑定纹理
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    //纹理着色器
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_REPLACE, transformPipeline.GetModelViewProjectionMatrix(), 0);
    pyramidBatch.Draw();
    
    modelViewMatrix.PopMatrix();
    
    glutSwapBuffers();
    
}



void SpecialKeys(int key, int x, int y)
{
    if(key == GLUT_KEY_UP)
        objectFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_DOWN)
        objectFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_LEFT)
        objectFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
    
    if(key == GLUT_KEY_RIGHT)
        objectFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
    
    glutPostRedisplay();
}





void ChangeSize(int w, int h)
{
    //1.设置视口
    glViewport(0, 0, w, h);
    viewFrustum.SetPerspective(35.0f, float(w) / float(h), 1.0f, 500.0f);
   projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());

   transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}


int main(int argc, char* argv[])
{
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Pyramid");
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialKeys);
    glutDisplayFunc(RenderScene);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    
    SetupRC();
    
    glutMainLoop();
    
    ShutdownRC();
    
    return 0;
}
