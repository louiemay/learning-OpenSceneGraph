#include <Windows.h>
#include <osgViewer\Viewer>
#include <osg\Node>
#include <osg\Geode>
#include <osg\Group>
#include <osgUtil\Optimizer>
#include <osgGA\StateSetManipulator>
#include <osgGA\TerrainManipulator>
#include <osg\MatrixTransform>
#include <osg\Math>
#include <cmath>

#include <iostream>

// sign: 1.0 for outward facet normal, -1.0 for inward facet normal
osg::ref_ptr<deprecated_osg::Geometry> createCylinder(osg::Vec3& origin, float radius, float height,
	osg::Vec3& normal, float sign, int resolution)
{
	osg::ref_ptr<deprecated_osg::Geometry> geometry = new deprecated_osg::Geometry();

	// Vertex array
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
	for (int i = 0; i < resolution; ++i) {
		v->push_back(osg::Vec3(std::cos(osg::PI*i * 2 / resolution)*radius,
			std::sin(osg::PI*i * 2 / resolution)*radius, -0.5*height));
		v->push_back(osg::Vec3(std::cos(osg::PI*(i + 1) * 2 / resolution)*radius,
			std::sin(osg::PI*(i + 1) * 2 / resolution), -0.5*height));
		v->push_back(osg::Vec3(std::cos(osg::PI*(i + 1) * 2 / resolution)*radius,
			std::sin(osg::PI*(i + 1) * 2 / resolution), 0.5*height));
		v->push_back(osg::Vec3(std::cos(osg::PI*i * 2 / resolution)*radius,
			std::sin(osg::PI*i * 2 / resolution)*radius, 0.5*height));
	}
	geometry->setVertexArray(v.get());

	// Primitive set
	for (int i = 0; i < resolution; ++i) {
		osg::ref_ptr<osg::DrawElementsUInt> quad = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
		quad->push_back(4 * i + 0);
		quad->push_back(4 * i + 1);
		quad->push_back(4 * i + 2);
		quad->push_back(4 * i + 3);
		geometry->addPrimitiveSet(quad.get());
	}

	// Color array
	osg::ref_ptr<osg::Vec4Array> fcolor = new osg::Vec4Array();
	fcolor->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));

	// Color index array
	osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType, 4, 4>* colorIndex =
		new osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType, 4, 4>();
	for (int i = 0; i < resolution; ++i)
		colorIndex->push_back(0);
	geometry->setColorArray(fcolor.get());
	geometry->setColorIndices(colorIndex);
	geometry->setColorBinding(deprecated_osg::Geometry::BIND_PER_PRIMITIVE);    // bind color to primitive

	// Facet normal array
	osg::ref_ptr<osg::Vec3Array> fnormal = new osg::Vec3Array();
	for (int i = 0; i < resolution; ++i)
		fnormal->push_back(osg::Vec3(sign*std::cos(2 * (0.5 + i)*osg::PI / resolution),
			sign*std::sin(2 * (0.5 + i)*osg::PI / resolution), 0.0f));
	geometry->setNormalArray(fnormal.get());
	geometry->setNormalBinding(deprecated_osg::Geometry::BIND_PER_PRIMITIVE);    // bind facet normal to primitive

	return geometry.get();
}

int main()
{
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer();
	viewer->addEventHandler(new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet()));
	osg::ref_ptr<osg::Group> root = new osg::Group();

	osg::Vec3 origin(0.0f, 0.0f, 0.0f), normal(1.0f, 1.0f, 1.0f);
	float radius = 1.0f, height = 2.0f, sign = -1.0f;
	int resolution = 500;
	
	// Original geometry
	osg::ref_ptr<deprecated_osg::Geometry> geometry_origin = createCylinder(origin, radius, height, normal,
		1.0f, resolution);
	osg::ref_ptr<osg::Geode> geode_origin = new osg::Geode();
	geode_origin->addDrawable(geometry_origin.get());
	root->addChild(geode_origin.get());

	// Rotated geometry
	osg::ref_ptr<deprecated_osg::Geometry> geometry_rotate = createCylinder(origin, radius, height, normal,
		-1.0f, resolution);
	osg::ref_ptr<osg::Geode> geode_rotate = new osg::Geode();
	geode_rotate->addDrawable(geometry_rotate.get());
	// Set rotational matrix
	osg::ref_ptr<osg::MatrixTransform> rotate = new osg::MatrixTransform();
	rotate->setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(45.0f), 1, 0, 0));    // rotate 45 degrees aroung x-axis
	rotate->addChild(geode_rotate);
	root->addChild(rotate.get());

	osgUtil::Optimizer optimizer;
	optimizer.optimize(root.get());
	viewer->setSceneData(root.get());
	viewer->setCameraManipulator(new osgGA::TerrainManipulator());

	viewer->realize();
	osgViewer::GraphicsWindow* pwindow =
		dynamic_cast<osgViewer::GraphicsWindow*>(viewer->getCamera()->getGraphicsContext());
	if (pwindow) {
		pwindow->setWindowRectangle(100, 100, 1000, 800);
		pwindow->setWindowDecoration(true);
	}
	viewer->run();

	return true;
}


