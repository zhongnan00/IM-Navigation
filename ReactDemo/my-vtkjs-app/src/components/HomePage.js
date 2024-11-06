import React, { act,useState,useEffect, useRef } from 'react';
import ConeRender from './ConeRender';
import './HomePage.css'; // 样式文件
import STLViewer from './STLViewer';
import OBJViewer from './OBJViewer';
// import DICOMViewer from './DICOMViewer';
import { obj } from '@kitware/vtk.js/macros';


const HomePage = () => {

 // 定义一个 state 来存储当前显示的页面
  const [activePage, setActivePage] = useState('home');
  const stlVtkContainerRef = useRef(null);
  const stlObjContainerRef = useRef(null)
  let stlViewer;
  let objViewer;

  const stlUrl = '/Tools/960-553.stl'
  const objUrl = '/Tools/10x360-length,100mm2.obj'

  // 处理按钮点击事件的函数，更新 activePage 的值
  const handlePageChange = (page) => {
    setActivePage(page);

  };
  // 根据 activePage 的值渲染不同的页面

  useEffect(() => {
    if (activePage === 'tools') {
        
        if( stlVtkContainerRef.current){
            stlViewer = new STLViewer(stlVtkContainerRef.current);
            stlViewer.loadSTL(stlUrl);
            // stlViewer.renderer();
        }

        return () => {
            if(stlViewer){
                stlViewer.dispose();
            }
        }
    }

    if(activePage === 'calibration'){
        if(stlObjContainerRef.current){
            objViewer = new OBJViewer(stlObjContainerRef.current);
            objViewer.load(objUrl);
            
        }

        return() => {
            if(objViewer){
                objViewer.dispose();
            }
        }
    }

  }, [activePage]);


  // 页面渲染
  return (
    <div className="home">
      <header className="header">
        <div></div>
        <button onClick={() => handlePageChange('home')} className="selected header-item">Home</button>
        <button onClick={() => handlePageChange('tools')} className="header-item">Tools</button>
        <button onClick={() => handlePageChange('calibration')} className="header-item">Calibration</button>
        <button onClick={() => handlePageChange('navigation')} className="header-item">Navigation</button>
        <button onClick={() => handlePageChange('dicom')} className="header-item">Dicom</button>
      </header>

      <div className="main-content">
        <div className="sidebar">
          <div className="tool-card">
            <div className="tool-home">
              <button>Views</button>
              <button>Move</button>
              <button>Rotate</button>
              <button>Distance</button>
            </div>
            <div className="tool-home">
              <button>Screenshot</button>
              <button>Video</button>
              <button>Setting</button>
              <button>Lock</button>
            </div>
          </div>
        </div>

        <div className="content">
            {activePage === 'home' && 
                      <div id="view-item" style={{width: '100%', height: '100%', position: 'relative'}}>
                      <ConeRender />
                    </div>
            }
            {activePage === 'navigation' && 
            <div>navigation</div>
            }
            {activePage === 'tools' && 
                <div id="stl-vtk-container"  ref={stlVtkContainerRef} style={{ width: '100%', height: '100%', position: 'relative'}}>  </div>
              
            }
            {activePage === 'calibration' && 
                <div id="stl-obj-container" ref={stlObjContainerRef} style={{ width: '100%', height: '100%', position: 'relative'}}> </div>
            }
        </div>
      </div>

      <footer className="footer">
        <p>Status: Online</p>
      </footer>
    </div>
  );
};

export default HomePage;
