import '@kitware/vtk.js/Rendering/Profiles/Geometry';
import vtkRenderWindow from '@kitware/vtk.js/Rendering/Core/RenderWindow';
import vtkRenderWindowInteractor from '@kitware/vtk.js/Rendering/Core/RenderWindowInteractor';
import vtkRenderer from '@kitware/vtk.js/Rendering/Core/Renderer';
import vtkActor from '@kitware/vtk.js/Rendering/Core/Actor';
import vtkMapper from '@kitware/vtk.js/Rendering/Core/Mapper';
import vtkSTLReader from '@kitware/vtk.js/IO/Geometry/STLReader';
import vtkOBJReader from '@kitware/vtk.js/IO/Misc/OBJReader';
import vtkOpenGLRenderWindow from '@kitware/vtk.js/Rendering/OpenGL/RenderWindow'; 
import vtkInteractorStyleTrackballCamera from '@kitware/vtk.js/Interaction/Style/InteractorStyleTrackballCamera';
import { act } from 'react';

class OBJViewer {
  constructor(container) {
    // 初始化渲染窗口
    this.renderWindow = vtkRenderWindow.newInstance();
    this.renderer = vtkRenderer.newInstance();
    this.renderWindow.addRenderer(this.renderer);

    // 创建与 div 容器的交互
    this.openGLRenderWindow = vtkOpenGLRenderWindow.newInstance();
    this.openGLRenderWindow.setContainer(container);
    this.renderWindow.addView(this.openGLRenderWindow);

    this.interactor = vtkRenderWindowInteractor.newInstance();
    this.interactor.setInteractorStyle(vtkInteractorStyleTrackballCamera.newInstance());
    this.interactor.setView(this.openGLRenderWindow);
    this.interactor.initialize();
    this.interactor.setContainer(container);

    // 设置背景颜色
    this.renderer.setBackground(0.1, 0.1, 0.1);
  }

  dispose() {
    // Remove all actors from the renderer
    if (this.renderer) {
      this.renderer.removeAllActors();
    }
  
    // Properly dispose of the interactor and views
    if (this.interactor) {
      this.interactor = null; // For garbage collection
    }
    
    if (this.openGLRenderWindow) {
      this.openGLRenderWindow.setContainer(null);
      this.openGLRenderWindow = null; // For garbage collection
    }
  
    // Clean up the render window
    if (this.renderWindow) {
      this.renderWindow.removeRenderer(this.renderer);
      this.renderWindow = null; // For garbage collection
    }
  
    this.renderer = null; // For garbage collection
  }
  

  load(url) {
    const reader = vtkOBJReader.newInstance();
    reader.setUrl(url).then(() => {
      const mapper = vtkMapper.newInstance();
      mapper.setInputData(reader.getOutputData());
      const actor = vtkActor.newInstance();
      actor.setMapper(mapper);
      actor.getProperty().setColor(1,0,1);    //grey color
    //   actor.getProperty().setEdgeVisibility(true);
    //   actor.getProperty().setSpecular(0.3); //光泽
    //   actor.getProperty().setSpecularPower(10); //高光集中度

      this.renderer.addActor(actor);
      this.renderer.resetCamera();
      this.renderWindow.render();
    }).catch(error => {
      console.error('Error loading STL file:', error);
    });
  
  
  }

  setBackground(r, g, b) {
    this.renderer.setBackground(r, g, b);
    this.renderWindow.render();
  }
}

export default OBJViewer;
