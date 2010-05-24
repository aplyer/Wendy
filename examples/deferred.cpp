
#include <wendy/Wendy.h>

using namespace wendy;

class Demo : public Trackable
{
public:
  ~Demo(void);
  bool init(void);
  void run(void);
private:
  bool render(void);
  input::MayaCamera controller;
  Ptr<deferred::Renderer> renderer;
  Ref<render::Camera> camera;
  scene::Graph graph;
  scene::Node* rootNode;
  scene::CameraNode* cameraNode;
  Timer timer;
  Time currentTime;
};

Demo::~Demo(void)
{
  graph.destroyRootNodes();

  camera = NULL;
  renderer = NULL;

  input::Context::destroy();
  GL::Renderer::destroy();
  GL::Context::destroy();
}

bool Demo::init(void)
{
  Image::addSearchPath(Path("media/deferred"));
  Mesh::addSearchPath(Path("media/deferred"));
  GL::Texture::addSearchPath(Path("media/deferred"));
  GL::VertexProgram::addSearchPath(Path("media/deferred"));
  GL::FragmentProgram::addSearchPath(Path("media/deferred"));
  GL::Program::addSearchPath(Path("media/deferred"));
  render::Material::addSearchPath(Path("media/deferred"));

  if (!GL::Context::create(GL::ContextMode()))
    return false;

  GL::Context* context = GL::Context::get();
  context->setTitle("Deferred Rendering");

  const unsigned int width = context->getScreenCanvas().getWidth();
  const unsigned int height = context->getScreenCanvas().getHeight();

  if (!GL::Renderer::create(*context))
    return false;

  renderer = deferred::Renderer::create(*context, deferred::Config(width, height));
  if (!renderer)
    return false;

  if (!input::Context::create(*context))
    return false;

  Ref<render::Mesh> mesh = render::Mesh::readInstance("cube");
  if (!mesh)
    return false;

  rootNode = new scene::Node();
  graph.addRootNode(*rootNode);

  RandomRange angle(0.f, M_PI * 2.f);
  RandomVolume axis(Vec3(-1.f, -1.f, -1.f), Vec3(1.f, 1.f, 1.f));
  RandomVolume position(Vec3(-2.f, -2.f, -2.f), Vec3(2.f, 2.f, 2.f));

  for (size_t i = 0;  i < 20;  i++)
  {
    scene::MeshNode* meshNode = new scene::MeshNode();
    meshNode->setMesh(mesh);
    meshNode->getLocalTransform().position = position.generate();
    meshNode->getLocalTransform().rotation.setAxisRotation(axis.generate().normalized(),
                                                           angle.generate());
    rootNode->addChild(*meshNode);
  }

  camera = new render::Camera();
  camera->setDepthRange(0.5f, 500.f);
  camera->setFOV(60.f);

  cameraNode = new scene::CameraNode();
  cameraNode->setCamera(camera);
  cameraNode->getLocalTransform().position.z = mesh->getBounds().radius * 3.f;
  graph.addRootNode(*cameraNode);

  scene::LightNode* lightNode;
  render::LightRef light;

  light = new render::Light();
  light->setColor(ColorRGB(1.f, 0.3f, 0.3f));
  light->setBounds(Sphere(Vec3::ZERO, 50.f));

  lightNode = new scene::LightNode();
  lightNode->getLocalTransform().rotation.setAxisRotation(Vec3::Y, M_PI / 2.f);
  lightNode->setLight(light);
  graph.addRootNode(*lightNode);

  light = new render::Light();
  light->setColor(ColorRGB(0.7f, 0.2f, 0.8f));
  light->setBounds(Sphere(Vec3::ZERO, 50.f));

  lightNode = new scene::LightNode();
  lightNode->getLocalTransform().rotation.setAxisRotation(Vec3::Y, -M_PI / 2.f);
  lightNode->setLight(light);
  graph.addRootNode(*lightNode);

  input::Context::get()->setFocus(&controller);

  timer.start();

  return true;
}

void Demo::run(void)
{
  GL::Context* context = GL::Context::get();

  render::Queue queue(*camera);

  do
  {
    currentTime = timer.getTime();

    rootNode->getLocalTransform().rotation.setAxisRotation(Vec3(0.f, 1.f, 0.f),
							   (float) currentTime);
    cameraNode->getLocalTransform() = controller.getTransform();

    graph.update();
    graph.enqueue(queue);

    context->clearDepthBuffer();
    context->clearColorBuffer(ColorRGBA::BLACK);

    renderer->render(queue);

    queue.destroyOperations();
    queue.detachLights();
  }
  while (context->update());
}

int main()
{
  if (!wendy::initialize())
    exit(1);

  Ptr<Demo> demo(new Demo());
  if (demo->init())
    demo->run();

  demo = NULL;

  wendy::shutdown();
  exit(0);
}

