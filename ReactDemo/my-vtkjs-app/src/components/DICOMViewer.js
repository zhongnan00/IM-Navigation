import '@kitware/vtk.js/favicon';
import '@kitware/vtk.js/Rendering/Profiles/Geometry';
import vtkRenderer from '@kitware/vtk.js/Rendering/Core/Renderer';
import vtkRenderWindow from '@kitware/vtk.js/Rendering/Core/RenderWindow';
import vtkOpenGLRenderWindow from '@kitware/vtk.js/Rendering/OpenGL/RenderWindow';
import vtkRenderWindowInteractor from '@kitware/vtk.js/Rendering/Core/RenderWindowInteractor';
import vtkDICOMImageReader from '@kitware/vtk.js/IO/Medical/DICOMImageReader';
import vtkVolume from '@kitware/vtk.js/Rendering/Core/Volume';
import vtkVolumeMapper from '@kitware/vtk.js/Rendering/Core/VolumeMapper';
import vtkColorTransferFunction from '@kitware/vtk.js/Common/Core/ColorTransferFunction';

class DICOMViewer {
  constructor(container) {
    this.container = container;
    this.renderWindow = vtkRenderWindow.newInstance();
    this.renderer = vtkRenderer.newInstance();
    this.openGLRenderWindow = vtkOpenGLRenderWindow.newInstance();
    this.interactor = vtkRenderWindowInteractor.newInstance();

    this.renderWindow.addRenderer(this.renderer);
    this.openGLRenderWindow.setContainer(container);
    this.renderWindow.addView(this.openGLRenderWindow);
    this.interactor.setView(this.openGLRenderWindow);
    this.interactor.initialize();

    this.reader = vtkDICOMImageReader.newInstance();
    this.volume = vtkVolume.newInstance();
    this.volumeMapper = vtkVolumeMapper.newInstance();

    // Set up volume properties
    this.colorTransferFunction = vtkColorTransferFunction.newInstance();
    this.colorTransferFunction.addRGBPoint(0, 0, 0, 0); // Background
    this.colorTransferFunction.addRGBPoint(255, 1, 1, 1); // Foreground

    this.volume.setMapper(this.volumeMapper);
    this.volume.getProperty().setRGBTransferFunction(0, this.colorTransferFunction);

    this.renderer.addVolume(this.volume);
  }

  loadDICOM(path) {
    this.reader.setUrl(path, { loadData: true }).then(() => {
      this.volumeMapper.setInputData(this.reader.getOutputData(0));
      this.renderer.resetCamera();
      this.renderWindow.render();
    });
  }

  render() {
    this.renderWindow.render();
  }

  resize() {
    this.openGLRenderWindow.resize();
  }

  dispose() {
    this.renderWindow.delete();
    this.openGLRenderWindow.delete();
    this.interactor.delete();
  }
}

export default DICOMViewer;
