#include <osgViewer\Viewer>
#include <osg\Node>  // 结点
#include <osg\Geode>  // 叶结点
#include <osg\Group>  // 组结点

#include <osgDB\ReadFile>
#include <osgDB\WriteFile>

#include <osgUtil\Optimizer>

int main()
{
	// 创建Viewer对象，场景浏览器
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer();

	// 创建场景组结点
	osg::ref_ptr<osg::Group> root = new osg::Group();

	// 创建一个结点，读取牛的模型
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cow.osg");

	// 添加到场景
	root->addChild(node.get());

	// 优化场景数据
	osgUtil::Optimizer optimizer;
	optimizer.optimize(root.get());

	// 设置场景数据
	viewer->setSceneData(root.get());

	// 初始化并创建窗口
	viewer->realize();

	// 开始渲染
	viewer->run();

	return 0;



}





